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
    DecoderFixture()
    {
        canData.resize(canDataSize);
        std::iota(canData.begin(), canData.end(), 0);
        auto canMsg = createCanDataMessage(arbId, canData);
        auto dataMsg = createDataMessage(payloadTypeCan, canMsg);
        cmpMsg = createCmpMessage(deviceId, cmpMessageTypeData, streamId, dataMsg);
    }

protected:
    static constexpr size_t canDataSize = 8;
    static constexpr int32_t arbId = 33;
    static constexpr uint8_t payloadTypeCan = 0x01;
    static constexpr uint16_t deviceId = 3;
    static constexpr uint8_t cmpMessageTypeData = 0x01;
    static constexpr uint8_t streamId = 0x01;

protected:
    std::vector<uint8_t> canData;
    std::vector<uint8_t> cmpMsg;
};

TEST_F(DecoderFixture, DataNull)
{
    Decoder decoder;
    auto packets = decoder.decode(nullptr, 0);
    ASSERT_TRUE(packets.empty());
}

TEST_F(DecoderFixture, WrongHeaderSize)
{
    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size() - 1);
    ASSERT_EQ(packets.size(), 0);
}

TEST_F(DecoderFixture, CorruptedDataMessage)
{
    auto dataMessageHeader = reinterpret_cast<DataMessageHeader*>(cmpMsg.data() + sizeof(CmpMessageHeader));
    uint16_t newSize = swapEndian(dataMessageHeader->payloadLength) + 1;
    dataMessageHeader->payloadLength = swapEndian(newSize);

    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
    ASSERT_EQ(packets.size(), 0);
}

TEST_F(DecoderFixture, MessageWithErrorFlag)
{
    auto dataMessageHeader = reinterpret_cast<DataMessageHeader*>(cmpMsg.data() + sizeof(CmpMessageHeader));
    dataMessageHeader->flags |= errorInPayload;

    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
    ASSERT_EQ(packets.size(), 0);
}

TEST_F(DecoderFixture, CanMessage)
{
    Decoder decoder;
    auto packets = decoder.decode(cmpMsg.data(), cmpMsg.size());
    ASSERT_EQ(packets.size(), 1);

    auto packet = packets[0];
    auto& payload = packet->getPayload();
    ASSERT_EQ(static_cast<uint8_t>(payload.getType()), payloadTypeCan);

    auto canPayload = static_cast<const CanPayload&>(payload);
    ASSERT_EQ(canPayload.getId(), arbId);
    ASSERT_EQ(canPayload.getDataLength(), canDataSize);
    ASSERT_TRUE(std::equal(canData.begin(), canData.end(), canPayload.getData()));
}
