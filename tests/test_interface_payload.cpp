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

    InterfacePayload dcPayload;
};

TEST_F(InterfacePayloadTest, HeaderSize)
{
    static_assert(sizeof(InterfacePayload::Header) == 36, "Size of the header according to the standard");
}

TEST_F(InterfacePayloadTest, DefaultConstructor)
{
    ASSERT_TRUE(dcPayload.isValid());
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
    ASSERT_EQ(dcPayload.getType(), PayloadType::ifStatMsg);
}

TEST_F(InterfacePayloadTest, InterfaceId)
{
    ASSERT_EQ(payload->getInterfaceId(), interfaceId);
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setInterfaceId, &InterfacePayload::getInterfaceId, uint32_t{55}));
    ASSERT_EQ(dcPayload.getInterfaceId(), 0u);
}

TEST_F(InterfacePayloadTest, MsgTotalRx)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setMsgTotalRx, &InterfacePayload::getMsgTotalRx, uint32_t{55}));
    ASSERT_EQ(dcPayload.getMsgTotalRx(), 0u);
}

TEST_F(InterfacePayloadTest, MsgTotalTx)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setMsgTotalTx, &InterfacePayload::getMsgTotalTx, uint32_t{55}));
    ASSERT_EQ(dcPayload.getMsgTotalTx(), 0u);
}

TEST_F(InterfacePayloadTest, MsgDroppedRx)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setMsgDroppedRx, &InterfacePayload::getMsgDroppedRx, uint32_t{55}));
    ASSERT_EQ(dcPayload.getMsgDroppedRx(), 0u);
}

TEST_F(InterfacePayloadTest, MsgDroppedTx)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setMsgDroppedTx, &InterfacePayload::getMsgDroppedTx, uint32_t{55}));
    ASSERT_EQ(dcPayload.getMsgDroppedTx(), 0u);
}

TEST_F(InterfacePayloadTest, ErrorsTotalRx)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setErrorsTotalRx, &InterfacePayload::getErrorsTotalRx, uint32_t{55}));
    ASSERT_EQ(dcPayload.getErrorsTotalRx(), 0u);
}

TEST_F(InterfacePayloadTest, ErrorsTotalTx)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setErrorsTotalTx, &InterfacePayload::getErrorsTotalTx, uint32_t{55}));
    ASSERT_EQ(dcPayload.getErrorsTotalTx(), 0u);
}

TEST_F(InterfacePayloadTest, InterfaceType)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setInterfaceType, &InterfacePayload::getInterfaceType, uint8_t{55}));
    ASSERT_EQ(dcPayload.getInterfaceType(), 0u);
}

TEST_F(InterfacePayloadTest, InterfaceStatus)
{
    ASSERT_TRUE(TestSetterGetter(
        &InterfacePayload::setInterfaceStatus, &InterfacePayload::getInterfaceStatus, InterfacePayload::InterfaceStatus::linkStatusUp));
    ASSERT_EQ(dcPayload.getInterfaceStatus(), InterfacePayload::InterfaceStatus::linkStatusDown);
}

TEST_F(InterfacePayloadTest, FeatureSupportBitmask)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setFeatureSupportBitmask, &InterfacePayload::getFeatureSupportBitmask, uint32_t{55}));
    ASSERT_EQ(dcPayload.getFeatureSupportBitmask(), 0u);
}

TEST_F(InterfacePayloadTest, StreamIds)
{
    ASSERT_EQ(payload->getStreamIdsCount(), streamsIds.size());
    ASSERT_TRUE(std::equal(streamsIds.begin(), streamsIds.end(), payload->getStreamIds()));
    ASSERT_EQ(dcPayload.getStreamIdsCount(), 0u);
    ASSERT_EQ(dcPayload.getStreamIds(), nullptr);
}

TEST_F(InterfacePayloadTest, VendorDataOdd)
{
    ASSERT_EQ(payload->getVendorDataLength(), vendorData.size());
    ASSERT_TRUE(std::equal(vendorData.begin(), vendorData.end(), payload->getVendorData()));
    ASSERT_EQ(dcPayload.getVendorDataLength(), 0u);
    ASSERT_EQ(dcPayload.getVendorData(), nullptr);
}

TEST_F(InterfacePayloadTest, VendorDataEven)
{
    streamsIds.push_back(77);
    message = createInterfaceDataMessage(interfaceId, streamsIds, vendorData);
    payload = std::make_unique<InterfacePayload>(message.data(), message.size());

    ASSERT_EQ(payload->getVendorDataLength(), vendorData.size());
    ASSERT_TRUE(std::equal(vendorData.begin(), vendorData.end(), payload->getVendorData()));
}

TEST_F(InterfacePayloadTest, SetData)
{
    const std::vector<uint8_t> newsStreamsIds = {3, 4, 5, 6, 7, 8, 9};
    static constexpr uint16_t newVendorDataSize = vendorDataSize * 4;
    std::vector<uint8_t> newVendorData(newVendorDataSize);
    std::iota(newVendorData.begin(), newVendorData.end(), 0);

    payload->setData(newsStreamsIds.data(), static_cast<uint16_t>(newsStreamsIds.size()), newVendorData.data(), newVendorDataSize);
    ASSERT_EQ(payload->getStreamIdsCount(), newsStreamsIds.size());
    ASSERT_TRUE(std::equal(newsStreamsIds.begin(), newsStreamsIds.end(), payload->getStreamIds()));
    ASSERT_EQ(payload->getVendorDataLength(), newVendorData.size());
    ASSERT_TRUE(std::equal(newVendorData.begin(), newVendorData.end(), payload->getVendorData()));
}

TEST_F(InterfacePayloadTest, SetZeroData)
{
    payload->setData(nullptr, 0, nullptr, 0);
    ASSERT_EQ(payload->getStreamIdsCount(), 0);
    ASSERT_EQ(payload->getStreamIds(), nullptr);
    ASSERT_EQ(payload->getVendorDataLength(), 0);
    ASSERT_EQ(payload->getVendorData(), nullptr);
}

TEST_F(InterfacePayloadTest, IsValidPayload)
{
    ASSERT_TRUE(InterfacePayload::isValidPayload(message.data(), message.size()));
}
