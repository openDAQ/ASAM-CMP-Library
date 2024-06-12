#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/data_message_header.h>
#include <asam_cmp/device_status.h>

#include "create_message.h"

using ASAM::CMP::DeviceStatus;
using ASAM::CMP::InterfaceStatus;
using ASAM::CMP::Packet;
using ASAM::CMP::InterfacePayload;
using MessageType = ASAM::CMP::CmpHeader::MessageType;
using PayloadType = ASAM::CMP::DataMessageHeader::PayloadType;

class DeviceStatusTest : public ::testing::Test
{
public:
    DeviceStatusTest()
    {
        std::vector<uint8_t> data(dataSize);
        std::iota(data.begin(), data.end(), uint8_t{0});
        auto payloadMsg = createCaptureModuleDataMessage(deviceDescription, serialNumber, hardwareVersion, softwareVersion, data);
        auto dataMsg = createDataMessage(PayloadType::cmStatMsg, payloadMsg);
        cmPacket = Packet{MessageType::status, dataMsg.data(), dataMsg.size()};
        cmPacket.setDeviceId(deviceId);

        payloadMsg = createInterfaceDataMessage(interfaceId, streamIds, data);
        dataMsg = createDataMessage(PayloadType::ifStatMsg, payloadMsg);
        ifPacket = Packet{MessageType::status, dataMsg.data(), dataMsg.size()};
        ifPacket.setDeviceId(deviceId);
    }

protected:
    static constexpr std::string_view deviceDescription = "Device Description";
    static constexpr std::string_view serialNumber = "Serial Number";
    static constexpr std::string_view hardwareVersion = "Hardware Version";
    static constexpr std::string_view softwareVersion = "Software Version";
    static constexpr size_t dataSize = 32;

    static constexpr uint16_t deviceId = 37;

    static constexpr uint32_t interfaceId = 778899;
    inline static const std::vector<uint8_t> streamIds = {4, 8, 30, 42};

protected:
    Packet cmPacket;
    Packet ifPacket;
    DeviceStatus deviceStatus;
};

TEST_F(DeviceStatusTest, Empty)
{
    ASSERT_EQ(deviceStatus.getInterfaceStatusCount(), 0);
    ASSERT_FALSE(deviceStatus.getPacket().isValid());
}

TEST_F(DeviceStatusTest, UpdateWithCaptureModuleMessage)
{
    deviceStatus.update(cmPacket);
    ASSERT_TRUE(deviceStatus.getPacket().isValid());
    ASSERT_EQ(deviceStatus.getInterfaceStatusCount(), 0);
}

TEST_F(DeviceStatusTest, UpdateWithInterfaceMessage)
{
    deviceStatus.update(ifPacket);
    ASSERT_FALSE(deviceStatus.getPacket().isValid());
    ASSERT_EQ(deviceStatus.getInterfaceStatusCount(), 1);
}

TEST_F(DeviceStatusTest, AccessToPacket)
{
    constexpr uint8_t newStreamId = 99;

    deviceStatus.update(cmPacket);
    Packet& packet = deviceStatus.getPacket();
    ASSERT_EQ(packet, cmPacket);
    packet.setStreamId(newStreamId);
    ASSERT_NE(packet, cmPacket);
}

TEST_F(DeviceStatusTest, AccessToConstantPacket)
{
    deviceStatus.update(cmPacket);
    const DeviceStatus& constDeviceStatus = deviceStatus;
    const Packet& packet = constDeviceStatus.getPacket();
    ASSERT_EQ(packet, cmPacket);
}

TEST_F(DeviceStatusTest, AccessToInterfaces)
{
    deviceStatus.update(ifPacket);
    ASSERT_EQ(deviceStatus.getInterfaceStatusCount(), 1);
    InterfaceStatus& interfaceStatus = deviceStatus.getInterfaceStatus(0);
    auto packet = interfaceStatus.getPacket();
    ASSERT_EQ(packet, ifPacket);
}

TEST_F(DeviceStatusTest, AccessToConstantInterfaces)
{
    deviceStatus.update(ifPacket);
    const DeviceStatus& constDeviceStatus = deviceStatus;
    ASSERT_EQ(constDeviceStatus.getInterfaceStatusCount(), 1);
    const InterfaceStatus& interfaceStatus = constDeviceStatus.getInterfaceStatus(0);
    const Packet& packet = interfaceStatus.getPacket();
    ASSERT_EQ(packet, ifPacket);
}

TEST_F(DeviceStatusTest, SameInterfaceId)
{
    constexpr uint8_t newStreamId = 66;

    deviceStatus.update(ifPacket);
    ASSERT_EQ(deviceStatus.getInterfaceStatusCount(), 1);
    Packet packet = deviceStatus.getInterfaceStatus(0).getPacket();
    ASSERT_EQ(packet, ifPacket);

    packet.setStreamId(newStreamId);
    deviceStatus.update(packet);
    ASSERT_EQ(deviceStatus.getInterfaceStatusCount(), 1);
}

TEST_F(DeviceStatusTest, NewInterfaceId)
{
    const uint32_t newInterfaceId = 9999;

    deviceStatus.update(ifPacket);
    ASSERT_EQ(deviceStatus.getInterfaceStatusCount(), 1);
    Packet packet = deviceStatus.getInterfaceStatus(0).getPacket();
    ASSERT_EQ(packet, ifPacket);

    InterfacePayload& interfacePayload = static_cast<InterfacePayload&>(packet.getPayload());
    interfacePayload.setInterfaceId(newInterfaceId);
    deviceStatus.update(packet);
    ASSERT_EQ(deviceStatus.getInterfaceStatusCount(), 2);
}
