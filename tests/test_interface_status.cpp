#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/message_header.h>
#include <asam_cmp/device_status.h>

#include "create_message.h"

using ASAM::CMP::InterfacePayload;
using ASAM::CMP::InterfaceStatus;
using ASAM::CMP::Packet;
using ASAM::CMP::PayloadType;
using MessageType = ASAM::CMP::CmpHeader::MessageType;

class InterfaceStatusTest : public ::testing::Test
{
public:
    InterfaceStatusTest()
    {
        std::vector<uint8_t> data(dataSize);
        std::iota(data.begin(), data.end(), uint8_t{0});

        auto payloadMsg = createInterfaceDataMessage(interfaceId, streamIds, data);
        auto dataMsg = createDataMessage(PayloadType::ifStatMsg, payloadMsg);
        ifPacket = Packet{MessageType::status, dataMsg.data(), dataMsg.size()};
        ifPacket.setDeviceId(deviceId);
    }

protected:
    static constexpr size_t dataSize = 32;

    static constexpr uint16_t deviceId = 37;

    static constexpr uint32_t interfaceId = 778899;
    inline static const std::vector<uint8_t> streamIds = {4, 8, 30, 42};

protected:
    Packet ifPacket;
    InterfaceStatus interfaceStatus;
};

TEST_F(InterfaceStatusTest, Empty)
{
    ASSERT_FALSE(interfaceStatus.getPacket().isValid());
}

TEST_F(InterfaceStatusTest, Update)
{
    interfaceStatus.update(ifPacket);
    ASSERT_TRUE(interfaceStatus.getPacket().isValid());
}

TEST_F(InterfaceStatusTest, AccessToPacket)
{
    constexpr uint8_t newStreamId = 99;

    interfaceStatus.update(ifPacket);
    Packet& packet = interfaceStatus.getPacket();
    ASSERT_EQ(packet, ifPacket);
    packet.setStreamId(newStreamId);
    ASSERT_NE(packet, ifPacket);
}

TEST_F(InterfaceStatusTest, AccessToConstantPacket)
{
    interfaceStatus.update(ifPacket);
    const InterfaceStatus& constInterfaceStatus = interfaceStatus;
    const Packet& packet = constInterfaceStatus.getPacket();
    ASSERT_EQ(packet, ifPacket);
}

TEST_F(InterfaceStatusTest, GetInterfaceId)
{
    interfaceStatus.update(ifPacket);
    ASSERT_EQ(interfaceStatus.getInterfaceId(), interfaceId);
}
