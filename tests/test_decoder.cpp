#include <gtest/gtest.h>
#include <algorithm>
#include <numeric>

#include <asam_cmp/can_payload.h>
#include <asam_cmp/decoder.h>

#include "create_message.h"

using ASAM::CMP::CanPayload;
using ASAM::CMP::Decoder;
using ASAM::CMP::Payload;
using ASAM::CMP::swapEndian;

class DecoderFixture : public ::testing::Test
{
public:
    DecoderFixture()
    {
        canData.resize(canDataSize);
        std::iota(canData.begin(), canData.end(), 0);
        auto canMsg = createCanDataMessage(arbId, canData);
        dataMsg = createDataMessage(payloadTypeCan, canMsg);
        cmpMsg = createCmpMessage(deviceId, cmpMessageTypeData, streamId, dataMsg);
    }

protected:
    static constexpr size_t canDataSize = 8;
    static constexpr int32_t arbId = 33;
    static constexpr uint8_t payloadTypeCan = 0x01;
    static constexpr uint16_t deviceId = 3;
    static constexpr uint8_t cmpMessageTypeData = 0x01;
    static constexpr uint8_t streamId = 0x01;

protected:
    std::vector<uint8_t> canData;
    std::vector<uint8_t> cmpMsg;
    std::vector<uint8_t> dataMsg;
};

TEST_F(DecoderFixture, DataNull)
{
    Decoder decoder;
    auto packets = decoder.decode(nullptr, 0);
    ASSERT_TRUE(packets.empty());
}

TEST_F(DecoderFixture, WrongHeaderSize)
{
    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size() - 1);
    ASSERT_EQ(packets.size(), 0);
}

TEST_F(DecoderFixture, CorruptedDataMessage)
{
    auto dataMessageHeader = reinterpret_cast<DataMessageHeader*>(cmpMsg.data() + sizeof(CmpMessageHeader));
    uint16_t newSize = swapEndian(dataMessageHeader->payloadLength) + 1;
    dataMessageHeader->payloadLength = swapEndian(newSize);

    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
    ASSERT_EQ(packets.size(), 0);
}

TEST_F(DecoderFixture, MessageWithErrorFlag)
{
    auto dataMessageHeader = reinterpret_cast<DataMessageHeader*>(cmpMsg.data() + sizeof(CmpMessageHeader));
    dataMessageHeader->commonFlags |= errorInPayload;

    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
    ASSERT_EQ(packets.size(), 0);
}

TEST_F(DecoderFixture, CanMessage)
{
    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
    ASSERT_EQ(packets.size(), 1);

    auto packet = packets[0];
    auto& payload = packet->getPayload();
    ASSERT_EQ(static_cast<uint8_t>(payload.getType()), payloadTypeCan);

    auto& canPayload = static_cast<const CanPayload&>(payload);
    ASSERT_EQ(canPayload.getId(), arbId);
    ASSERT_EQ(canPayload.getDataLength(), canDataSize);
    ASSERT_TRUE(std::equal(canData.begin(), canData.end(), canPayload.getData()));
}

TEST_F(DecoderFixture, Aggregation)
{
    dataMsg.reserve(dataMsg.size() * 2);
    dataMsg.insert(dataMsg.end(), dataMsg.begin(), dataMsg.end());
    cmpMsg = createCmpMessage(deviceId, cmpMessageTypeData, streamId, dataMsg);

    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
    ASSERT_EQ(packets.size(), 2);
    ASSERT_EQ(packets[0]->getPayload().getType(), Payload::Type::can);
    ASSERT_EQ(packets[1]->getPayload().getType(), Payload::Type::can);
}

TEST_F(DecoderFixture, AggregationInvalidData)
{
    dataMsg.reserve(dataMsg.size() * 2);
    dataMsg.insert(dataMsg.end(), dataMsg.begin(), dataMsg.end());
    cmpMsg = createCmpMessage(deviceId, cmpMessageTypeData, streamId, dataMsg);

    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size() - 1);
    ASSERT_EQ(packets.size(), 1);
    ASSERT_EQ(packets[0]->getPayload().getType(), Payload::Type::can);
}

TEST_F(DecoderFixture, SegmentationOneStream)
{
    constexpr size_t ethernetPacketSize = 1500;
    constexpr size_t ethDataSize =
        ethernetPacketSize - sizeof(CmpMessageHeader) - sizeof(DataMessageHeader) - sizeof(EthernetDataMessageHeader);
    constexpr uint8_t payloadFormatEthernet = 0x08;
    constexpr size_t segmentCount = 4;

    std::vector<uint8_t> ethData(ethDataSize);
    auto ethPayload = createEthernetDataMessage(ethData);
    auto dataMsgEth = createDataMessage(payloadFormatEthernet, ethPayload);
    auto cmpMsgEth = createCmpMessage(deviceId, cmpMessageTypeData, streamId, dataMsgEth);
    ASSERT_EQ(cmpMsgEth.size(), ethernetPacketSize);

    auto dataMessageHeader = reinterpret_cast<DataMessageHeader*>(cmpMsgEth.data() + sizeof(CmpMessageHeader));
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::firstSegment;

    Decoder decoder;
    auto packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::intermediarySegment;

    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::lastSegment;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1);
    ASSERT_EQ(packets[0]->getPayloadSize(), sizeof(EthernetDataMessageHeader) + segmentCount * ethDataSize);
}

TEST_F(DecoderFixture, SegmentationMultipleStreams)
{
    constexpr size_t ethernetPacketSize = 1500;
    constexpr size_t ethDataSize =
        ethernetPacketSize - sizeof(CmpMessageHeader) - sizeof(DataMessageHeader) - sizeof(EthernetDataMessageHeader);
    constexpr uint8_t payloadFormatEthernet = 0x08;
    constexpr uint8_t streamId2 = streamId + 1;
    constexpr size_t segmentCount = 3;

    std::vector<uint8_t> ethData(ethDataSize);
    auto ethPayload = createEthernetDataMessage(ethData);
    auto dataMsgEth = createDataMessage(payloadFormatEthernet, ethPayload);
    auto cmpMsgEth = createCmpMessage(deviceId, cmpMessageTypeData, streamId, dataMsgEth);
    ASSERT_EQ(cmpMsgEth.size(), ethernetPacketSize);

    auto dataMessageHeader = reinterpret_cast<DataMessageHeader*>(cmpMsgEth.data() + sizeof(CmpMessageHeader));
    auto cmpMessageHeader = reinterpret_cast<CmpMessageHeader*>(cmpMsgEth.data());
    Decoder decoder;

    cmpMessageHeader->streamId = streamId;
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::firstSegment;
    auto packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->streamId = streamId2;
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::firstSegment;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->streamId = streamId;
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::intermediarySegment;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->streamId = streamId2;
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::intermediarySegment;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->streamId = streamId;
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::lastSegment;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1);
    ASSERT_EQ(packets[0]->getStreamId(), streamId);
    ASSERT_EQ(packets[0]->getPayloadSize(), sizeof(EthernetDataMessageHeader) + segmentCount * ethDataSize);

    cmpMessageHeader->streamId = streamId2;
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::lastSegment;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1);
    ASSERT_EQ(packets[0]->getStreamId(), streamId2);
    ASSERT_EQ(packets[0]->getPayloadSize(), sizeof(EthernetDataMessageHeader) + segmentCount * ethDataSize);
}

TEST_F(DecoderFixture, SegmentationMultipleDevices)
{
    constexpr size_t ethernetPacketSize = 1500;
    constexpr size_t ethDataSize =
        ethernetPacketSize - sizeof(CmpMessageHeader) - sizeof(DataMessageHeader) - sizeof(EthernetDataMessageHeader);
    constexpr uint8_t payloadFormatEthernet = 0x08;
    static constexpr uint16_t deviceId2 = deviceId + 1;
    constexpr size_t segmentCount = 3;

    std::vector<uint8_t> ethData(ethDataSize);
    auto ethPayload = createEthernetDataMessage(ethData);
    auto dataMsgEth = createDataMessage(payloadFormatEthernet, ethPayload);
    auto cmpMsgEth = createCmpMessage(deviceId, cmpMessageTypeData, streamId, dataMsgEth);
    ASSERT_EQ(cmpMsgEth.size(), ethernetPacketSize);

    auto dataMessageHeader = reinterpret_cast<DataMessageHeader*>(cmpMsgEth.data() + sizeof(CmpMessageHeader));
    auto cmpMessageHeader = reinterpret_cast<CmpMessageHeader*>(cmpMsgEth.data());
    Decoder decoder;

    cmpMessageHeader->deviceId = swapEndian(deviceId);
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::firstSegment;
    auto packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->deviceId = swapEndian(deviceId2);
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::firstSegment;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->deviceId = swapEndian(deviceId);
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::intermediarySegment;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->deviceId = swapEndian(deviceId2);
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::intermediarySegment;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->deviceId = swapEndian(deviceId);
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::lastSegment;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1);
    ASSERT_EQ(packets[0]->getDeviceId(), deviceId);
    ASSERT_EQ(packets[0]->getPayloadSize(), sizeof(EthernetDataMessageHeader) + segmentCount * ethDataSize);

    cmpMessageHeader->deviceId = swapEndian(deviceId2);
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::lastSegment;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1);
    ASSERT_EQ(packets[0]->getDeviceId(), deviceId2);
    ASSERT_EQ(packets[0]->getPayloadSize(), sizeof(EthernetDataMessageHeader) + segmentCount * ethDataSize);
}

TEST_F(DecoderFixture, SegmentedUnsegemnted)
{
    constexpr size_t ethernetPacketSize = 1500;
    constexpr size_t ethDataSize =
        ethernetPacketSize - sizeof(CmpMessageHeader) - sizeof(DataMessageHeader) - sizeof(EthernetDataMessageHeader);
    constexpr uint8_t payloadFormatEthernet = 0x08;
    static constexpr uint8_t streamId2 = streamId + 1;
    constexpr size_t segmentCount = 3;

    std::vector<uint8_t> ethData(ethDataSize);
    auto ethPayload = createEthernetDataMessage(ethData);
    auto dataMsgEth = createDataMessage(payloadFormatEthernet, ethPayload);
    auto cmpMsgEth = createCmpMessage(deviceId, cmpMessageTypeData, streamId, dataMsgEth);
    ASSERT_EQ(cmpMsgEth.size(), ethernetPacketSize);

    auto dataMessageHeader = reinterpret_cast<DataMessageHeader*>(cmpMsgEth.data() + sizeof(CmpMessageHeader));
    auto cmpMessageHeader = reinterpret_cast<CmpMessageHeader*>(cmpMsgEth.data());
    Decoder decoder;

    cmpMessageHeader->streamId = streamId;
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::unsegmented;
    auto packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1);
    ASSERT_EQ(packets[0]->getStreamId(), streamId);

    cmpMessageHeader->streamId = streamId2;
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::firstSegment;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->streamId = streamId;
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::unsegmented;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1);
    ASSERT_EQ(packets[0]->getStreamId(), streamId);

    cmpMessageHeader->streamId = streamId2;
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::intermediarySegment;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->streamId = streamId;
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::unsegmented;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1);
    ASSERT_EQ(packets[0]->getStreamId(), streamId);
    ASSERT_EQ(packets[0]->getPayloadSize(), ethPayload.size());

    cmpMessageHeader->streamId = streamId2;
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::lastSegment;
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1);
    ASSERT_EQ(packets[0]->getStreamId(), streamId2);
    ASSERT_EQ(packets[0]->getPayloadSize(), sizeof(EthernetDataMessageHeader) + segmentCount * ethDataSize);
}
