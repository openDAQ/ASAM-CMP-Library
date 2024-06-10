#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/packet.h>
#include <asam_cmp/can_fd_payload.h>

#include "create_message.h"

using ASAM::CMP::CanPayload;
using ASAM::CMP::CanFdPayload;
using ASAM::CMP::Decoder;
using ASAM::CMP::EthernetPayload;
using ASAM::CMP::Packet;
using ASAM::CMP::Payload;
using ASAM::CMP::CmpHeader;
using ASAM::CMP::DataMessageHeader;
using PayloadType = DataMessageHeader::PayloadType;

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
    static constexpr PayloadType payloadType = PayloadType::can;
    static constexpr uint32_t arbId = 87;

protected:
    std::vector<uint8_t> dataMsg;
};

TEST_F(PacketFixture, Version)
{
    constexpr uint8_t newVersion = 99;

    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    packet.setVersion(newVersion);
    auto version = packet.getVersion();
    ASSERT_EQ(version, newVersion);
}

TEST_F(PacketFixture, DeviceId)
{
    constexpr uint8_t newId = 99;

    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    packet.setDeviceId(newId);
    auto id = packet.getDeviceId();
    ASSERT_EQ(id, newId);
}

TEST_F(PacketFixture, StreamId)
{
    constexpr uint8_t newId = 99;

    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    packet.setStreamId(newId);
    auto id = packet.getStreamId();
    ASSERT_EQ(id, newId);
}

TEST_F(PacketFixture, MessageTypeData)
{
    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    auto type = packet.getMessageType();
    ASSERT_EQ(type, CmpHeader::MessageType::data);
}

TEST_F(PacketFixture, CanPayload)
{
    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    const auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::can);
}

TEST_F(PacketFixture, CanFdPayload)
{
    std::vector<uint8_t> canMsg(canDataSize);
    const auto payloadMsg = createCanDataMessage(arbId, canMsg);
    dataMsg = createDataMessage(PayloadType::canFd, payloadMsg);
    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    const auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::canFd);
}

TEST_F(PacketFixture, AnalogPayload)
{
    std::vector<uint8_t> canMsg(canDataSize);
    const auto payloadMsg = createAnalogDataMessage(canMsg);
    dataMsg = createDataMessage(PayloadType::analog, payloadMsg);
    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    const auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::analog);
}

TEST_F(PacketFixture, EthernetPayload)
{
    std::vector<uint8_t> canMsg(canDataSize);
    const auto payloadMsg = createEthernetDataMessage(canMsg);
    dataMsg = createDataMessage(PayloadType::ethernet, payloadMsg);
    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    const auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::ethernet);
}

TEST_F(PacketFixture, UnknownDataMessagePayload)
{
    constexpr PayloadType unknowPayload = static_cast<PayloadType>(0xFE);
    std::vector<uint8_t> canMsg(canDataSize);
    const auto payloadMsg = createEthernetDataMessage(canMsg);
    dataMsg = createDataMessage(unknowPayload, payloadMsg);
    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    const auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), unknowPayload);
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
    const auto payloadMsg = createCaptureModuleDataMessage(deviceDescription, serialNumber, hardwareVersion, softwareVersion, data);
    const auto cmDataMsg = createDataMessage(PayloadType::cmStatMsg, payloadMsg);

    const Packet packet(CmpHeader::MessageType::status, cmDataMsg.data(), cmDataMsg.size());
    const auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::cmStatMsg);
}

TEST_F(PacketFixture, InterfacePayload)
{
    constexpr uint32_t interfaceId = 7777;
    const std::vector<uint8_t> streamIds = {11, 22, 33};
    const std::vector<uint8_t> vendorData = {3, 3, 3};

    const auto payloadMsg = createInterfaceDataMessage(interfaceId, streamIds, vendorData);
    dataMsg = createDataMessage(PayloadType::ifStatMsg, payloadMsg);
    Packet packet(CmpHeader::MessageType::status, dataMsg.data(), dataMsg.size());
    const auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::ifStatMsg);
}

TEST_F(PacketFixture, UnknownStatusMessagePayload)
{
    constexpr PayloadType unknowPayload = static_cast<PayloadType>(0xFE);
    std::vector<uint8_t> canMsg(canDataSize);
    auto payloadMsg = createEthernetDataMessage(canMsg);
    dataMsg = createDataMessage(unknowPayload, payloadMsg);
    Packet packet(CmpHeader::MessageType::status, dataMsg.data(), dataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), unknowPayload);
}

TEST_F(PacketFixture, CanPayloadErrorFlags)
{
    auto canHeader = reinterpret_cast<CanPayload::Header*>(dataMsg.data() + sizeof(DataMessageHeader));
    canHeader->setFlags(1);
    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::invalid);
}

TEST_F(PacketFixture, CanPayloadErrorPosition)
{
    auto canHeader = reinterpret_cast<CanPayload::Header*>(dataMsg.data() + sizeof(DataMessageHeader));
    canHeader->setErrorPosition(1);
    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::invalid);
}

TEST_F(PacketFixture, CanPayloadWrongDataLength)
{
    auto canHeader = reinterpret_cast<CanPayload::Header*>(dataMsg.data() + sizeof(DataMessageHeader));
    canHeader->setDataLength(canHeader->getDataLength() + 1);
    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    auto& payload = packet.getPayload();
    ASSERT_EQ(payload.getType(), PayloadType::invalid);
}

TEST_F(PacketFixture, Copy)
{
    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    Packet packetCopy(packet);

    ASSERT_TRUE(packet == packetCopy);
}

TEST_F(PacketFixture, CopyAssignment)
{
    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    Packet packetCopy(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size() / 2);
    packetCopy = packet;

    ASSERT_TRUE(packet == packetCopy);
}

TEST_F(PacketFixture, Move)
{
    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    Packet packetCopy(packet);

    Packet checker(std::move(packet));
    ASSERT_TRUE(checker == packetCopy);
    ASSERT_FALSE(checker == packet);
}

TEST_F(PacketFixture, MoveAssignment)
{
    Packet packet(CmpHeader::MessageType::data, dataMsg.data(), dataMsg.size());
    Packet packetCopy(packet);

    std::vector<uint8_t> checkerData(16, 0);
    Packet checker(CmpHeader::MessageType::data, checkerData.data(), checkerData.size());

    checker = std::move(packet);
    ASSERT_TRUE(checker == packetCopy);
    ASSERT_FALSE(checker == packet);
}
