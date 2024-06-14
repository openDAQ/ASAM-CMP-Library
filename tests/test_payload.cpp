#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/payload.h>
#include <asam_cmp/message_header.h>

#include "create_message.h"

using ASAM::CMP::Payload;
using ASAM::CMP::PayloadType;
using MessageType = ASAM::CMP::CmpHeader::MessageType;

class PayloadTest : public ::testing::Test
{
public:
    PayloadTest()
    {
        data.resize(dataSize);
        std::iota(data.begin(), data.end(), uint8_t{});
        message = createDataMessage(payloadType, data);

        payload = std::make_unique<Payload>(payloadType, message.data(), message.size());
    }

protected:
    static constexpr size_t dataSize = 64;
    static constexpr uint32_t arbId = 78;
    static constexpr PayloadType payloadType = PayloadType::flexRay;

protected:
    std::vector<uint8_t> data;
    std::vector<uint8_t> message;
    std::unique_ptr<Payload> payload;
};

TEST_F(PayloadTest, DefaultConstructor)
{
    Payload pay;
    ASSERT_FALSE(pay.isValid());
}

TEST_F(PayloadTest, Copy)
{
    auto payloadCopy(*payload);
    ASSERT_TRUE(*payload == payloadCopy);
}

TEST_F(PayloadTest, CopyAssignment)
{
    Payload payloadCopy;

    payloadCopy = *payload;
    ASSERT_TRUE(*payload == payloadCopy);
}

TEST_F(PayloadTest, Move)
{
    Payload payloadChecker(*payload);

    Payload payloadCopy(std::move(*payload));

    ASSERT_TRUE(payloadCopy == payloadChecker);
    ASSERT_FALSE(payloadCopy == *payload);
}

TEST_F(PayloadTest, MoveAssignment)
{
    Payload payloadChecker(*payload);

    Payload payloadCopy;
    payloadCopy = std::move(*payload);

    ASSERT_TRUE(payloadCopy == payloadChecker);
    ASSERT_FALSE(payloadCopy == *payload);
}

TEST_F(PayloadTest, IsValid)
{
    ASSERT_TRUE(payload->isValid());
}

TEST_F(PayloadTest, Type)
{
    ASSERT_EQ(payload->getType(), payloadType);
}

TEST_F(PayloadTest, Size)
{
    ASSERT_EQ(payload->getSize(), message.size());
}

TEST_F(PayloadTest, RawPayload)
{
    ASSERT_TRUE(std::equal(message.begin(), message.end(), payload->getRawPayload()));
}
