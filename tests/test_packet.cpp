#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/packet.h>

#include "create_message.h"

using ASAM::CMP::Packet;
using ASAM::CMP::Payload;
using ASAM::CMP::CanPayload;
using ASAM::CMP::EthernetPayload;
using ASAM::CMP::Decoder;

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
    static constexpr Payload::Type payloadType = Payload::Type::can;
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
    auto canHeader = reinterpret_cast<CanPayload::Header*>(dataMsg.data() + sizeof(Packet::MessageHeader));
    canHeader->setFlags(1);
    Packet packet(dataMsg.data(), dataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), Payload::Type::invalid);
}

TEST_F(PacketFixture, CanPayloadErrorPosition)
{
    auto canHeader = reinterpret_cast<CanPayload::Header*>(dataMsg.data() + sizeof(Packet::MessageHeader));
    canHeader->setErrorPosition(1);
    Packet packet(dataMsg.data(), dataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), Payload::Type::invalid);
}

TEST_F(PacketFixture, CanPayloadWrongDataLength)
{
    auto canHeader = reinterpret_cast<CanPayload::Header*>(dataMsg.data() + sizeof(Packet::MessageHeader));
    canHeader->setDataLength(canHeader->getDataLength() + 1);
    Packet packet(dataMsg.data(), dataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), Payload::Type::invalid);
}

TEST_F(PacketFixture, SegmentedPackets)
{
    constexpr size_t ethernetPacketSize = 1500 - sizeof(Decoder::CmpHeader);
    constexpr size_t payloadSize = ethernetPacketSize - sizeof(Packet::MessageHeader);
    constexpr size_t ethDataSize = payloadSize - sizeof(EthernetPayload::Header);
    constexpr Payload::Type payloadFormatEthernet = Payload::Type::ethernet;

    std::vector<uint8_t> ethData(ethDataSize);
    auto payloadMsg = createEthernetDataMessage(ethData);
    dataMsg = createDataMessage(payloadFormatEthernet, payloadMsg);
    ASSERT_EQ(dataMsg.size(), ethernetPacketSize);

    auto dataMessageHeader = reinterpret_cast<Packet::MessageHeader*>(dataMsg.data());
    dataMessageHeader->setSegmentType(Packet::SegmentType::firstSegment);

    Packet packet(dataMsg.data(), dataMsg.size());
    ASSERT_EQ(packet.getSegmentType(), Packet::SegmentType::firstSegment);

    dataMessageHeader->setSegmentType(Packet::SegmentType::intermediarySegment);
    bool add = packet.addSegment(dataMsg.data(), dataMsg.size());
    ASSERT_TRUE(add);
    ASSERT_EQ(packet.getSegmentType(), Packet::SegmentType::intermediarySegment);
    ASSERT_EQ(packet.getPayloadSize(), sizeof(EthernetPayload::Header) + 2 * ethDataSize);

    dataMessageHeader->setSegmentType(Packet::SegmentType::lastSegment);
    add = packet.addSegment(dataMsg.data(), dataMsg.size());
    ASSERT_TRUE(add);
    ASSERT_EQ(packet.getSegmentType(), Packet::SegmentType::lastSegment);
    ASSERT_EQ(packet.getPayloadSize(), sizeof(EthernetPayload::Header) + 3 * ethDataSize);
}
