#include <gtest/gtest.h>

#include <asam_cmp/tecmp_interface_payload.h>

using TECMP::InterfacePayload;
using TECMP::Payload;
using TECMP::PayloadType;

class TecmpInterfacePayloadTest : public ::testing::Test
{
public:
    TecmpInterfacePayloadTest()
    {
        std::vector<uint8_t> rawData = {0x0c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x43, 0x01, 0x61, 0x16, 0xe1, 0x00,
                                        0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0x20, 0x00, 0x00, 0x02, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

        payload = std::make_unique<InterfacePayload>(rawData.data(), rawData.size());
        // vendorData.resize(vendorDataSize);
        // std::iota(vendorData.begin(), vendorData.end(), uint8_t{0});
        // message = createInterfaceDataMessage(interfaceId, streamsIds, vendorData);
        // payload = std::make_unique<InterfacePayload>(message.data(), message.size());
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

TEST_F(TecmpInterfacePayloadTest, DefaultConstructor)
{
    ASSERT_TRUE(dcPayload.isValid());
}

TEST_F(TecmpInterfacePayloadTest, Copy)
{
    auto payloadCopy{*payload};
    ASSERT_TRUE(*payload == payloadCopy);
}

TEST_F(TecmpInterfacePayloadTest, CopyAssignment)
{
    auto checker{*payload};
    InterfacePayload payloadCopy;
    payloadCopy = checker;
    ASSERT_TRUE(checker == payloadCopy);
}

TEST_F(TecmpInterfacePayloadTest, VendorId)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setVendorId, &InterfacePayload::getVendorId, uint8_t{55}));
}

TEST_F(TecmpInterfacePayloadTest, CmVersion)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setCmVersion, &InterfacePayload::getCmVersion, uint8_t{10}));
}

TEST_F(TecmpInterfacePayloadTest, CmType)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setCmType, &InterfacePayload::getCmType, uint8_t{5}));
}

TEST_F(TecmpInterfacePayloadTest, VendorDataLength)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setVendorDataLength, &InterfacePayload::getVendorDataLength, uint16_t{100}));
}

TEST_F(TecmpInterfacePayloadTest, DeviceId)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setDeviceId, &InterfacePayload::getDeviceId, uint16_t{200}));
}

TEST_F(TecmpInterfacePayloadTest, SerialNumber)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setSerialNumber, &InterfacePayload::getSerialNumber, uint32_t{123456}));
}

TEST_F(TecmpInterfacePayloadTest, InterfaceId)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setInterfaceId, &InterfacePayload::getInterfaceId, uint32_t{654321}));
}

TEST_F(TecmpInterfacePayloadTest, MessagesTotal)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setMessagesTotal, &InterfacePayload::getMessagesTotal, uint32_t{1000}));
}

TEST_F(TecmpInterfacePayloadTest, ErrorsTotal)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setErrorsTotal, &InterfacePayload::getErrorsTotal, uint32_t{500}));
}

TEST_F(TecmpInterfacePayloadTest, VendorDataLinkStatus)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setVendorDataLinkStatus, &InterfacePayload::getVendorDataLinkStatus, uint8_t{1}));
}

TEST_F(TecmpInterfacePayloadTest, VendorDataLinkQuality)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setVendorDataLinkQuality, &InterfacePayload::getVendorDataLinkQuality, uint8_t{75}));
}

TEST_F(TecmpInterfacePayloadTest, VendorDataLinkupTime)
{
    ASSERT_TRUE(TestSetterGetter(&InterfacePayload::setVendorDataLinkupTime, &InterfacePayload::getVendorDataLinkupTime, uint16_t{3600}));
}

TEST_F(TecmpInterfacePayloadTest, Fields)
{
    ASSERT_EQ(payload->getVendorId(), 0x0c);
    ASSERT_EQ(payload->getDeviceId(), 0x0043);
    ASSERT_EQ(payload->getInterfaceId(), 0x10u);
}
