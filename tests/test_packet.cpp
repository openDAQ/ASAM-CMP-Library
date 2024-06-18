#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/packet.h>
#include <asam_cmp/can_fd_payload.h>
#include <asam_cmp/interface_payload.h>
#include <asam_cmp/capture_module_payload.h>

#include "create_message.h"

using ASAM::CMP::CanPayload;
using ASAM::CMP::CanFdPayload;
using ASAM::CMP::Decoder;
using ASAM::CMP::EthernetPayload;
using ASAM::CMP::CaptureModulePayload;
using ASAM::CMP::InterfacePayload;
using ASAM::CMP::Packet;
using ASAM::CMP::Payload;
using ASAM::CMP::PayloadType;
using ASAM::CMP::CmpHeader;
using ASAM::CMP::MessageHeader;

class PacketFixture : public ::testing::Test
{
public:
    PacketFixture()
    {
        std::vector<uint8_t> canMsg(dataSize);
        canPayloadMsg = createCanDataMessage(arbId, canMsg);
        canDataMsg = createDataMessage(payloadType, canPayloadMsg);
    }

protected:
    static constexpr size_t dataSize = 8;
    static constexpr PayloadType payloadType = PayloadType::can;
    static constexpr uint32_t arbId = 87;

protected:
    std::vector<uint8_t> canDataMsg;
    std::vector<uint8_t> canPayloadMsg;
};

TEST_F(PacketFixture, DefaultConstructor)
{
    Packet p;
    ASSERT_FALSE(p.isValid());
}

TEST_F(PacketFixture, IsValid)
{
    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    ASSERT_TRUE(packet.isValid());
}

TEST_F(PacketFixture, Version)
{
    constexpr uint8_t newVersion = 99;

    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    packet.setVersion(newVersion);
    auto version = packet.getVersion();
    ASSERT_EQ(version, newVersion);
}

TEST_F(PacketFixture, DeviceId)
{
    constexpr uint8_t newId = 99;

    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    packet.setDeviceId(newId);
    auto id = packet.getDeviceId();
    ASSERT_EQ(id, newId);
}

TEST_F(PacketFixture, StreamId)
{
    constexpr uint8_t newId = 99;

    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    packet.setStreamId(newId);
    auto id = packet.getStreamId();
    ASSERT_EQ(id, newId);
}

TEST_F(PacketFixture, MessageTypeData)
{
    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    ASSERT_EQ(packet.getMessageType(), CmpHeader::MessageType::data);
}

TEST_F(PacketFixture, PayloadSize)
{
    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    ASSERT_EQ(packet.getPayloadSize(), canPayloadMsg.size());
}

TEST_F(PacketFixture, PayloadAccess)
{
    CanPayload canPayload(canPayloadMsg.data(), canPayloadMsg.size());
    Packet packet;
    packet.setPayload(canPayload);
    ASSERT_TRUE(packet.isValid());

    auto& newPayload = packet.getPayload();
    ASSERT_EQ(newPayload, canPayload);
}

TEST_F(PacketFixture, CanPayload)
{
    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    const auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::can);
}

TEST_F(PacketFixture, CanFdPayload)
{
    std::vector<uint8_t> canMsg(dataSize);
    const auto payloadMsg = createCanDataMessage(arbId, canMsg);
    canDataMsg = createDataMessage(PayloadType::canFd, payloadMsg);
    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    const auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::canFd);
}

TEST_F(PacketFixture, LinPayload)
{
    std::vector<uint8_t> canMsg(dataSize);
    const auto payloadMsg = createLinDataMessage(arbId, canMsg);
    canDataMsg = createDataMessage(PayloadType::lin, payloadMsg);
    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    const auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::lin);
}

TEST_F(PacketFixture, AnalogPayload)
{
    std::vector<uint8_t> canMsg(dataSize);
    const auto payloadMsg = createAnalogDataMessage(canMsg);
    canDataMsg = createDataMessage(PayloadType::analog, payloadMsg);
    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    const auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::analog);
}

TEST_F(PacketFixture, EthernetPayload)
{
    std::vector<uint8_t> canMsg(dataSize);
    const auto payloadMsg = createEthernetDataMessage(canMsg);
    canDataMsg = createDataMessage(PayloadType::ethernet, payloadMsg);
    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    const auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::ethernet);
}

TEST_F(PacketFixture, UnknownDataMessagePayload)
{
    constexpr PayloadType unknowPayload{CmpHeader::MessageType::data, 0xFE};

    std::vector<uint8_t> canMsg(dataSize);
    const auto payloadMsg = createEthernetDataMessage(canMsg);
    canDataMsg = createDataMessage(unknowPayload, payloadMsg);
    Packet packet(unknowPayload.getMessageType(), canDataMsg.data(), canDataMsg.size());
    const auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), unknowPayload);
}

TEST_F(PacketFixture, CaptureModulePayload)
{
    static constexpr std::string_view deviceDescription = "Device Description";
    static constexpr std::string_view serialNumber = "Serial Number";
    static constexpr std::string_view hardwareVersion = "Hardware Version";
    static constexpr std::string_view softwareVersion = "Software Version";

    std::vector<uint8_t> data(dataSize);
    std::iota(data.begin(), data.end(), uint8_t{0});
    const auto payloadMsg = createCaptureModuleDataMessage(deviceDescription, serialNumber, hardwareVersion, softwareVersion, data);
    const auto cmDataMsg = createDataMessage(PayloadType::cmStatMsg, payloadMsg);

    const Packet packet(CmpHeader::MessageType::status, cmDataMsg.data(), cmDataMsg.size());
    const auto& payload = static_cast<const CaptureModulePayload&>(packet.getPayload());
    ASSERT_EQ(packet.getMessageType(), CmpHeader::MessageType::status);
    ASSERT_EQ(payload.getType(), PayloadType::cmStatMsg);
    ASSERT_EQ(payload.getDeviceDescription(), deviceDescription);
}

TEST_F(PacketFixture, InterfacePayload)
{
    constexpr uint32_t interfaceId = 7777;
    const std::vector<uint8_t> streamIds = {11, 22, 33};
    const std::vector<uint8_t> vendorData = {3, 3, 3};

    const auto payloadMsg = createInterfaceDataMessage(interfaceId, streamIds, vendorData);
    canDataMsg = createDataMessage(PayloadType::ifStatMsg, payloadMsg);
    Packet packet(CmpHeader::MessageType::status, canDataMsg.data(), canDataMsg.size());
    const auto& payload = static_cast<const InterfacePayload&>(packet.getPayload());
    ASSERT_EQ(payload.getType(), PayloadType::ifStatMsg);
    ASSERT_EQ(payload.getStreamIdsCount(), streamIds.size());
}

TEST_F(PacketFixture, UnknownStatusMessagePayload)
{
    constexpr PayloadType unknowPayload{CmpHeader::MessageType::status, 0xFE};
    std::vector<uint8_t> canMsg(dataSize);
    auto payloadMsg = createEthernetDataMessage(canMsg);
    canDataMsg = createDataMessage(unknowPayload, payloadMsg);
    Packet packet(unknowPayload.getMessageType(), canDataMsg.data(), canDataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), unknowPayload);
}

TEST_F(PacketFixture, CanPayloadErrorFlags)
{
    auto canHeader = reinterpret_cast<CanPayload::Header*>(canDataMsg.data() + sizeof(MessageHeader));
    canHeader->setFlags(1);
    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::invalid);
}

TEST_F(PacketFixture, CanPayloadErrorPosition)
{
    auto canHeader = reinterpret_cast<CanPayload::Header*>(canDataMsg.data() + sizeof(MessageHeader));
    canHeader->setErrorPosition(1);
    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::invalid);
}

TEST_F(PacketFixture, CanPayloadWrongDataLength)
{
    auto canHeader = reinterpret_cast<CanPayload::Header*>(canDataMsg.data() + sizeof(MessageHeader));
    canHeader->setDataLength(canHeader->getDataLength() + 1);
    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::invalid);
}

TEST_F(PacketFixture, Copy)
{
    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    Packet packetCopy(packet);

    ASSERT_TRUE(packet == packetCopy);
}

TEST_F(PacketFixture, CopyAssignment)
{
    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    Packet packetCopy(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size() / 2);
    packetCopy = packet;

    ASSERT_TRUE(packet == packetCopy);
}

TEST_F(PacketFixture, Move)
{
    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    Packet packetCopy(packet);

    Packet checker(std::move(packet));
    ASSERT_TRUE(checker == packetCopy);
    ASSERT_FALSE(checker == packet);
}

TEST_F(PacketFixture, MoveAssignment)
{
    Packet packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
    Packet packetCopy(packet);

    std::vector<uint8_t> checkerData(16, 0);
    Packet checker(CmpHeader::MessageType::data, checkerData.data(), checkerData.size());

    checker = std::move(packet);
    ASSERT_TRUE(checker == packetCopy);
    ASSERT_FALSE(checker == packet);
}

TEST_F(PacketFixture, IsValidPacket)
{
    ASSERT_TRUE(Packet::isValidPacket(canDataMsg.data(), canDataMsg.size()));

    ASSERT_FALSE(Packet::isValidPacket(canDataMsg.data(), sizeof(MessageHeader) - 1));
    ASSERT_FALSE(Packet::isValidPacket(canDataMsg.data(), canDataMsg.size() - 1));

    auto msgHeader = reinterpret_cast<MessageHeader*>(canDataMsg.data());
    msgHeader->setCommonFlag(MessageHeader::CommonFlags::errorInPayload, true);
    ASSERT_FALSE(Packet::isValidPacket(canDataMsg.data(), canDataMsg.size()));

    msgHeader->setCommonFlag(MessageHeader::CommonFlags::errorInPayload, false);
    msgHeader->setPayloadType(0);
    ASSERT_FALSE(Packet::isValidPacket(canDataMsg.data(), canDataMsg.size()));
}
