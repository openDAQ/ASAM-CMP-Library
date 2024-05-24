#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/packet.h>

#include "create_message.h"

using ASAM::CMP::Packet;
using ASAM::CMP::Payload;

class PacketFixture : public ::testing::Test
{
public:
    PacketFixture()
    {
        std::vector<uint8_t> canMsg(canDataSize);
        auto payloadMsg = createCanDataMessage(arbId, canMsg);
        dataMsg = createDataMessage(payloadType, payloadMsg);
    }

protected:
    static constexpr size_t canDataSize = 8;
    static constexpr uint8_t payloadType = 0x01;
    static constexpr uint32_t arbId = 87;

protected:
    std::vector<uint8_t> dataMsg;
};

TEST_F(PacketFixture, Version)
{
    constexpr uint8_t newVersion = 99;

    Packet packet(dataMsg.data(), dataMsg.size());
    packet.setVersion(newVersion);
    auto version = packet.getVersion();
    ASSERT_EQ(version, newVersion);
}

TEST_F(PacketFixture, DeviceId)
{
    constexpr uint8_t newId = 99;

    Packet packet(dataMsg.data(), dataMsg.size());
    packet.setDeviceId(newId);
    auto id = packet.getDeviceId();
    ASSERT_EQ(id, newId);
}

TEST_F(PacketFixture, StreamId)
{
    constexpr uint8_t newId = 99;

    Packet packet(dataMsg.data(), dataMsg.size());
    packet.setStreamId(newId);
    auto id = packet.getStreamId();
    ASSERT_EQ(id, newId);
}

TEST_F(PacketFixture, MessageTypeData)
{
    Packet packet(dataMsg.data(), dataMsg.size());
    auto type = packet.getMessageType();
    ASSERT_EQ(type, Packet::MessageType::Data);
}

TEST_F(PacketFixture, GetPayload)
{
    Packet packet(dataMsg.data(), dataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), Payload::Type::can);
}

TEST_F(PacketFixture, CanPayloadErrorFlags)
{
    auto canHeader = reinterpret_cast<CanDataMessageHeader*>(dataMsg.data() + sizeof(DataMessageHeader));
    canHeader->flags = 1;
    Packet packet(dataMsg.data(), dataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), Payload::Type::invalid);
}

TEST_F(PacketFixture, CanPayloadErrorPosition)
{
    auto canHeader = reinterpret_cast<CanDataMessageHeader*>(dataMsg.data() + sizeof(DataMessageHeader));
    canHeader->errorPosition = 1;
    Packet packet(dataMsg.data(), dataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), Payload::Type::invalid);
}

TEST_F(PacketFixture, CanPayloadWrongDataLength)
{
    auto canHeader = reinterpret_cast<CanDataMessageHeader*>(dataMsg.data() + sizeof(DataMessageHeader));
    ++canHeader->dataLength;
    Packet packet(dataMsg.data(), dataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), Payload::Type::invalid);
}

TEST_F(PacketFixture, SegmentedPackets)
{
    constexpr size_t ethernetPacketSize = 1500 - sizeof(CmpMessageHeader);
    constexpr size_t payloadSize = ethernetPacketSize - sizeof(DataMessageHeader);
    constexpr size_t ethDataSize = payloadSize - sizeof(EthernetDataMessageHeader);
    constexpr uint8_t payloadFormatEthernet = 0x08;

    std::vector<uint8_t> ethData(ethDataSize);
    auto payloadMsg = createEthernetDataMessage(ethData);
    dataMsg = createDataMessage(payloadFormatEthernet, payloadMsg);
    ASSERT_EQ(dataMsg.size(), ethernetPacketSize);

    auto dataMessageHeader = reinterpret_cast<DataMessageHeader*>(dataMsg.data());
    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::firstSegment;

    Packet packet(dataMsg.data(), dataMsg.size());
    ASSERT_EQ(packet.getSegmentType(), Packet::SegmentType::firstSegment);

    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::intermediarySegment;
    bool add = packet.addSegment(dataMsg.data(), dataMsg.size());
    ASSERT_TRUE(add);
    ASSERT_EQ(packet.getSegmentType(), Packet::SegmentType::intermediarySegment);
    ASSERT_EQ(packet.getPayloadSize(), sizeof(EthernetDataMessageHeader) + 2 * ethDataSize);

    dataMessageHeader->commonFlagsFields.seg = DataMessageHeader::SegmentType::lastSegment;
    add = packet.addSegment(dataMsg.data(), dataMsg.size());
    ASSERT_TRUE(add);
    ASSERT_EQ(packet.getSegmentType(), Packet::SegmentType::lastSegment);
    ASSERT_EQ(packet.getPayloadSize(), sizeof(EthernetDataMessageHeader) + 3 * ethDataSize);
}
