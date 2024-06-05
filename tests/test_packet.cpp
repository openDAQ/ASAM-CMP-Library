#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/packet.h>

#include "create_message.h"

using ASAM::CMP::CanPayload;
using ASAM::CMP::Decoder;
using ASAM::CMP::EthernetPayload;
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

TEST_F(PacketFixture, Copy)
{
    Packet packet(dataMsg.data(), dataMsg.size());
    Packet packetCopy(packet);

    ASSERT_TRUE(packet == packetCopy);
}

TEST_F(PacketFixture, CopyAssignment)
{
    Packet packet(dataMsg.data(), dataMsg.size());
    Packet packetCopy(dataMsg.data(), dataMsg.size() / 2);
    packetCopy = packet;

    ASSERT_TRUE(packet == packetCopy);
}

TEST_F(PacketFixture, Move)
{
    Packet packet(dataMsg.data(), dataMsg.size());
    Packet packetCopy(packet);

    Packet checker(std::move(packet));
    ASSERT_TRUE(checker == packetCopy);
    ASSERT_FALSE(checker == packet);
}

TEST_F(PacketFixture, MoveAssignment)
{
    Packet packet(dataMsg.data(), dataMsg.size());
    Packet packetCopy(packet);

    std::vector<uint8_t> checkerData(16, 0);
    Packet checker(checkerData.data(), checkerData.size());

    checker = std::move(packet);
    ASSERT_TRUE(checker == packetCopy);
    ASSERT_FALSE(checker == packet);
}
