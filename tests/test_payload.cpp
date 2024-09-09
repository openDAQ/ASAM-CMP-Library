#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/message_header.h>
#include <asam_cmp/payload.h>

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
    template <typename T>
    using SetterFunc = void (Payload::*)(T);

    template <typename T>
    using GetterFunc = T (Payload::*)() const;

    template <typename T>
    bool TestSetterGetter(SetterFunc<T> setter, GetterFunc<T> getter, const T newValue)
    {
        (payload.get()->*setter)(newValue);
        T value = (payload.get()->*getter)();
        return (value == newValue);
    }

protected:
    static constexpr size_t dataSize = 64;
    static constexpr PayloadType payloadType = PayloadType::flexRay;

protected:
    std::vector<uint8_t> data;
    std::vector<uint8_t> message;
    std::unique_ptr<Payload> payload;
};

TEST_F(PayloadTest, IsValid)
{
    ASSERT_TRUE(payload->isValid());
}

TEST_F(PayloadTest, MessageType)
{
    ASSERT_TRUE(TestSetterGetter(&Payload::setMessageType, &Payload::getMessageType, MessageType::control));
}

TEST_F(PayloadTest, RawPayloadType)
{
    ASSERT_TRUE(TestSetterGetter(&Payload::setRawPayloadType, &Payload::getRawPayloadType, uint8_t{11}));
}

TEST_F(PayloadTest, Type)
{
    ASSERT_TRUE(TestSetterGetter(&Payload::setType, &Payload::getType, PayloadType{PayloadType::dleStatMsg}));
}

TEST_F(PayloadTest, Length)
{
    ASSERT_EQ(payload->getLength(), message.size());
}

TEST_F(PayloadTest, RawPayload)
{
    ASSERT_TRUE(std::equal(message.begin(), message.end(), payload->getRawPayload()));
}
