#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/analog_payload.h>
#include <asam_cmp/payload_type.h>

#include "create_message.h"

using ASAM::CMP::AnalogPayload;
using ASAM::CMP::Payload;
using ASAM::CMP::PayloadType;

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
        T value = (payload.get()->*getter)();
        return (value == newValue);
    }

protected:
    static constexpr size_t dataSize = 40;

protected:
    std::vector<uint8_t> data;
    std::vector<uint8_t> message;
    std::unique_ptr<AnalogPayload> payload;
    AnalogPayload dcPayload;
};

TEST_F(AnalogPayloadTest, HeaderSize)
{
    static_assert(sizeof(AnalogPayload::Header) == 16, "Size of the header according to the standard");
}

TEST_F(AnalogPayloadTest, DefaultConstructor)
{
    ASSERT_TRUE(dcPayload.isValid());
}

TEST_F(AnalogPayloadTest, Type)
{
    ASSERT_EQ(payload->getType(), PayloadType::analog);
    ASSERT_EQ(dcPayload.getType(), PayloadType::analog);
}

TEST_F(AnalogPayloadTest, Flags)
{
    ASSERT_TRUE(TestSetterGetter(&AnalogPayload::setFlags, &AnalogPayload::getFlags, uint16_t{55}));
    ASSERT_EQ(dcPayload.getFlags(), 0u);
}

TEST_F(AnalogPayloadTest, SampleDt)
{
    ASSERT_TRUE(TestSetterGetter(&AnalogPayload::setSampleDt, &AnalogPayload::getSampleDt, AnalogPayload::SampleDt::aInt32));
    ASSERT_EQ(dcPayload.getSampleDt(), AnalogPayload::SampleDt::aInt16);
}

TEST_F(AnalogPayloadTest, Unit)
{
    ASSERT_TRUE(TestSetterGetter(&AnalogPayload::setUnit, &AnalogPayload::getUnit, AnalogPayload::Unit::kelvin));
    ASSERT_EQ(dcPayload.getUnit(), AnalogPayload::Unit::undefined);
}

TEST_F(AnalogPayloadTest, SampleInterval)
{
    ASSERT_TRUE(TestSetterGetter(&AnalogPayload::setSampleInterval, &AnalogPayload::getSampleInterval, 1.23f));
    ASSERT_EQ(dcPayload.getSampleInterval(), 0.f);
}

TEST_F(AnalogPayloadTest, SampleOffset)
{
    ASSERT_TRUE(TestSetterGetter(&AnalogPayload::setSampleOffset, &AnalogPayload::getSampleOffset, 1.23f));
    ASSERT_EQ(dcPayload.getSampleOffset(), 0.f);
}

TEST_F(AnalogPayloadTest, SampleScalar)
{
    ASSERT_TRUE(TestSetterGetter(&AnalogPayload::setSampleScalar, &AnalogPayload::getSampleScalar, 1.23f));
    ASSERT_EQ(dcPayload.getSampleScalar(), 0.f);
}

TEST_F(AnalogPayloadTest, SamplesCount)
{
    payload->setSampleDt(AnalogPayload::SampleDt::aInt16);
    ASSERT_EQ(payload->getSamplesCount(), dataSize / sizeof(uint16_t));
    payload->setSampleDt(AnalogPayload::SampleDt::aInt32);
    ASSERT_EQ(payload->getSamplesCount(), dataSize / sizeof(uint32_t));
    ASSERT_EQ(dcPayload.getSamplesCount(), 0u);
}

TEST_F(AnalogPayloadTest, GetData)
{
    ASSERT_TRUE(std::equal(data.begin(), data.end(), payload->getData()));
    ASSERT_EQ(dcPayload.getData(), nullptr);
}

TEST_F(AnalogPayloadTest, SetData)
{
    constexpr size_t newDataSize = dataSize * 2;
    std::vector<uint8_t> newData(newDataSize);
    std::iota(newData.begin(), newData.end(), 0);
    payload->setData(newData.data(), newData.size());
    ASSERT_EQ(payload->getSampleDt(), AnalogPayload::SampleDt::aInt16);
    ASSERT_EQ(payload->getSamplesCount(), newDataSize / sizeof(uint16_t));
    ASSERT_TRUE(std::equal(newData.begin(), newData.end(), payload->getData()));

    dcPayload.setData(newData.data(), newData.size());
    ASSERT_TRUE(std::equal(newData.begin(), newData.end(), dcPayload.getData()));
}

TEST_F(AnalogPayloadTest, SetZeroData)
{
    payload->setData(nullptr, 0);
    ASSERT_EQ(payload->getSamplesCount(), 0);
    ASSERT_EQ(payload->getData(), nullptr);
}

TEST_F(AnalogPayloadTest, IsValidPayload)
{
    ASSERT_TRUE(AnalogPayload::isValidPayload(message.data(), message.size()));
}
