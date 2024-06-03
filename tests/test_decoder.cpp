#include <gtest/gtest.h>
#include <algorithm>
#include <numeric>

#include <asam_cmp/can_payload.h>
#include <asam_cmp/decoder.h>

#include "create_message.h"

using ASAM::CMP::CanPayload;
using ASAM::CMP::Decoder;
using ASAM::CMP::EthernetPayload;
using ASAM::CMP::Packet;
using ASAM::CMP::Payload;

class DecoderFixture : public ::testing::Test
{
public:
    DecoderFixture()
    {
        canData.resize(canDataSize);
        std::iota(canData.begin(), canData.end(), uint8_t{});
        auto canMsg = createCanDataMessage(arbId, canData);
        dataMsg = createDataMessage(payloadTypeCan, canMsg);
        cmpMsg = createCmpMessage(deviceId, cmpMessageTypeData, streamId, dataMsg);
    }

    std::vector<uint8_t> createEthernetPacket(size_t size)
    {
        size_t ethDataSize = size - sizeof(Decoder::CmpHeader) - sizeof(Packet::MessageHeader) - sizeof(EthernetPayload::Header);

        std::vector<uint8_t> ethData(ethDataSize);
        auto ethPayload = createEthernetDataMessage(ethData);
        auto dataMsgEth = createDataMessage(Payload::Type::ethernet, ethPayload);
        return createCmpMessage(deviceId, cmpMessageTypeData, streamId, dataMsgEth);
    }

protected:
    static constexpr size_t canDataSize = 8;
    static constexpr uint32_t arbId = 33;
    static constexpr Payload::Type payloadTypeCan = Payload::Type::can;
    static constexpr uint16_t deviceId = 3;
    static constexpr uint8_t cmpMessageTypeData = 0x01;
    static constexpr uint8_t streamId = 0x01;

    static constexpr size_t ethernetPacketSize = 1500;

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
    ASSERT_EQ(packets.size(), 0u);
}

TEST_F(DecoderFixture, CorruptedDataMessage)
{
    auto dataMessageHeader = reinterpret_cast<Packet::MessageHeader*>(cmpMsg.data() + sizeof(Decoder::CmpHeader));
    dataMessageHeader->setPayloadLength(dataMessageHeader->getPayloadLength() + 1);

    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
    ASSERT_EQ(packets.size(), 0u);
}

TEST_F(DecoderFixture, MessageWithErrorFlag)
{
    auto dataMessageHeader = reinterpret_cast<Packet::MessageHeader*>(cmpMsg.data() + sizeof(Decoder::CmpHeader));
    dataMessageHeader->setCommonFlag(Packet::CommonFlags::errorInPayload, true);

    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
    ASSERT_EQ(packets.size(), 0u);
}

TEST_F(DecoderFixture, CanMessage)
{
    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
    ASSERT_EQ(packets.size(), 1u);

    auto packet = packets[0];
    auto& payload = packet->getPayload();
    ASSERT_EQ(payload.getType(), payloadTypeCan);

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
    ASSERT_EQ(packets.size(), 2u);
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
    ASSERT_EQ(packets.size(), 1u);
    ASSERT_EQ(packets[0]->getPayload().getType(), Payload::Type::can);
}

TEST_F(DecoderFixture, CreateEthernetPacket)
{
    auto packet = createEthernetPacket(ethernetPacketSize);
    ASSERT_EQ(packet.size(), ethernetPacketSize);
}

TEST_F(DecoderFixture, SegmentationOneStream)
{
    constexpr size_t segmentCount = 4;
    constexpr size_t ethDataSize =
        ethernetPacketSize - sizeof(Decoder::CmpHeader) - sizeof(Packet::MessageHeader) - sizeof(EthernetPayload::Header);
    constexpr size_t payloadSizeAll = (segmentCount) * (ethDataSize + sizeof(EthernetPayload::Header));

    auto cmpMsgEth = createEthernetPacket(ethernetPacketSize);
    auto cmpMessageHeader = reinterpret_cast<Decoder::CmpHeader*>(cmpMsgEth.data());
    cmpMessageHeader->setSequenceCounter(1);
    auto dataMessageHeader = reinterpret_cast<Packet::MessageHeader*>(cmpMsgEth.data() + sizeof(Decoder::CmpHeader));
    dataMessageHeader->setSegmentType(Packet::SegmentType::firstSegment);

    Decoder decoder;
    auto packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->setSequenceCounter(cmpMessageHeader->getSequenceCounter() + 1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::intermediarySegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->setSequenceCounter(cmpMessageHeader->getSequenceCounter() + 1);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->setSequenceCounter(cmpMessageHeader->getSequenceCounter() + 1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::lastSegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1u);
    ASSERT_EQ(packets[0]->getPayloadSize(), payloadSizeAll);
}

TEST_F(DecoderFixture, SegmentationMultipleStreams)
{
    constexpr uint8_t streamId2 = streamId + 1;
    constexpr size_t segmentCount = 3;
    constexpr size_t ethDataSize =
        ethernetPacketSize - sizeof(Decoder::CmpHeader) - sizeof(Packet::MessageHeader) - sizeof(EthernetPayload::Header);
    constexpr size_t payloadSizeAll = (segmentCount) * (ethDataSize + sizeof(EthernetPayload::Header));

    auto cmpMsgEth = createEthernetPacket(ethernetPacketSize);
    auto dataMessageHeader = reinterpret_cast<Packet::MessageHeader*>(cmpMsgEth.data() + sizeof(Decoder::CmpHeader));
    auto cmpMessageHeader = reinterpret_cast<Decoder::CmpHeader*>(cmpMsgEth.data());
    Decoder decoder;

    cmpMessageHeader->setStreamId(streamId);
    cmpMessageHeader->setSequenceCounter(1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::firstSegment);
    auto packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->setStreamId(streamId2);
    dataMessageHeader->setSegmentType(Packet::SegmentType::firstSegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->setStreamId(streamId);
    cmpMessageHeader->setSequenceCounter(cmpMessageHeader->getSequenceCounter() + 1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::intermediarySegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->setStreamId(streamId2);
    dataMessageHeader->setSegmentType(Packet::SegmentType::intermediarySegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->setStreamId(streamId);
    cmpMessageHeader->setSequenceCounter(cmpMessageHeader->getSequenceCounter() + 1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::lastSegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1u);
    ASSERT_EQ(packets[0]->getStreamId(), streamId);
    ASSERT_EQ(packets[0]->getPayloadSize(), payloadSizeAll);

    cmpMessageHeader->setStreamId(streamId2);
    dataMessageHeader->setSegmentType(Packet::SegmentType::lastSegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1u);
    ASSERT_EQ(packets[0]->getStreamId(), streamId2);
    ASSERT_EQ(packets[0]->getPayloadSize(), payloadSizeAll);
}

TEST_F(DecoderFixture, SegmentationMultipleDevices)
{
    static constexpr uint16_t deviceId2 = deviceId + 1;
    constexpr size_t segmentCount = 3;
    constexpr size_t ethDataSize =
        ethernetPacketSize - sizeof(Decoder::CmpHeader) - sizeof(Packet::MessageHeader) - sizeof(EthernetPayload::Header);
    constexpr size_t payloadSizeAll = (segmentCount) * (ethDataSize + sizeof(EthernetPayload::Header));

    auto cmpMsgEth = createEthernetPacket(ethernetPacketSize);
    auto dataMessageHeader = reinterpret_cast<Packet::MessageHeader*>(cmpMsgEth.data() + sizeof(Decoder::CmpHeader));
    auto cmpMessageHeader = reinterpret_cast<Decoder::CmpHeader*>(cmpMsgEth.data());
    Decoder decoder;

    cmpMessageHeader->setDeviceId(deviceId);
    cmpMessageHeader->setSequenceCounter(1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::firstSegment);
    auto packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->setDeviceId(deviceId2);
    dataMessageHeader->setSegmentType(Packet::SegmentType::firstSegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->setDeviceId(deviceId);
    cmpMessageHeader->setSequenceCounter(cmpMessageHeader->getSequenceCounter() + 1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::intermediarySegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->setDeviceId(deviceId2);
    dataMessageHeader->setSegmentType(Packet::SegmentType::intermediarySegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->setDeviceId(deviceId);
    cmpMessageHeader->setSequenceCounter(cmpMessageHeader->getSequenceCounter() + 1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::lastSegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1u);
    ASSERT_EQ(packets[0]->getDeviceId(), deviceId);
    ASSERT_EQ(packets[0]->getPayloadSize(), payloadSizeAll);

    cmpMessageHeader->setDeviceId(deviceId2);
    dataMessageHeader->setSegmentType(Packet::SegmentType::lastSegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1u);
    ASSERT_EQ(packets[0]->getDeviceId(), deviceId2);
    ASSERT_EQ(packets[0]->getPayloadSize(), payloadSizeAll);
}

TEST_F(DecoderFixture, SegmentedUnsegmented)
{
    static constexpr uint8_t streamId2 = streamId + 1;
    constexpr size_t segmentCount = 3;
    constexpr size_t ethDataSize =
        ethernetPacketSize - sizeof(Decoder::CmpHeader) - sizeof(Packet::MessageHeader) - sizeof(EthernetPayload::Header);
    constexpr size_t payloadSizeAll = (segmentCount) * (ethDataSize + sizeof(EthernetPayload::Header));

    auto cmpMsgEth = createEthernetPacket(ethernetPacketSize);
    auto dataMessageHeader = reinterpret_cast<Packet::MessageHeader*>(cmpMsgEth.data() + sizeof(Decoder::CmpHeader));
    auto cmpMessageHeader = reinterpret_cast<Decoder::CmpHeader*>(cmpMsgEth.data());
    Decoder decoder;

    cmpMessageHeader->setStreamId(streamId);
    dataMessageHeader->setSegmentType(Packet::SegmentType::unsegmented);
    auto packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1u);
    ASSERT_EQ(packets[0]->getStreamId(), streamId);

    cmpMessageHeader->setStreamId(streamId2);
    cmpMessageHeader->setSequenceCounter(1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::firstSegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->setStreamId(streamId);
    dataMessageHeader->setSegmentType(Packet::SegmentType::unsegmented);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1u);
    ASSERT_EQ(packets[0]->getStreamId(), streamId);

    cmpMessageHeader->setStreamId(streamId2);
    cmpMessageHeader->setSequenceCounter(cmpMessageHeader->getSequenceCounter() + 1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::intermediarySegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->setStreamId(streamId);
    dataMessageHeader->setSegmentType(Packet::SegmentType::unsegmented);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1u);
    ASSERT_EQ(packets[0]->getStreamId(), streamId);
    ASSERT_EQ(packets[0]->getPayloadSize(), ethDataSize + sizeof(EthernetPayload::Header));

    cmpMessageHeader->setStreamId(streamId2);
    cmpMessageHeader->setSequenceCounter(cmpMessageHeader->getSequenceCounter() + 1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::lastSegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1u);
    ASSERT_EQ(packets[0]->getStreamId(), streamId2);
    ASSERT_EQ(packets[0]->getPayloadSize(), payloadSizeAll);
}

TEST_F(DecoderFixture, SegmentedUnsegemntedSameStreams)
{
    auto cmpMsgEth = createEthernetPacket(ethernetPacketSize);
    auto dataMessageHeader = reinterpret_cast<Packet::MessageHeader*>(cmpMsgEth.data() + sizeof(Decoder::CmpHeader));
    auto cmpMessageHeader = reinterpret_cast<Decoder::CmpHeader*>(cmpMsgEth.data());
    Decoder decoder;

    cmpMessageHeader->setStreamId(streamId);
    dataMessageHeader->setSegmentType(Packet::SegmentType::unsegmented);
    auto packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1u);

    cmpMessageHeader->setSequenceCounter(1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::firstSegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    dataMessageHeader->setSegmentType(Packet::SegmentType::unsegmented);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 1u);
    ASSERT_EQ(packets[0]->getStreamId(), streamId);

    cmpMessageHeader->setSequenceCounter(cmpMessageHeader->getSequenceCounter() + 1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::lastSegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_EQ(packets.size(), 0u);
}

TEST_F(DecoderFixture, SegmentationWrongSequenceCounter)
{
    auto cmpMsgEth = createEthernetPacket(ethernetPacketSize);
    auto cmpMessageHeader = reinterpret_cast<Decoder::CmpHeader*>(cmpMsgEth.data());
    cmpMessageHeader->setSequenceCounter(1);
    auto dataMessageHeader = reinterpret_cast<Packet::MessageHeader*>(cmpMsgEth.data() + sizeof(Decoder::CmpHeader));
    dataMessageHeader->setSegmentType(Packet::SegmentType::firstSegment);

    Decoder decoder;
    auto packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    dataMessageHeader->setSegmentType(Packet::SegmentType::intermediarySegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());

    cmpMessageHeader->setSequenceCounter(cmpMessageHeader->getSequenceCounter() + 1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::lastSegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());
}

TEST_F(DecoderFixture, SegmentationWrongVersion)
{
    auto cmpMsgEth = createEthernetPacket(ethernetPacketSize);
    auto cmpMessageHeader = reinterpret_cast<Decoder::CmpHeader*>(cmpMsgEth.data());
    cmpMessageHeader->setSequenceCounter(1);
    auto dataMessageHeader = reinterpret_cast<Packet::MessageHeader*>(cmpMsgEth.data() + sizeof(Decoder::CmpHeader));
    dataMessageHeader->setSegmentType(Packet::SegmentType::firstSegment);

    Decoder decoder;
    auto packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->setSequenceCounter(cmpMessageHeader->getSequenceCounter() + 1);
    cmpMessageHeader->setVersion(cmpMessageHeader->getVersion() + 1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::lastSegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());
}

TEST_F(DecoderFixture, SegmentationWrongMessageType)
{
    auto cmpMsgEth = createEthernetPacket(ethernetPacketSize);
    auto cmpMessageHeader = reinterpret_cast<Decoder::CmpHeader*>(cmpMsgEth.data());
    cmpMessageHeader->setSequenceCounter(1);
    auto dataMessageHeader = reinterpret_cast<Packet::MessageHeader*>(cmpMsgEth.data() + sizeof(Decoder::CmpHeader));
    dataMessageHeader->setSegmentType(Packet::SegmentType::firstSegment);

    Decoder decoder;
    auto packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());

    cmpMessageHeader->setSequenceCounter(cmpMessageHeader->getSequenceCounter() + 1);
    cmpMessageHeader->setMessageType(cmpMessageHeader->getMessageType() + 1);
    dataMessageHeader->setSegmentType(Packet::SegmentType::lastSegment);
    packets = decoder.decode(cmpMsgEth.data(), cmpMsgEth.size());
    ASSERT_TRUE(packets.empty());
}
