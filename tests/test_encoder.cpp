#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/can_payload.h>
#include <asam_cmp/encoder.h>
#include <asam_cmp/decoder.h>
#include "create_message.h"
#include "utilities.h"

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
        MessageInit(size_t canDataSize = 8, uint16_t deviceId = 3, uint8_t streamId = 1)
            : canDataSize(canDataSize)
            , deviceId(deviceId)
            , streamId(streamId)
        {
        }

        size_t canDataSize;
        int32_t arbId = 33;
        uint8_t payloadTypeCan = 0x01;
        uint16_t deviceId;
        uint8_t cmpMessageTypeData = 0x01;
        uint8_t streamId;
    };

    std::vector<uint8_t> composeMessage(const MessageInit& init)
    {
        std::vector<uint8_t> canData(init.canDataSize);
        std::iota(canData.begin(), canData.end(), uint8_t{});
        auto canMsg = createCanDataMessage(init.arbId, canData);
        auto dataMsg = createDataMessage(init.payloadTypeCan, canMsg);

        return createCmpMessage(init.deviceId, init.cmpMessageTypeData, init.streamId, dataMsg);
    }

    std::vector<PacketPtr> composePacketsPtrs(const std::vector<MessageInit>& initStructures)
    {
        std::vector<PacketPtr> result;
        result.reserve(initStructures.size());

        Decoder decoder;

        for (size_t i = 0; i < initStructures.size(); ++i)
        {
            auto cmpMsg = composeMessage(initStructures[i]);
            auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
            std::copy(begin(packets), end(packets), std::back_inserter(result));
        }

        EXPECT_EQ(result.size(), initStructures.size());
        return result;
    }

    std::vector<Packet> composePackets(const std::vector<MessageInit>& initStructures)
    {
        std::vector<PacketPtr> resultPtrs;
        resultPtrs.reserve(initStructures.size());

        Decoder decoder;

        for (int i = 0; i < initStructures.size(); ++i)
        {
            auto cmpMsg = composeMessage(initStructures[i]);
            auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
            std::copy(begin(packets), end(packets), std::back_inserter(resultPtrs));
        }

        EXPECT_EQ(resultPtrs.size(), initStructures.size());

        std::vector<Packet> result;
        result.reserve(initStructures.size());
        for (const auto& e : resultPtrs)
        {
            result.push_back(*(e.get()));
        }

        return result;
    }
};

TEST_F(EncoderFixture, CorrectnessRef)
{
    std::vector<MessageInit> initStructures = { MessageInit(8, 3) };
    auto packets = composePacketsPtrs(initStructures);
    Encoder encoder;
    auto encodedData = encoder.encode(begin(packets), end(packets), {64, 1500});

    Decoder decoder;
    auto checker = decoder.decode(encodedData[0].data(), encodedData[0].size());

    ASSERT_EQ(checker.size(), 2);
    ASSERT_TRUE(ASAM::CMP::isSamePacket(*(packets[0].get()), *(checker[0].get())));
}

TEST_F(EncoderFixture, Correctness)
{
    std::vector<MessageInit> initStructures = {MessageInit(8, 3)};
    auto packets = composePackets(initStructures);
    Encoder encoder;
    auto encodedData = encoder.encode(begin(packets), end(packets), {64, 1500});

    Decoder decoder;
    auto checker = decoder.decode(encodedData[0].data(), encodedData[0].size());

    ASSERT_EQ(checker.size(), 2u);
    ASSERT_TRUE(ASAM::CMP::isSamePacket(packets[0], *(checker[0].get())));
}

TEST_F(EncoderFixture, Aggregation)
{
    std::vector<MessageInit> initStructures(5, MessageInit(8, 3));
    auto packets = composePacketsPtrs(initStructures);
    Encoder encoder;
    auto encodedData = encoder.encode(begin(packets), end(packets), {64, 1500});
    ASSERT_EQ(encodedData.size(), 1u);
}

TEST_F(EncoderFixture, Segmmentation)
{
    std::vector<MessageInit> initStructures = {
        MessageInit(8,  3, 1),
        MessageInit(60, 3, 1),
        MessageInit(8,  3, 1)
    };

    auto packetsPtrs = composePacketsPtrs(initStructures);
    Encoder encoder;
    auto encodedData = encoder.encode(begin(packetsPtrs), end(packetsPtrs), {64, 90});

    ASSERT_EQ(encodedData.size(), 4);
    ASSERT_EQ(encodedData[0].size(), 64);
    ASSERT_EQ(encodedData[1].size(), 90);
    ASSERT_EQ(encodedData[2].size(), 64);
    ASSERT_EQ(encodedData[2].size(), 64);
}

TEST_F(EncoderFixture, EmptyRange)
{
    Encoder encoder;

    auto packetsPtrs = composePacketsPtrs({});
    EXPECT_THROW(encoder.encode(begin(packetsPtrs), end(packetsPtrs), {64, 1500}), std::invalid_argument);

    auto packets = composePackets({});
    EXPECT_THROW(encoder.encode(begin(packets), end(packets), {64, 1500}), std::invalid_argument);
}

TEST_F(EncoderFixture, WrongDeviceId)
{
    Encoder encoder;

    std::vector<MessageInit> initStructures = {
        MessageInit(8, 3), MessageInit(8, 5)
    };
    auto packetsPtrs = composePacketsPtrs(initStructures);
    EXPECT_THROW(encoder.encode(begin(packetsPtrs), end(packetsPtrs), {64, 1500}), std::runtime_error);

}

TEST_F(EncoderFixture, WrongStreamId)
{
    Encoder encoder;

    std::vector<MessageInit> initStructures = {MessageInit(8, 3, 1), MessageInit(8, 3, 2)};
    auto packetsPtrs = composePacketsPtrs(initStructures);
    EXPECT_THROW(encoder.encode(begin(packetsPtrs), end(packetsPtrs), {64, 1500}), std::runtime_error);
}
