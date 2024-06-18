#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/can_payload.h>
#include <asam_cmp/decoder.h>
#include <asam_cmp/encoder.h>
#include "create_message.h"

using ASAM::CMP::CanPayload;
using ASAM::CMP::CmpHeader;
using ASAM::CMP::DataContext;
using ASAM::CMP::Decoder;
using ASAM::CMP::Encoder;
using ASAM::CMP::Packet;
using ASAM::CMP::Payload;
using ASAM::CMP::PayloadType;
using ASAM::CMP::swapEndian;
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

        size_t canDataSize{0};
        int32_t arbId{33};
        PayloadType payloadTypeCan{PayloadType::can};
        uint16_t deviceId{0};
        CmpHeader::MessageType cmpMessageTypeData{CmpHeader::MessageType::data};
        uint8_t streamId{0};
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

        for (size_t i = 0; i < initStructures.size(); ++i)
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
    std::vector<MessageInit> initStructures = {MessageInit(8, 3)};
    auto packets = composePacketsPtrs(initStructures);

    Encoder encoder;

    encoder.setDeviceId(3);
    encoder.setStreamId(1);
    auto encodedData = encoder.encode(begin(packets), end(packets), {64, 1500});

    Decoder decoder;
    auto checker = decoder.decode(encodedData[0].data(), encodedData[0].size());

    ASSERT_EQ(checker.size(), 1u);
    ASSERT_TRUE(*packets[0] == *checker[0]);
}

TEST_F(EncoderFixture, Correctness)
{
    std::vector<MessageInit> initStructures = {MessageInit(8, 3)};
    auto packets = composePackets(initStructures);

    Encoder encoder;
    encoder.setDeviceId(3);
    encoder.setStreamId(1);
    auto encodedData = encoder.encode(begin(packets), end(packets), {64, 1500});

    Decoder decoder;
    auto checker = decoder.decode(encodedData[0].data(), encodedData[0].size());

    ASSERT_EQ(checker.size(), 1u);
    ASSERT_TRUE(packets[0] == *checker[0]);
}

TEST_F(EncoderFixture, Aggregation)
{
    std::vector<MessageInit> initStructures(5, MessageInit(8, 3));
    auto packetsPtrs = composePacketsPtrs(initStructures);

    Encoder encoder;
    encoder.setDeviceId(3);
    encoder.setStreamId(1);
    auto encodedData = encoder.encode(begin(packetsPtrs), end(packetsPtrs), {64, 1500});
    ASSERT_EQ(encodedData.size(), 1u);

    Decoder decoder;
    auto decodedData = decoder.decode(encodedData[0].data(), encodedData[0].size());

    ASSERT_EQ(packetsPtrs.size(), decodedData.size());
    for (size_t i = 0; i < packetsPtrs.size(); ++i)
    {
        ASSERT_TRUE(*packetsPtrs[i] == *decodedData[i]);
    }
}

TEST_F(EncoderFixture, Segmmentation)
{
    std::vector<MessageInit> initStructures = {
        MessageInit(8,  3, 1),
        MessageInit(8,  3, 1),
        MessageInit(100, 3, 1),
        MessageInit(8,  3, 1),
        MessageInit(8,  3, 1)
    };

    auto packetsPtrs = composePacketsPtrs(initStructures);
    Encoder encoder;
    encoder.setDeviceId(3);
    encoder.setStreamId(1);
    auto encodedData = encoder.encode(begin(packetsPtrs), end(packetsPtrs), {64, 100});

    ASSERT_EQ(encodedData.size(), 4u);
    // Packet had spare space but segmentation should be done from new message
    ASSERT_EQ(encodedData[0].size(), 88u);
    // Full packet length
    ASSERT_EQ(encodedData[1].size(), 100u);
    // There was enough space for one small packet but it filled up to min size with nulls because segmentation should be isolated
    ASSERT_EQ(encodedData[2].size(), 64u);
    // Two small packets
    ASSERT_EQ(encodedData[3].size(), 88u);
}

TEST_F(EncoderFixture, LargePacketInSeparateMessage)
{
    std::vector<MessageInit> initStructures = {
        MessageInit(8, 3, 1), MessageInit(8, 3, 1), MessageInit(60, 3, 1), MessageInit(8, 3, 1), MessageInit(8, 3, 1)};
    auto packetsPtrs = composePacketsPtrs(initStructures);

    Encoder encoder;
    encoder.setDeviceId(3);
    encoder.setStreamId(1);
    auto encodedData = encoder.encode(begin(packetsPtrs), end(packetsPtrs), {64, 100});

    ASSERT_EQ(encodedData.size(), 3u);
    // There was enough space for a piece of new packet, but segmentation should starts from new message
    ASSERT_EQ(encodedData[0].size(), 88u);
    // Big packet fits a single message
    ASSERT_EQ(encodedData[1].size(), 100u);
    ASSERT_EQ(encodedData[2].size(), 88u);
}

TEST_F(EncoderFixture, TestSequenceCounter)
{
    std::vector<MessageInit> initStructures = {
        MessageInit(8, 3, 1), MessageInit(8, 3, 1), MessageInit(100, 3, 1), MessageInit(8, 3, 1), MessageInit(8, 3, 1)};

    auto packetsPtrs = composePacketsPtrs(initStructures);
    Encoder encoder;
    encoder.setDeviceId(3);
    encoder.setStreamId(1);
    auto encodedData = encoder.encode(begin(packetsPtrs), end(packetsPtrs), {64, 100});

    ASSERT_EQ(encoder.getSequenceCounter(), 4u);
    encoder.restart();
    ASSERT_EQ(encoder.getSequenceCounter(), 0u);
}

TEST_F(EncoderFixture, EncodePacket)
{
    constexpr uint32_t arbId = 22;
    constexpr DataContext dataContext = {64, 1500};
    std::vector<uint8_t> canData(8);

    auto canPayloadData = createCanDataMessage(arbId, canData);
    CanPayload payload(canPayloadData.data(), canPayloadData.size());
    Packet packet;
    packet.setPayload(payload);

    Encoder encoder;
    encoder.setDeviceId(3);
    encoder.setStreamId(1);
    auto encodedData = encoder.encode(packet, dataContext);
    ASSERT_EQ(encodedData.size(), 1u);

    Decoder decoder;
    auto decodedPackets = decoder.decode(encodedData[0].data(), encodedData[0].size());

    ASSERT_EQ(decodedPackets.size(), 1u);
    ASSERT_EQ(decodedPackets[0]->getPayload(), payload);
}

// TODO: test if packet has non-data dataType (not implemented yet)
