#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/capture_module_payload.h>

#include "create_message.h"

using ASAM::CMP::CaptureModulePayload;
using ASAM::CMP::PayloadType;
using ASAM::CMP::Payload;

class CaptureModulePayloadTest : public ::testing::Test
{
public:
    CaptureModulePayloadTest()
    {
        vendorData.resize(dataSize);
        std::iota(vendorData.begin(), vendorData.end(), uint8_t{});
        message = createCaptureModuleDataMessage(deviceDescription, serialNumber, hardwareVersion, softwareVersion, vendorData);
        payload = std::make_unique<CaptureModulePayload>(message.data(), message.size());
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
    static constexpr size_t dataSize = 40;
    static constexpr std::string_view deviceDescription = "Device Description";
    static constexpr std::string_view serialNumber = "Serial Number";
    static constexpr std::string_view hardwareVersion = "Hardware Version";
    static constexpr std::string_view softwareVersion = "Software Version";

protected:
    std::vector<uint8_t> vendorData;
    std::vector<uint8_t> message;
    std::unique_ptr<CaptureModulePayload> payload;
};

TEST_F(CaptureModulePayloadTest, HeaderSize)
{
    static_assert(sizeof(CaptureModulePayload::Header) == 26, "Size of the header according to the standard");
}

TEST_F(CaptureModulePayloadTest, DefaultConstructor)
{
    CaptureModulePayload pay;
    ASSERT_FALSE(pay.isValid());
}

TEST_F(CaptureModulePayloadTest, Copy)
{
    auto payloadCopy{*payload};
    ASSERT_TRUE(*payload == payloadCopy);

    std::string description{payloadCopy.getDeviceDescription()};
    payload.reset();
    ASSERT_EQ(description, payloadCopy.getDeviceDescription());
}

TEST_F(CaptureModulePayloadTest, CopyAssignment)
{
    auto checker{*payload};
    CaptureModulePayload payloadCopy;
    payloadCopy = checker;
    ASSERT_TRUE(checker == payloadCopy);

    std::string version{payloadCopy.getSoftwareVersion()};
    payload.reset();
    ASSERT_EQ(version, payloadCopy.getSoftwareVersion());
}

TEST_F(CaptureModulePayloadTest, Type)
{
    ASSERT_EQ(payload->getType(), PayloadType::cmStatMsg);
}

TEST_F(CaptureModulePayloadTest, Uptime)
{
    ASSERT_TRUE(TestSetterGetter(&CaptureModulePayload::setUptime, &CaptureModulePayload::getUptime, uint64_t{55}));
}

TEST_F(CaptureModulePayloadTest, GmIdentity)
{
    ASSERT_TRUE(TestSetterGetter(&CaptureModulePayload::setGmIdentity, &CaptureModulePayload::getGmIdentity, uint64_t{55}));
}

TEST_F(CaptureModulePayloadTest, GmClockQuality)
{
    ASSERT_TRUE(TestSetterGetter(&CaptureModulePayload::setGmClockQuality, &CaptureModulePayload::getGmClockQuality, uint32_t{55}));
}

TEST_F(CaptureModulePayloadTest, CurrentUtcOffset)
{
    ASSERT_TRUE(TestSetterGetter(&CaptureModulePayload::setCurrentUtcOffset, &CaptureModulePayload::getCurrentUtcOffset, uint16_t{55}));
}

TEST_F(CaptureModulePayloadTest, TimeSource)
{
    ASSERT_TRUE(TestSetterGetter(&CaptureModulePayload::setTimeSource, &CaptureModulePayload::getTimeSource, uint8_t{55}));
}

TEST_F(CaptureModulePayloadTest, DomainNumber)
{
    ASSERT_TRUE(TestSetterGetter(&CaptureModulePayload::setDomainNumber, &CaptureModulePayload::getDomainNumber, uint8_t{55}));
}

TEST_F(CaptureModulePayloadTest, GptpFlags)
{
    ASSERT_TRUE(TestSetterGetter(&CaptureModulePayload::setGptpFlags, &CaptureModulePayload::getGptpFlags, uint8_t{55}));
}

TEST_F(CaptureModulePayloadTest, DeviceDescription)
{
    ASSERT_EQ(payload->getDeviceDescription(), deviceDescription);
}

TEST_F(CaptureModulePayloadTest, SerialNumber)
{
    ASSERT_EQ(payload->getSerialNumber(), serialNumber);
}

TEST_F(CaptureModulePayloadTest, HardwareVersion)
{
    ASSERT_EQ(payload->getHardwareVersion(), hardwareVersion);
}

TEST_F(CaptureModulePayloadTest, SoftwareVersion)
{
    ASSERT_EQ(payload->getSoftwareVersion(), softwareVersion);
}

TEST_F(CaptureModulePayloadTest, VendorData)
{
    ASSERT_EQ(payload->getVendorDataLength(), vendorData.size());
    ASSERT_TRUE(std::equal(vendorData.begin(), vendorData.end(), payload->getVendorData()));
}

TEST_F(CaptureModulePayloadTest, VendorDataStringView)
{
    ASSERT_EQ(payload->getVendorDataStringView().size(), vendorData.size());
    ASSERT_TRUE(std::equal(vendorData.begin(), vendorData.end(), payload->getVendorDataStringView().begin()));
}

TEST_F(CaptureModulePayloadTest, IsValidPayload)
{
    ASSERT_TRUE(CaptureModulePayload::isValidPayload(message.data(), message.size()));
}
