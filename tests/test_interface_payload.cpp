#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/interface_payload.h>

#include "create_message.h"

using ASAM::CMP::InterfacePayload;
using ASAM::CMP::Payload;
using ASAM::CMP::PayloadType;

class InterfacePayloadTest : public ::testing::Test
{
public:
    InterfacePayloadTest()
    {
        vendorData.resize(vendorDataSize);
        std::iota(vendorData.begin(), vendorData.end(), uint8_t{0});
        message = createInterfaceDataMessage(interfaceId, streamsIds, vendorData);
        payload = std::make_unique<InterfacePayload>(message.data(), message.size());
    }

protected:
    template <typename T>
    using SetterFunc = void (InterfacePayload::*)(T);

    template <typename T>
    using GetterFunc = T (InterfacePayload::*)() const;

    template <typename T>
    bool TestSetterGetter(SetterFunc<T> setter, GetterFunc<T> getter, const T newValue)
    {
        (payload.get()->*setter)(newValue);
        T value = (payload.get()->*getter)();
        return (value == newValue);
    }

protected:
    static constexpr size_t vendorDataSize = 40;
    static constexpr size_t interfaceId = 333;

protected:
    std::vector<uint8_t> streamsIds = {3, 4, 5, 6};
    std::vector<uint8_t> vendorData;
    std::vector<uint8_t> message;
    std::unique_ptr<InterfacePayload> payload;
};

TEST_F(InterfacePayloadTest, HeaderSize)
{
    static_assert(sizeof(InterfacePayload::Header) == 36, "Size of the header according to the standard");
}

TEST_F(InterfacePayloadTest, DefaultConstructor)
{
    InterfacePayload pay;
    ASSERT_FALSE(pay.isValid());
}

TEST_F(InterfacePayloadTest, Copy)
{
    auto payloadCopy{*payload};
    ASSERT_TRUE(*payload == payloadCopy);

    std::vector vec(payloadCopy.getStreamIds(), payloadCopy.getStreamIds() + payloadCopy.getStreamIdsCount());
    payload.reset();
    ASSERT_TRUE(std::equal(vec.begin(), vec.end(), payloadCopy.getStreamIds()));
}

TEST_F(InterfacePayloadTest, CopyAssignment)
{
    auto checker{*payload};
    InterfacePayload payloadCopy;
    payloadCopy = checker;
    ASSERT_TRUE(checker == payloadCopy);

    std::vector vec(payloadCopy.getStreamIds(), payloadCopy.getStreamIds() + payloadCopy.getStreamIdsCount());
    payload.reset();
    ASSERT_TRUE(std::equal(vec.begin(), vec.end(), payloadCopy.getStreamIds()));
}

TEST_F(InterfacePayloadTest, Type)
{
    ASSERT_EQ(payload->getType(), PayloadType::ifStatMsg);
}

TEST_F(InterfacePayloadTest, InterfaceId)
{
    ASSERT_EQ(payload->getInterfaceId(), interfaceId);
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setInterfaceId, &InterfacePayload::getInterfaceId, uint32_t{55}));
}

TEST_F(InterfacePayloadTest, MsgTotalRx)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setMsgTotalRx, &InterfacePayload::getMsgTotalRx, uint32_t{55}));
}

TEST_F(InterfacePayloadTest, MsgTotalTx)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setMsgTotalTx, &InterfacePayload::getMsgTotalTx, uint32_t{55}));
}

TEST_F(InterfacePayloadTest, MsgDroppedRx)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setMsgDroppedRx, &InterfacePayload::getMsgDroppedRx, uint32_t{55}));
}

TEST_F(InterfacePayloadTest, MsgDroppedTx)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setMsgDroppedTx, &InterfacePayload::getMsgDroppedTx, uint32_t{55}));
}

TEST_F(InterfacePayloadTest, ErrorsTotalRx)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setErrorsTotalRx, &InterfacePayload::getErrorsTotalRx, uint32_t{55}));
}

TEST_F(InterfacePayloadTest, ErrorsTotalTx)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setErrorsTotalTx, &InterfacePayload::getErrorsTotalTx, uint32_t{55}));
}

TEST_F(InterfacePayloadTest, InterfaceType)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setInterfaceType, &InterfacePayload::getInterfaceType, uint8_t{55}));
}

TEST_F(InterfacePayloadTest, InterfaceStatus)
{
    ASSERT_TRUE(TestSetterGetter(
        &InterfacePayload::setInterfaceStatus, &InterfacePayload::getInterfaceStatus, InterfacePayload::InterfaceStatus::linkStatusUp));
}

TEST_F(InterfacePayloadTest, FeatureSupportBitmask)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setFeatureSupportBitmask, &InterfacePayload::getFeatureSupportBitmask, uint32_t{55}));
}

TEST_F(InterfacePayloadTest, StreamIds)
{
    ASSERT_EQ(payload->getStreamIdsCount(), streamsIds.size());
    ASSERT_TRUE(std::equal(streamsIds.begin(), streamsIds.end(), payload->getStreamIds()));
}

TEST_F(InterfacePayloadTest, VendorDataOdd)
{
    ASSERT_EQ(payload->getVendorDataLength(), vendorData.size());
    ASSERT_TRUE(std::equal(vendorData.begin(), vendorData.end(), payload->getVendorData()));
}

TEST_F(InterfacePayloadTest, VendorDataEven)
{
    streamsIds.push_back(77);
    message = createInterfaceDataMessage(interfaceId, streamsIds, vendorData);
    payload = std::make_unique<InterfacePayload>(message.data(), message.size());

    ASSERT_EQ(payload->getVendorDataLength(), vendorData.size());
    ASSERT_TRUE(std::equal(vendorData.begin(), vendorData.end(), payload->getVendorData()));
}

TEST_F(InterfacePayloadTest, IsValidPayload)
{
    ASSERT_TRUE(InterfacePayload::isValidPayload(message.data(), message.size()));
}
