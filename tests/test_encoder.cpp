#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/can_payload.h>
#include <asam_cmp/encoder.h>
#include <asam_cmp/decoder.h>
#include "create_message.h"

using ASAM::CMP::CanPayload;
using ASAM::CMP::Decoder;
using ASAM::CMP::Encoder;
using ASAM::CMP::DataContext;
using ASAM::CMP::Payload;
using ASAM::CMP::swapEndian;
using ASAM::CMP::Packet;
using PayloadType = ASAM::CMP::Payload::Type;
using PacketPtr = std::shared_ptr<ASAM::CMP::Packet>;

class EncoderFixture : public ::testing::Test
{
public:
    struct MessageInit
    {
        MessageInit(size_t canDataSize = 8)
            : canDataSize(canDataSize)
        {
        }

        size_t canDataSize;
        const int32_t arbId = 33;
        const PayloadType payloadTypeCan = PayloadType::can;
        const uint16_t deviceId = 3;
        Packet::MessageType cmpMessageTypeData = Packet::MessageType::data;
        const uint8_t streamId = 0x01;
    };

    std::vector<uint8_t> composeMessage(const MessageInit& init)
    {
        std::vector<uint8_t> canData(init.canDataSize);
        std::iota(canData.begin(), canData.end(), uint8_t{});
        auto canMsg = createCanDataMessage(init.arbId, canData);
        auto dataMsg = createDataMessage(init.payloadTypeCan, canMsg);

        return createCmpMessage(init.deviceId, init.cmpMessageTypeData, init.streamId, dataMsg);
    }

    std::vector<PacketPtr> composePackets(size_t cnt, const std::vector<size_t>& packet_size)
    {
        EXPECT_EQ(packet_size.size(), cnt);

        std::vector<PacketPtr> result;
        result.reserve(cnt);

        Decoder decoder;

        for (size_t i = 0; i < cnt; ++i)
        {
            auto cmpMsg = composeMessage(MessageInit(packet_size[i]));
            auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
            std::copy(begin(packets), end(packets), std::back_inserter(result));
        }

        EXPECT_EQ(result.size(), cnt);
        return result;
    }

    bool isSamePayload(const Payload& lhs, const Payload& rhs)
    {
        if (lhs.getType() != rhs.getType())
            return false;

        if (lhs.getSize() != rhs.getSize())
            return false;

        const uint8_t* lhsRaw = lhs.getRawPayload();
        const uint8_t* rhsRaw = rhs.getRawPayload();

        for (size_t i = 0; i < lhs.getSize(); ++i)
            if (lhsRaw[i] != rhsRaw[i])
                return false;

        return true;
    }

    bool isSamePacket(const Packet& lhs, const Packet& rhs)
    {
        if (lhs.getDeviceId() != rhs.getDeviceId())
            return false;

        if (lhs.getMessageType() != rhs.getMessageType())
            return false;

        if (lhs.getPayloadSize() != rhs.getPayloadSize())
            return false;

        if (lhs.getStreamId() != rhs.getStreamId())
            return false;

        if (lhs.getVersion() != rhs.getVersion())
            return false;

        return isSamePayload(lhs.getPayload(), rhs.getPayload());
    }
};

TEST_F(EncoderFixture, Correctness)
{
    auto packets = composePackets(1, {8});
    Encoder encoder;
    auto encodedData = encoder.encode(begin(packets), end(packets), {64, 1500});

    Decoder decoder;
    auto checker = decoder.decode(encodedData[0].data(), encodedData[0].size());

    ASSERT_EQ(checker.size(), 2u);
    ASSERT_TRUE(isSamePacket(*(packets[0].get()), *(checker[0].get())));
}

TEST_F(EncoderFixture, Aggregation)
{
    auto packets = composePackets(5, {8,8,8,8,8});
    Encoder encoder;
    auto encodedData = encoder.encode(begin(packets), end(packets), {64, 1500});
    ASSERT_EQ(encodedData.size(), 1u);
}

TEST_F(EncoderFixture, WrongMessageType)
{

}
