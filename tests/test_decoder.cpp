#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/can_payload.h>
#include <asam_cmp/decoder.h>

#include "create_message.h"

using ASAM::CMP::CanPayload;
using ASAM::CMP::Decoder;
using ASAM::CMP::swapEndian;

class DecoderFixture : public ::testing::Test
{
public:
};

TEST_F(DecoderFixture, Construct)
{
    Decoder decoder;
    decoder;
}

TEST_F(DecoderFixture, DataNull)
{
    Decoder decoder;
    auto packets = decoder.decode(nullptr, 0);
    ASSERT_TRUE(packets.empty());
}

TEST_F(DecoderFixture, WrongHeaderSize)
{
    constexpr uint8_t payloadType = 0x01;
    constexpr uint16_t deviceId = 3;
    constexpr uint8_t cmpMessageTypeData = 0x01;
    constexpr uint8_t streamId = 0x01;

    std::vector<uint8_t> payloadMsg(0);
    auto dataMsg = createDataMessage(payloadType, payloadMsg);
    auto cmpMsg = createCmpMessage(deviceId, cmpMessageTypeData, streamId, dataMsg);

    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size() - 1);
    ASSERT_EQ(packets.size(), 1);
    ASSERT_FALSE(packets[0]->isValid());
}

TEST_F(DecoderFixture, CorruptedDataMessage)
{
    constexpr size_t payloadDataSize = 8;
    constexpr uint8_t payloadType = 0x01;
    constexpr uint16_t deviceId = 3;
    constexpr uint8_t cmpMessageTypeData = 0x01;
    constexpr uint8_t streamId = 0x01;

    std::vector<uint8_t> payloadMsg(payloadDataSize);
    auto dataMsg = createDataMessage(payloadType, payloadMsg);
    auto cmpMsg = createCmpMessage(deviceId, cmpMessageTypeData, streamId, dataMsg);

    auto dataMessageHeader = reinterpret_cast<DataMessageHeader*>(cmpMsg.data() + sizeof(CmpMessageHeader));
    uint16_t newSize = swapEndian(dataMessageHeader->payloadLength) + 1;
    dataMessageHeader->payloadLength = swapEndian(newSize);

    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
    ASSERT_EQ(packets.size(), 1);
    ASSERT_FALSE(packets[0]->isValid());
}

TEST_F(DecoderFixture, MessageWithErrorFlag)
{
    constexpr size_t payloadDataSize = 8;
    constexpr uint8_t payloadType = 0x01;
    constexpr uint16_t deviceId = 3;
    constexpr uint8_t cmpMessageTypeData = 0x01;
    constexpr uint8_t streamId = 0x01;

    std::vector<uint8_t> payloadMsg(payloadDataSize);
    auto dataMsg = createDataMessage(payloadType, payloadMsg);
    auto cmpMsg = createCmpMessage(deviceId, cmpMessageTypeData, streamId, dataMsg);

    auto dataMessageHeader = reinterpret_cast<DataMessageHeader*>(cmpMsg.data() + sizeof(CmpMessageHeader));
    dataMessageHeader->flags |= errorInPayload;

    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
    ASSERT_EQ(packets.size(), 1);
    ASSERT_FALSE(packets[0]->isValid());
}

TEST_F(DecoderFixture, CanMessage)
{
    constexpr size_t canDataSize = 8;
    constexpr uint32_t arbId = 33;
    constexpr uint8_t payloadTypeCan = 0x01;
    constexpr uint16_t deviceId = 3;
    constexpr uint8_t cmpMessageTypeData = 0x01;
    constexpr uint8_t streamId = 0x01;

    std::vector<uint8_t> data(canDataSize);
    std::iota(data.begin(), data.end(), 0);
    auto canMsg = createCanDataMessage(arbId, data);
    auto dataMsg = createDataMessage(payloadTypeCan, canMsg);
    auto cmpMsg = createCmpMessage(deviceId, cmpMessageTypeData, streamId, dataMsg);

    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
    ASSERT_EQ(packets.size(), 1);

    auto packet = packets[0];
    auto& payload = packet->getPayload();
    ASSERT_EQ(static_cast<uint8_t>(payload.getType()), payloadTypeCan);

    auto canPayload = static_cast<const CanPayload&>(payload);
    ASSERT_EQ(canPayload.getId(), arbId);
    ASSERT_EQ(canPayload.getDataLength(), canDataSize);
    ASSERT_TRUE(std::equal(data.begin(), data.end(), canPayload.getData()));
}
