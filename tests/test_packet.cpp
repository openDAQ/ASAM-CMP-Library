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

    Packet packet(Packet::MessageType::data, dataMsg.data(), dataMsg.size());
    packet.setVersion(newVersion);
    auto version = packet.getVersion();
    ASSERT_EQ(version, newVersion);
}

TEST_F(PacketFixture, DeviceId)
{
    constexpr uint8_t newId = 99;

    Packet packet(Packet::MessageType::data, dataMsg.data(), dataMsg.size());
    packet.setDeviceId(newId);
    auto id = packet.getDeviceId();
    ASSERT_EQ(id, newId);
}

TEST_F(PacketFixture, StreamId)
{
    constexpr uint8_t newId = 99;

    Packet packet(Packet::MessageType::data, dataMsg.data(), dataMsg.size());
    packet.setStreamId(newId);
    auto id = packet.getStreamId();
    ASSERT_EQ(id, newId);
}

TEST_F(PacketFixture, MessageTypeData)
{
    Packet packet(Packet::MessageType::data, dataMsg.data(), dataMsg.size());
    auto type = packet.getMessageType();
    ASSERT_EQ(type, Packet::MessageType::data);
}

TEST_F(PacketFixture, GetPayload)
{
    Packet packet(Packet::MessageType::data, dataMsg.data(), dataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), Payload::Type::can);
}

TEST_F(PacketFixture, CaptureModulePayload)
{
    static constexpr std::string_view deviceDescription = "Device Description";
    static constexpr std::string_view serialNumber = "Serial Number";
    static constexpr std::string_view hardwareVersion = "Hardware Version";
    static constexpr std::string_view softwareVersion = "Software Version";
    constexpr size_t dataSize = 32;

    std::vector<uint8_t> data(dataSize);
    std::iota(data.begin(), data.end(), uint8_t{0});
    auto payloadMsg = createCaptureModuleDataMessage(deviceDescription, serialNumber, hardwareVersion, softwareVersion, data);

    auto cmDataMsg = createDataMessage(Payload::Type::cmStatMsg, payloadMsg);

    Packet packet(Packet::MessageType::status, cmDataMsg.data(), cmDataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), Payload::Type::cmStatMsg);
}

TEST_F(PacketFixture, CanPayloadErrorFlags)
{
    auto canHeader = reinterpret_cast<CanPayload::Header*>(dataMsg.data() + sizeof(Packet::MessageHeader));
    canHeader->setFlags(1);
    Packet packet(Packet::MessageType::data, dataMsg.data(), dataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), Payload::Type::invalid);
}

TEST_F(PacketFixture, CanPayloadErrorPosition)
{
    auto canHeader = reinterpret_cast<CanPayload::Header*>(dataMsg.data() + sizeof(Packet::MessageHeader));
    canHeader->setErrorPosition(1);
    Packet packet(Packet::MessageType::data, dataMsg.data(), dataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), Payload::Type::invalid);
}

TEST_F(PacketFixture, CanPayloadWrongDataLength)
{
    auto canHeader = reinterpret_cast<CanPayload::Header*>(dataMsg.data() + sizeof(Packet::MessageHeader));
    canHeader->setDataLength(canHeader->getDataLength() + 1);
    Packet packet(Packet::MessageType::data, dataMsg.data(), dataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), Payload::Type::invalid);
}
