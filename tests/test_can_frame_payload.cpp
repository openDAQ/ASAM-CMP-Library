#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/can_frame_payload.h>

#include "create_message.h"

using AsamCmp::CanFramePayload;
using AsamCmp::Payload;
using CanFramePayloadTest = testing::Test;

TEST_F(CanFramePayloadTest, EmptyPayload)
{
    CanFramePayload payload(nullptr, 0);
}

TEST_F(CanFramePayloadTest, Properties)
{
    constexpr size_t canDataSize = 64;
    constexpr uint32_t arbId = 78;

    std::vector<uint8_t> data(canDataSize);
    std::iota(data.begin(), data.end(), 0);
    auto message = createCanDataMessage(arbId, data);

    CanFramePayload payload(message.data(), message.size());

    ASSERT_EQ(payload.getType(), Payload::PayloadType::can);
    ASSERT_EQ(payload.getArbId(), arbId);
    ASSERT_EQ(payload.getLength(), canDataSize);
    ASSERT_TRUE(std::equal(data.begin(), data.end(), payload.getData()));
}
