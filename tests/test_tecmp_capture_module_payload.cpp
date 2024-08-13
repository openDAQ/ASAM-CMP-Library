#include <gtest/gtest.h>

#include <asam_cmp/tecmp_capture_module_payload.h>
#include <memory>

using TECMP::CaptureModulePayload;

class TecmpCaptureModulePayloadTest : public ::testing::Test
{
public:
    TecmpCaptureModulePayloadTest()
    {
        // The actual data provided
        std::vector<uint8_t> rawData = {0x0c, 0x01, 0x04, 0x00, 0x00, 0x18, 0x00, 0x43, 0x01, 0x61, 0x16, 0xe1, 0x00, 0x14, 0x07,
                                        0x0a, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x61, 0x1a, 0x54,
                                        0x71, 0x2d, 0x15, 0x14, 0x21, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

        data = rawData;

        payload = std::make_unique<CaptureModulePayload>(data.data(), data.size());
        // Create the Payload object
        // memcpy(&payload, data.data(), sizeof(CaptureModulePayload));
    }

protected:
    template <typename T>
    using SetterFunc = void (CaptureModulePayload::*)(T);

    template <typename T>
    using GetterFunc = T (CaptureModulePayload::*)() const;

    template <typename T>
    bool TestSetterGetter(SetterFunc<T> setter, GetterFunc<T> getter, const T newValue)
    {
        (payload.get()->*setter)(newValue);
        T value = (payload.get()->*getter)();
        return (value == newValue);
    }

protected:
    std::vector<uint8_t> data;
    std::unique_ptr<CaptureModulePayload> payload;
    CaptureModulePayload dcPayload;
};

TEST_F(TecmpCaptureModulePayloadTest, DefaultConstructor)
{
    ASSERT_TRUE(dcPayload.isValid());
}

TEST_F(TecmpCaptureModulePayloadTest, Copy)
{
    auto payloadCopy{*payload};
    ASSERT_TRUE(*payload == payloadCopy);
}

TEST_F(TecmpCaptureModulePayloadTest, CopyAssignment)
{
    auto checker{*payload};
    CaptureModulePayload payloadCopy;
    payloadCopy = checker;
    ASSERT_TRUE(checker == payloadCopy);
}

TEST_F(TecmpCaptureModulePayloadTest, Type)
{
    ASSERT_EQ(payload->getType(), TECMP::PayloadType::cmStatMsg);
    ASSERT_EQ(dcPayload.getType(), TECMP::PayloadType::cmStatMsg);
}

TEST_F(TecmpCaptureModulePayloadTest, Fields)
{
    ASSERT_EQ(payload->getVendorId(), 0x0c);
    ASSERT_EQ(payload->getDeviceType(), 0x04);
    ASSERT_EQ(payload->getVendorDataLength(), 24);
    ASSERT_EQ(payload->getDeviceId(), 0x0043);
    ASSERT_EQ(payload->getSerialNumber(), 23140065);
    ASSERT_EQ(payload->getSwVersionMajor(), 20);
    ASSERT_EQ(payload->getSwVersionMinor(), 7);
    ASSERT_EQ(payload->getSwVersionPatch(), 10);
    ASSERT_EQ(payload->getSwVersion(), "v20.7.10");
    ASSERT_EQ(payload->getVoltage(), 21.2f);
    ASSERT_EQ(payload->getHwVersion(), "v3.3");
}

TEST_F(TecmpCaptureModulePayloadTest, VendorId)
{
    TestSetterGetter(&CaptureModulePayload::setVendorId, &CaptureModulePayload::getVendorId, uint8_t{55});
}

TEST_F(TecmpCaptureModulePayloadTest, DeviceVersion)
{
    TestSetterGetter(&CaptureModulePayload::setDeviceVersion, &CaptureModulePayload::getDeviceVersion, uint8_t{10});
}

TEST_F(TecmpCaptureModulePayloadTest, DeviceType)
{
    TestSetterGetter(&CaptureModulePayload::setDeviceType, &CaptureModulePayload::getDeviceType, uint8_t{20});
}

TEST_F(TecmpCaptureModulePayloadTest, VendorDataLength)
{
    TestSetterGetter(&CaptureModulePayload::setVendorDataLength, &CaptureModulePayload::getVendorDataLength, uint16_t{100});
}

TEST_F(TecmpCaptureModulePayloadTest, DeviceId)
{
    TestSetterGetter(&CaptureModulePayload::setDeviceId, &CaptureModulePayload::getDeviceId, uint16_t{200});
}

TEST_F(TecmpCaptureModulePayloadTest, SerialNumber)
{
    TestSetterGetter(&CaptureModulePayload::setSerialNumber, &CaptureModulePayload::getSerialNumber, uint32_t{123456789});
}

TEST_F(TecmpCaptureModulePayloadTest, SwVersionMajor)
{
    TestSetterGetter(&CaptureModulePayload::setSwVersionMajor, &CaptureModulePayload::getSwVersionMajor, uint8_t{1});
}

TEST_F(TecmpCaptureModulePayloadTest, SwVersionMinor)
{
    TestSetterGetter(&CaptureModulePayload::setSwVersionMinor, &CaptureModulePayload::getSwVersionMinor, uint8_t{2});
}

TEST_F(TecmpCaptureModulePayloadTest, SwVersionPatch)
{
    TestSetterGetter(&CaptureModulePayload::setSwVersionPatch, &CaptureModulePayload::getSwVersionPatch, uint8_t{3});
}

TEST_F(TecmpCaptureModulePayloadTest, HwVersionMajor)
{
    TestSetterGetter(&CaptureModulePayload::setHwVersionMajor, &CaptureModulePayload::getHwVersionMajor, uint8_t{1});
}

TEST_F(TecmpCaptureModulePayloadTest, HwVersionMinor)
{
    TestSetterGetter(&CaptureModulePayload::setHwVersionMinor, &CaptureModulePayload::getHwVersionMinor, uint8_t{2});
}

TEST_F(TecmpCaptureModulePayloadTest, VoltageWhole)
{
    TestSetterGetter(&CaptureModulePayload::setVoltageWhole, &CaptureModulePayload::getVoltageWhole, uint8_t{12});
}

TEST_F(TecmpCaptureModulePayloadTest, VoltageFraction)
{
    TestSetterGetter(&CaptureModulePayload::setVoltageFraction, &CaptureModulePayload::getVoltageFraction, uint8_t{34});
}

TEST_F(TecmpCaptureModulePayloadTest, BufferFill)
{
    TestSetterGetter(&CaptureModulePayload::setBufferFill, &CaptureModulePayload::getBufferFill, uint8_t{50});
}

TEST_F(TecmpCaptureModulePayloadTest, IsBufferOverflow)
{
    TestSetterGetter(&CaptureModulePayload::setIsBufferOverflow, &CaptureModulePayload::getIsBufferOverflow, uint8_t{1});
}

TEST_F(TecmpCaptureModulePayloadTest, BufferSize)
{
    TestSetterGetter(&CaptureModulePayload::setBufferSize, &CaptureModulePayload::getBufferSize, uint32_t{1024});
}

TEST_F(TecmpCaptureModulePayloadTest, Lifecycle)
{
    TestSetterGetter(&CaptureModulePayload::setLifecycle, &CaptureModulePayload::getLifecycle, uint64_t{9876543210});
}

TEST_F(TecmpCaptureModulePayloadTest, ChassisTemp)
{
    TestSetterGetter(&CaptureModulePayload::setChassisTemp, &CaptureModulePayload::getChassisTemp, uint8_t{45});
}

TEST_F(TecmpCaptureModulePayloadTest, SilliconTemp)
{
    TestSetterGetter(&CaptureModulePayload::setSilliconTemp, &CaptureModulePayload::getSilliconTemp, uint8_t{50});
}
