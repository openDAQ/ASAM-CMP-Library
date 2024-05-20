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

TEST_F(CanPayloadTest, EmptyPayload)
{
    CanPayload payload(nullptr, 0);
    ASSERT_EQ(payload.getType(), Payload::Type::invalid);
}

TEST_F(CanPayloadTest, WrongHeaderSize)
{
    CanDataMessageHeader header;
    CanPayload payload(reinterpret_cast<uint8_t*>(&header), sizeof(CanDataMessageHeader) - 1);
    ASSERT_EQ(payload.getType(), Payload::Type::invalid);
}

TEST_F(CanPayloadTest, ErrorFlags)
{
    constexpr size_t canDataSize = 64;
    constexpr uint32_t arbId = 78;

    std::vector<uint8_t> data(canDataSize);
    std::iota(data.begin(), data.end(), 0);
    auto message = createCanDataMessage(arbId, data);
    auto header = reinterpret_cast<CanDataMessageHeader*>(message.data());
    header->flags = 1;

    CanPayload payload(message.data(), message.size());

    ASSERT_EQ(payload.getType(), Payload::Type::invalid);
}

TEST_F(CanPayloadTest, ErrorPosition)
{
    constexpr size_t canDataSize = 64;
    constexpr uint32_t arbId = 78;

    std::vector<uint8_t> data(canDataSize);
    std::iota(data.begin(), data.end(), 0);
    auto message = createCanDataMessage(arbId, data);
    auto header = reinterpret_cast<CanDataMessageHeader*>(message.data());
    header->errorPosition = 1;

    CanPayload payload(message.data(), message.size());

    ASSERT_EQ(payload.getType(), Payload::Type::invalid);
}

TEST_F(CanPayloadTest, WrongDataLength)
{
    constexpr size_t canDataSize = 64;
    constexpr uint32_t arbId = 78;

    std::vector<uint8_t> data(canDataSize);
    std::iota(data.begin(), data.end(), 0);
    auto message = createCanDataMessage(arbId, data);
    auto header = reinterpret_cast<CanDataMessageHeader*>(message.data());
    header->dataLength = static_cast<uint8_t>(message.size() + 1);

    CanPayload payload(message.data(), message.size());

    ASSERT_EQ(payload.getType(), Payload::Type::invalid);
}
