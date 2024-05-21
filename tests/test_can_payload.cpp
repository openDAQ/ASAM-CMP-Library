#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/can_payload.h>

#include "create_message.h"

using ASAM::CMP::CanPayload;
using ASAM::CMP::Payload;
using CanPayloadTest = testing::Test;

TEST_F(CanPayloadTest, Properties)
{
    constexpr size_t canDataSize = 64;
    constexpr uint32_t arbId = 78;

    std::vector<uint8_t> data(canDataSize);
    std::iota(data.begin(), data.end(), 0);
    auto message = createCanDataMessage(arbId, data);

    CanPayload payload(message.data(), message.size());

    ASSERT_EQ(payload.getType(), Payload::Type::can);
    ASSERT_EQ(payload.getId(), arbId);
    ASSERT_EQ(payload.getDataLength(), canDataSize);
    ASSERT_TRUE(std::equal(data.begin(), data.end(), payload.getData()));
}
