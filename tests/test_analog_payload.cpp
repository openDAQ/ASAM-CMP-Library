#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/analog_payload.h>

#include "create_message.h"

using ASAM::CMP::AnalogPayload;
using ASAM::CMP::Payload;
using PayloadType = ASAM::CMP::MessageHeader::PayloadType;

class AnalogPayloadTest : public ::testing::Test
{
public:
    AnalogPayloadTest()
    {
        data.resize(dataSize);
        std::iota(data.begin(), data.end(), uint8_t{});
        message = createAnalogDataMessage(data);
        payload = std::make_unique<AnalogPayload>(message.data(), message.size());
    }

protected:
    template <typename T>
    using SetterFunc = void (AnalogPayload::*)(T);

    template <typename T>
    using GetterFunc = T (AnalogPayload::*)() const;

    template <typename T>
    bool TestSetterGetter(SetterFunc<T> setter, GetterFunc<T> getter, const T newValue)
    {
        (payload.get()->*setter)(newValue);
        T value = (payload.get() ->*getter)();
        return (value == newValue);
    }

protected:
    static constexpr size_t dataSize = 40;

protected:
    std::vector<uint8_t> data;
    std::vector<uint8_t> message;
    std::unique_ptr<AnalogPayload> payload;
};

TEST_F(AnalogPayloadTest, PayloadType)
{
    ASSERT_EQ(payload->getType(), PayloadType::analog);
}

TEST_F(AnalogPayloadTest, Flags)
{
    ASSERT_TRUE(TestSetterGetter(&AnalogPayload::setFlags, &AnalogPayload::getFlags, uint16_t{55}));
 }

TEST_F(AnalogPayloadTest, SampleDt)
{
    ASSERT_TRUE(TestSetterGetter(&AnalogPayload::setSampleDt, &AnalogPayload::getSampleDt, AnalogPayload::SampleDt::aInt32));
}

TEST_F(AnalogPayloadTest, Unit)
{
    ASSERT_TRUE(TestSetterGetter(&AnalogPayload::setUnit, &AnalogPayload::getUnit, AnalogPayload::Unit::kelvin));
}

TEST_F(AnalogPayloadTest, SampleInterval)
{
    ASSERT_TRUE(TestSetterGetter(&AnalogPayload::setSampleInterval, &AnalogPayload::getSampleInterval, 1.23f));
}

TEST_F(AnalogPayloadTest, SampleOffset)
{
    ASSERT_TRUE(TestSetterGetter(&AnalogPayload::setSampleOffset, &AnalogPayload::getSampleOffset, 1.23f));
}

TEST_F(AnalogPayloadTest, SampleScalar)
{
    ASSERT_TRUE(TestSetterGetter(&AnalogPayload::setSampleScalar, &AnalogPayload::getSampleScalar, 1.23f));
}

TEST_F(AnalogPayloadTest, SamplesCount)
{
    payload->setSampleDt(AnalogPayload::SampleDt::aInt16);
    ASSERT_EQ(payload->getSamplesCount(), dataSize / sizeof(uint16_t));
    payload->setSampleDt(AnalogPayload::SampleDt::aInt32);
    ASSERT_EQ(payload->getSamplesCount(), dataSize / sizeof(uint32_t));
}

TEST_F(AnalogPayloadTest, Data)
{
    ASSERT_TRUE(std::equal(data.begin(), data.end(), payload->getData()));
}

TEST_F(AnalogPayloadTest, IsValidPayload)
{
    ASSERT_TRUE(AnalogPayload::isValidPayload(message.data(), message.size()));
}
