#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/status.h>

#include "create_message.h"

using ASAM::CMP::Status;
using ASAM::CMP::DeviceStatus;
using ASAM::CMP::Packet;
using ASAM::CMP::Payload;
using ASAM::CMP::PayloadType;
using MessageType = ASAM::CMP::CmpHeader::MessageType;

class StatusTest : public ::testing::Test
{
public:
    StatusTest()
    {
        std::vector<uint8_t> data(dataSize);
        std::iota(data.begin(), data.end(), uint8_t{0});
        auto payloadMsg = createCaptureModuleDataMessage(deviceDescription, serialNumber, hardwareVersion, softwareVersion, data);
        auto cmDataMsg = createDataMessage(PayloadType::cmStatMsg, payloadMsg);
        cmPacket = Packet{MessageType::status, cmDataMsg.data(), cmDataMsg.size()};
        cmPacket.setDeviceId(deviceId);
    }

protected:
    static constexpr std::string_view deviceDescription = "Device Description";
    static constexpr std::string_view serialNumber = "Serial Number";
    static constexpr std::string_view hardwareVersion = "Hardware Version";
    static constexpr std::string_view softwareVersion = "Software Version";
    static constexpr size_t dataSize = 32;

    static constexpr uint16_t deviceId = 37;

protected:
    Packet cmPacket;
    Status status;
};

TEST_F(StatusTest, Empty)
{
    ASSERT_EQ(status.getDeviceStatusCount(), 0);
}

TEST_F(StatusTest, UpdateClear)
{
    status.update(cmPacket);
    ASSERT_EQ(status.getDeviceStatusCount(), 1);
    status.clear();
    ASSERT_EQ(status.getDeviceStatusCount(), 0);
}

TEST_F(StatusTest, Access)
{
    status.update(cmPacket);
    ASSERT_EQ(status.getDeviceStatusCount(), 1);
    DeviceStatus& deviceStatus = status.getDeviceStatus(0);
    Packet& packet = deviceStatus.getPacket();
    ASSERT_EQ(packet, cmPacket);
}

TEST_F(StatusTest, ConstantAccess)
{
    status.update(cmPacket);
    const Status& constStatus = status;
    ASSERT_EQ(constStatus.getDeviceStatusCount(), 1);
    const DeviceStatus& deviceStatus = constStatus.getDeviceStatus(0);
    const Packet& packet = deviceStatus.getPacket();
    ASSERT_EQ(packet, cmPacket);
}

TEST_F(StatusTest, EmptyInterfaces)
{
    status.update(cmPacket);
    ASSERT_EQ(status.getDeviceStatusCount(), 1);
    ASSERT_EQ(status.getDeviceStatus(0).getInterfaceStatusCount(), 0);
}

TEST_F(StatusTest, SameDeviceId)
{
    constexpr uint8_t newStreamId = 66;

    status.update(cmPacket);
    ASSERT_EQ(status.getDeviceStatusCount(), 1);
    Packet packet = status.getDeviceStatus(0).getPacket();
    ASSERT_EQ(packet, cmPacket);
    packet.setStreamId(newStreamId);
    status.update(packet);
    ASSERT_EQ(status.getDeviceStatusCount(), 1);
}

TEST_F(StatusTest, NewDeviceId)
{
    constexpr uint8_t newStreamId = 66;
    constexpr uint8_t newDeviceId = 77;

    status.update(cmPacket);
    ASSERT_EQ(status.getDeviceStatusCount(), 1);
    Packet packet = status.getDeviceStatus(0).getPacket();
    ASSERT_EQ(packet, cmPacket);
    packet.setStreamId(newStreamId);
    packet.setDeviceId(newDeviceId);
    status.update(packet);
    ASSERT_EQ(status.getDeviceStatusCount(), 2);
}
