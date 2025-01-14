#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/can_fd_payload.h>
#include <asam_cmp/capture_module_payload.h>
#include <asam_cmp/common.h>
#include <asam_cmp/interface_payload.h>
#include <asam_cmp/packet.h>

#include "create_message.h"

using ASAM::CMP::CanFdPayload;
using ASAM::CMP::CanPayload;
using ASAM::CMP::CaptureModulePayload;
using ASAM::CMP::CmpHeader;
using ASAM::CMP::Decoder;
using ASAM::CMP::EthernetPayload;
using ASAM::CMP::InterfacePayload;
using ASAM::CMP::MessageHeader;
using ASAM::CMP::Packet;
using ASAM::CMP::Payload;
using ASAM::CMP::PayloadType;
using ASAM::CMP::swapEndian;

class PacketFixture : public ::testing::Test
{
public:
    PacketFixture()
    {
        std::vector<uint8_t> canMsg(dataSize);
        canPayloadMsg = createCanDataMessage(arbId, canMsg);
        canDataMsg = createDataMessage(payloadType, canPayloadMsg);
        canPacket = Packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());

        anotherPacket = Packet(CmpHeader::MessageType::data, canDataMsg.data(), canDataMsg.size());
        anotherPacket.setVersion(anotherPacket.getVersion() + 1);
        anotherPacket.setDeviceId(anotherPacket.getDeviceId() + 1);
        anotherPacket.setStreamId(anotherPacket.getStreamId() + 1);
        anotherPacket.setSequenceCounter(anotherPacket.getSequenceCounter() + 1);
        anotherPacket.setTimestamp(anotherPacket.getTimestamp() + 1);
        anotherPacket.setInterfaceId(anotherPacket.getInterfaceId() + 1);
        anotherPacket.setVendorId(anotherPacket.getVendorId() + 1);
        anotherPacket.setCommonFlags(anotherPacket.getCommonFlags() + 1);
        anotherPacket.setSegmentType(MessageHeader::SegmentType::lastSegment);
    }

protected:
    template <typename T>
    using SetterFunc = void (Packet::*)(T);

    template <typename T>
    using GetterFunc = T (Packet::*)() const;

    template <typename T>
    bool TestSetterGetter(SetterFunc<T> setter, GetterFunc<T> getter, const T newValue)
    {
        (canPacket.*setter)(newValue);
        T value = (canPacket.*getter)();
        return (value == newValue);
    }

protected:
    static constexpr size_t dataSize = 8;
    static constexpr PayloadType payloadType = PayloadType::can;
    static constexpr uint32_t arbId = 87;

protected:
    std::vector<uint8_t> canDataMsg;
    std::vector<uint8_t> canPayloadMsg;
    Packet canPacket;
    Packet anotherPacket;
};

TEST_F(PacketFixture, DefaultConstructor)
{
    Packet p;
    ASSERT_FALSE(p.isValid());
}

TEST_F(PacketFixture, Copy)
{
    Packet packetCopy(anotherPacket);

    ASSERT_TRUE(anotherPacket == packetCopy);
}

TEST_F(PacketFixture, CopyEmptyPacket)
{
    Packet emptyPacket;
    Packet packetCopy(emptyPacket);

    ASSERT_TRUE(emptyPacket == packetCopy);
}

TEST_F(PacketFixture, CopyAssignment)
{
    Packet packetCopy;
    packetCopy = anotherPacket;

    ASSERT_TRUE(anotherPacket == packetCopy);
}

TEST_F(PacketFixture, Move)
{
    Packet packetCopy(anotherPacket);

    Packet checker(std::move(anotherPacket));
    ASSERT_TRUE(checker == packetCopy);
    ASSERT_FALSE(checker == anotherPacket);
}

TEST_F(PacketFixture, MoveAssignment)
{
    Packet packetCopy(anotherPacket);

    Packet checker;

    checker = std::move(anotherPacket);
    ASSERT_TRUE(checker == packetCopy);
    ASSERT_FALSE(checker == anotherPacket);
}

TEST_F(PacketFixture, IsValid)
{
    ASSERT_TRUE(canPacket.isValid());
}

TEST_F(PacketFixture, Version)
{
    ASSERT_TRUE(TestSetterGetter(&Packet::setVersion, &Packet::getVersion, uint8_t{99}));
}

TEST_F(PacketFixture, DeviceId)
{
    ASSERT_TRUE(TestSetterGetter(&Packet::setDeviceId, &Packet::getDeviceId, uint16_t{99}));
}

TEST_F(PacketFixture, MessageTypeData)
{
    ASSERT_EQ(canPacket.getMessageType(), CmpHeader::MessageType::data);
}

TEST_F(PacketFixture, StreamId)
{
    ASSERT_TRUE(TestSetterGetter(&Packet::setStreamId, &Packet::getStreamId, uint8_t{99}));
}

TEST_F(PacketFixture, SequenceCounter)
{
    ASSERT_TRUE(TestSetterGetter(&Packet::setSequenceCounter, &Packet::getSequenceCounter, uint16_t{99}));
}

TEST_F(PacketFixture, RawCmpHeader)
{
    constexpr uint8_t version = 2;
    constexpr uint16_t deviceId = 3;
    constexpr uint8_t streamId = 4;
    constexpr uint16_t sequenceCounter = 5;

    canPacket.setVersion(version);
    canPacket.setDeviceId(deviceId);
    canPacket.setStreamId(streamId);
    canPacket.setSequenceCounter(sequenceCounter);

    CmpHeader header;
    canPacket.getRawCmpHeader(&header);
    ASSERT_EQ(header.getVersion(), version);
    ASSERT_EQ(header.getDeviceId(), deviceId);
    ASSERT_EQ(header.getMessageType(), CmpHeader::MessageType::data);
    ASSERT_EQ(header.getStreamId(), streamId);
    ASSERT_EQ(header.getSequenceCounter(), sequenceCounter);
}

TEST_F(PacketFixture, Timestamp)
{
    ASSERT_TRUE(TestSetterGetter(&Packet::setTimestamp, &Packet::getTimestamp, uint64_t{99}));
}

TEST_F(PacketFixture, InterfaceId)
{
    ASSERT_TRUE(TestSetterGetter(&Packet::setInterfaceId, &Packet::getInterfaceId, uint32_t{99}));
}

TEST_F(PacketFixture, VendorId)
{
    ASSERT_TRUE(TestSetterGetter(&Packet::setVendorId, &Packet::getVendorId, uint16_t{99}));
}

TEST_F(PacketFixture, CommonFlags)
{
    ASSERT_TRUE(TestSetterGetter(&Packet::setCommonFlags, &Packet::getCommonFlags, uint8_t{99}));
}

TEST_F(PacketFixture, SegmentType)
{
    ASSERT_TRUE(TestSetterGetter(&Packet::setSegmentType, &Packet::getSegmentType, MessageHeader::SegmentType::intermediarySegment));
}

TEST_F(PacketFixture, PayloadType)
{
    ASSERT_EQ(canPacket.getPayloadType(), payloadType.getRawPayloadType());
}

TEST_F(PacketFixture, PayloadLength)
{
    ASSERT_EQ(canPacket.getPayloadLength(), canPayloadMsg.size());
}

TEST_F(PacketFixture, RawMessageHeader)
{
    constexpr uint64_t timestamp = 11;
    constexpr uint32_t interfaceId = 22;
    constexpr uint8_t commonFlags = 33;
    constexpr MessageHeader::SegmentType segmentType = MessageHeader::SegmentType::lastSegment;

    canPacket.setTimestamp(timestamp);
    canPacket.setInterfaceId(interfaceId);
    canPacket.setCommonFlags(commonFlags);
    canPacket.setSegmentType(segmentType);

    MessageHeader header;
    canPacket.getRawMessageHeader(&header);
    ASSERT_EQ(header.getTimestamp(), timestamp);
    ASSERT_EQ(header.getInterfaceId(), interfaceId);
    ASSERT_EQ(header.getCommonFlags(), commonFlags);
    ASSERT_EQ(header.getPayloadType(), payloadType.getRawPayloadType());
    ASSERT_EQ(header.getPayloadLength(), canPayloadMsg.size());
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
    const auto& payload = canPacket.getPayload();
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

TEST_F(PacketFixture, IsHeaderCorrect)
{
    //Data copied from wireshark
    std::vector<uint8_t> sampleData = {
        0x17, 0xe0, 0x3e, 0x88, 0x6b, 0x86, 0x63, 0xcd,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x82,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x05, 0x00, 0x9f, 0xc6, 0x00, 0x1e
    };

    Packet packet(ASAM::CMP::CmpHeader::MessageType::data, sampleData.data(), sampleData.size());

    uint64_t expectedTimestamp = swapEndian(*reinterpret_cast<uint64_t*>(sampleData.data()));
    uint32_t expectedInterfaceId = swapEndian(*reinterpret_cast<uint32_t*>(sampleData.data() + 8));
    uint8_t expectedFlags = swapEndian(*(sampleData.data() + 12));
    uint8_t expectedPayloadType = swapEndian(*(sampleData.data() + 13));
    uint16_t expectedPayloadLength = swapEndian(*reinterpret_cast<uint16_t*>(sampleData.data() + 14));

    ASSERT_EQ(packet.getTimestamp(), expectedTimestamp);
    ASSERT_EQ(packet.getInterfaceId(), expectedInterfaceId);
    ASSERT_EQ(packet.getCommonFlags(), expectedFlags);
    ASSERT_EQ(packet.getPayloadType(), expectedPayloadType);
    ASSERT_EQ(packet.getPayloadLength(), expectedPayloadLength);
}
