#include <gtest/gtest.h>

#include <asam_cmp/tecmp_can_payload.h>

using TECMP::CanPayload;
using TECMP::Payload;
using TECMP::PayloadType;

class TecmpCanPayloadTest : public ::testing::Test
{
public:
    TecmpCanPayloadTest()
    {
        std::vector<uint8_t> rawData = {0x00, 0x00, 0x00, 0x00, 0x08, 0x7b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

        data = rawData;

        canPayload = std::make_unique<CanPayload>(data.data(), data.size());
    }

protected:
    template <typename T>
    using SetterFunc = void (CanPayload::*)(T);

    template <typename T>
    using GetterFunc = T (CanPayload::*)() const;

    template <typename T>
    bool TestSetterGetter(SetterFunc<T> setter, GetterFunc<T> getter, const T newValue)
    {
        (canPayload.get()->*setter)(newValue);
        T value = (canPayload.get()->*getter)();
        return (value == newValue);
    }

protected:
    std::vector<uint8_t> data;
    std::unique_ptr<CanPayload> canPayload;
    CanPayload dcCanPayload;
};

TEST_F(TecmpCanPayloadTest, DefaultConstructorCan)
{
    ASSERT_TRUE(dcCanPayload.isValid());
}

TEST_F(TecmpCanPayloadTest, TypeCan)
{
    CanPayload* payload = canPayload.get();
    ASSERT_EQ(payload->getType(), PayloadType::can);
    ASSERT_EQ(dcCanPayload.getType(), PayloadType::can);
}

TEST_F(TecmpCanPayloadTest, Fields)
{
    ASSERT_EQ(canPayload->getArbId(), 0u);
    ASSERT_EQ(canPayload->getDlc(), 8u);
    auto dataPtr = reinterpret_cast<const uint16_t*>(canPayload->getData());
    ASSERT_EQ(*dataPtr, 123);
}

TEST_F(TecmpCanPayloadTest, ArbId)
{
    TestSetterGetter(&CanPayload::setArbId, &CanPayload::getArbId, uint32_t{0x12345678});
}

TEST_F(TecmpCanPayloadTest, Dlc)
{
    TestSetterGetter(&CanPayload::setDlc, &CanPayload::getDlc, uint8_t{8});
}
