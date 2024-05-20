#include <gtest/gtest.h>
#include <asam_cmp/packet.h>

#include "create_message.h"

using ASAM::CMP::Packet;
using ASAM::CMP::Payload;
using PacketTest = testing::Test;

TEST_F(PacketTest, Construct)
{
    uint8_t buf = 55;
    Packet packet(&buf, 1);
}

TEST_F(PacketTest, EmptyPacket)
{
    Packet packet(nullptr, 0);
}

TEST_F(PacketTest, Version)
{
    constexpr uint8_t newVersion = 99;

    Packet packet(nullptr, 0);
    packet.setVersion(newVersion);
    auto version = packet.getVersion();
    ASSERT_EQ(version, newVersion);
}

TEST_F(PacketTest, DeviceId)
{
    constexpr uint8_t newId = 99;

    Packet packet(nullptr, 0);
    packet.setDeviceId(newId);
    auto id = packet.getDeviceId();
    ASSERT_EQ(id, newId);
}

TEST_F(PacketTest, StreamId)
{
    constexpr uint8_t newId = 99;

    Packet packet(nullptr, 0);
    packet.setStreamId(newId);
    auto id = packet.getStreamId();
    ASSERT_EQ(id, newId);
}

TEST_F(PacketTest, MessageTypeData)
{
    Packet packet(nullptr, 0);
    auto type = packet.getMessageType();
    ASSERT_EQ(type, Packet::MessageType::Data);
}

TEST_F(PacketTest, GetPayload)
{
    Packet packet(nullptr, 0);
    auto payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), Payload::Type::invalid);
}

