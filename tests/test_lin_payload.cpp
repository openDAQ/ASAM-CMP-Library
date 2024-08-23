#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/lin_payload.h>

#include "create_message.h"

using ASAM::CMP::LinPayload;
using ASAM::CMP::Payload;
using ASAM::CMP::PayloadType;

class LinPayloadTest : public ::testing::Test
{
public:
    LinPayloadTest()
    {
        data.resize(dataSize);
        std::iota(data.begin(), data.end(), uint8_t{});
        message = createLinDataMessage(linId, data);
        payload = std::make_unique<LinPayload>(message.data(), message.size());
    }

protected:
    template <typename T>
    using SetterFunc = void (LinPayload::*)(T);

    template <typename T>
    using GetterFunc = T (LinPayload::*)() const;

    template <typename T>
    bool TestSetterGetter(SetterFunc<T> setter, GetterFunc<T> getter, const T newValue)
    {
        (payload.get()->*setter)(newValue);
        T value = (payload.get()->*getter)();
        return (value == newValue);
    }

protected:
    static constexpr size_t dataSize = 40;
    static constexpr uint8_t linId = 33;

protected:
    std::vector<uint8_t> data;
    std::vector<uint8_t> message;
    std::unique_ptr<LinPayload> payload;
    LinPayload dcPayload;
};

TEST_F(LinPayloadTest, HeaderSize)
{
    static_assert(sizeof(LinPayload::Header) == 8, "Size of the header according to the standard");
}

TEST_F(LinPayloadTest, DefaultConstructor)
{
    ASSERT_TRUE(dcPayload.isValid());
}

TEST_F(LinPayloadTest, Copy)
{
    auto payloadCopy(*payload);
    ASSERT_TRUE(*payload == payloadCopy);
}

TEST_F(LinPayloadTest, CopyAssignment)
{
    LinPayload payloadCopy;

    payloadCopy = *payload;
    ASSERT_TRUE(*payload == payloadCopy);
}

TEST_F(LinPayloadTest, Move)
{
    LinPayload payloadChecker(*payload);

    LinPayload payloadCopy(std::move(*payload));

    ASSERT_TRUE(payloadCopy == payloadChecker);
    ASSERT_FALSE(payloadCopy == *payload);
}

TEST_F(LinPayloadTest, MoveAssignment)
{
    LinPayload payloadChecker(*payload);

    LinPayload payloadCopy;
    payloadCopy = std::move(*payload);

    ASSERT_TRUE(payloadCopy == payloadChecker);
    ASSERT_FALSE(payloadCopy == *payload);
}

TEST_F(LinPayloadTest, Type)
{
    ASSERT_EQ(payload->getType(), PayloadType::lin);
    ASSERT_EQ(dcPayload.getType(), PayloadType::lin);
}

TEST_F(LinPayloadTest, Flags)
{
    constexpr LinPayload::Flags collisionErr = LinPayload::Flags::collisionErr;
    constexpr LinPayload::Flags syncErr = LinPayload::Flags::syncErr;
    constexpr LinPayload::Flags wup = LinPayload::Flags::wup;
    uint16_t newFlags = to_underlying(collisionErr) | to_underlying(syncErr);

    payload->setFlags(newFlags);

    ASSERT_EQ(payload->getFlags(), newFlags);
    ASSERT_TRUE(payload->getFlag(collisionErr));
    ASSERT_TRUE(payload->getFlag(syncErr));
    ASSERT_FALSE(payload->getFlag(wup));

    payload->setFlag(wup, true);
    payload->setFlag(collisionErr, false);
    newFlags = to_underlying(syncErr) | to_underlying(wup);

    ASSERT_EQ(payload->getFlags(), newFlags);
    ASSERT_FALSE(payload->getFlag(collisionErr));
    ASSERT_TRUE(payload->getFlag(syncErr));
    ASSERT_TRUE(payload->getFlag(wup));

    ASSERT_EQ(dcPayload.getFlags(), 0u);
}

TEST_F(LinPayloadTest, LinId)
{
    ASSERT_TRUE(TestSetterGetter(&LinPayload::setLinId, &LinPayload::getLinId, uint8_t{55}));
    ASSERT_EQ(dcPayload.getLinId(), 0u);
}

TEST_F(LinPayloadTest, ParityBits)
{
    ASSERT_TRUE(TestSetterGetter(&LinPayload::setParityBits, &LinPayload::getParityBits, uint8_t{2}));
    ASSERT_EQ(dcPayload.getParityBits(), 0u);
}

TEST_F(LinPayloadTest, Checksum)
{
    ASSERT_TRUE(TestSetterGetter(&LinPayload::setChecksum, &LinPayload::getChecksum, uint8_t{55}));
    ASSERT_EQ(dcPayload.getChecksum(), 0u);
}

TEST_F(LinPayloadTest, DataLength)
{
    ASSERT_EQ(payload->getDataLength(), dataSize);
    ASSERT_EQ(dcPayload.getDataLength(), 0u);
}

TEST_F(LinPayloadTest, GetData)
{
    ASSERT_TRUE(std::equal(data.begin(), data.end(), payload->getData()));
    ASSERT_EQ(dcPayload.getData(), nullptr);
}

TEST_F(LinPayloadTest, SetData)
{
    constexpr uint16_t newDataSize = dataSize * 2;
    std::vector<uint8_t> newData(newDataSize);
    std::iota(newData.begin(), newData.end(), uint8_t{});
    payload->setData(newData.data(), newDataSize);
    ASSERT_EQ(payload->getDataLength(), newDataSize);
    ASSERT_TRUE(std::equal(newData.begin(), newData.end(), payload->getData()));

    dcPayload.setData(newData.data(), newDataSize);
    ASSERT_TRUE(std::equal(newData.begin(), newData.end(), dcPayload.getData()));
}

TEST_F(LinPayloadTest, IsValidPayload)
{
    ASSERT_TRUE(LinPayload::isValidPayload(message.data(), message.size()));
}
