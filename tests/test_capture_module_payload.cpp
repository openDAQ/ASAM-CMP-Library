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
    CaptureModulePayload dcPayload;
};

TEST_F(CaptureModulePayloadTest, HeaderSize)
{
    static_assert(sizeof(CaptureModulePayload::Header) == 26, "Size of the header according to the standard");
}

TEST_F(CaptureModulePayloadTest, DefaultConstructor)
{
    ASSERT_TRUE(dcPayload.isValid());
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
    ASSERT_EQ(dcPayload.getType(), PayloadType::cmStatMsg);
}

TEST_F(CaptureModulePayloadTest, Uptime)
{
    ASSERT_TRUE(TestSetterGetter(&CaptureModulePayload::setUptime, &CaptureModulePayload::getUptime, uint64_t{55}));
    ASSERT_EQ(dcPayload.getUptime(), 0u);
}

TEST_F(CaptureModulePayloadTest, GmIdentity)
{
    ASSERT_TRUE(TestSetterGetter(&CaptureModulePayload::setGmIdentity, &CaptureModulePayload::getGmIdentity, uint64_t{55}));
    ASSERT_EQ(dcPayload.getGmIdentity(), 0u);
}

TEST_F(CaptureModulePayloadTest, GmClockQuality)
{
    ASSERT_TRUE(TestSetterGetter(&CaptureModulePayload::setGmClockQuality, &CaptureModulePayload::getGmClockQuality, uint32_t{55}));
    ASSERT_EQ(dcPayload.getGmClockQuality(), 0u);
}

TEST_F(CaptureModulePayloadTest, CurrentUtcOffset)
{
    ASSERT_TRUE(TestSetterGetter(&CaptureModulePayload::setCurrentUtcOffset, &CaptureModulePayload::getCurrentUtcOffset, uint16_t{55}));
    ASSERT_EQ(dcPayload.getCurrentUtcOffset(), 0u);
}

TEST_F(CaptureModulePayloadTest, TimeSource)
{
    ASSERT_TRUE(TestSetterGetter(&CaptureModulePayload::setTimeSource, &CaptureModulePayload::getTimeSource, uint8_t{55}));
    ASSERT_EQ(dcPayload.getTimeSource(), 0u);
}

TEST_F(CaptureModulePayloadTest, DomainNumber)
{
    ASSERT_TRUE(TestSetterGetter(&CaptureModulePayload::setDomainNumber, &CaptureModulePayload::getDomainNumber, uint8_t{55}));
    ASSERT_EQ(dcPayload.getDomainNumber(), 0u);
}

TEST_F(CaptureModulePayloadTest, GptpFlags)
{
    ASSERT_TRUE(TestSetterGetter(&CaptureModulePayload::setGptpFlags, &CaptureModulePayload::getGptpFlags, uint8_t{55}));
    ASSERT_EQ(dcPayload.getGptpFlags(), 0u);
}

TEST_F(CaptureModulePayloadTest, DeviceDescription)
{
    ASSERT_EQ(payload->getDeviceDescription(), deviceDescription);
    ASSERT_EQ(dcPayload.getDeviceDescription(), "");
}

TEST_F(CaptureModulePayloadTest, SerialNumber)
{
    ASSERT_EQ(payload->getSerialNumber(), serialNumber);
    ASSERT_EQ(dcPayload.getSerialNumber(), "");
}

TEST_F(CaptureModulePayloadTest, HardwareVersion)
{
    ASSERT_EQ(payload->getHardwareVersion(), hardwareVersion);
    ASSERT_EQ(dcPayload.getHardwareVersion(), "");
}

TEST_F(CaptureModulePayloadTest, SoftwareVersion)
{
    ASSERT_EQ(payload->getSoftwareVersion(), softwareVersion);
    ASSERT_EQ(dcPayload.getSoftwareVersion(), "");
}

TEST_F(CaptureModulePayloadTest, VendorData)
{
    ASSERT_EQ(payload->getVendorDataLength(), vendorData.size());
    ASSERT_TRUE(std::equal(vendorData.begin(), vendorData.end(), payload->getVendorData()));
    ASSERT_EQ(dcPayload.getVendorDataLength(), 0u);
}

TEST_F(CaptureModulePayloadTest, VendorDataStringView)
{
    ASSERT_EQ(payload->getVendorDataStringView().size(), vendorData.size());
    ASSERT_TRUE(std::equal(vendorData.begin(), vendorData.end(), payload->getVendorDataStringView().begin()));
    ASSERT_EQ(dcPayload.getVendorDataStringView(), "");
}

TEST_F(CaptureModulePayloadTest, SetData)
{
    constexpr std::string_view newDeviceDescription = "Device Description2";
    constexpr std::string_view newSerialNumber = "Serial Num";
    constexpr std::string_view newHardwareVersion = "";
    constexpr std::string_view newSoftwareVersion = "Software Ver";
    static constexpr size_t newDataSize = dataSize * 3;
    std::vector<uint8_t> newVendorData(newDataSize);
    std::iota(newVendorData.begin(), newVendorData.end(), uint8_t{});

    payload->setData(newDeviceDescription, newSerialNumber, newHardwareVersion, newSoftwareVersion, newVendorData);
    ASSERT_EQ(payload->getDeviceDescription(), newDeviceDescription);
    ASSERT_EQ(payload->getSerialNumber(), newSerialNumber);
    ASSERT_EQ(payload->getHardwareVersion(), newHardwareVersion);
    ASSERT_EQ(payload->getSoftwareVersion(), newSoftwareVersion);
    ASSERT_TRUE(std::equal(newVendorData.begin(), newVendorData.end(), payload->getVendorDataStringView().begin()));
}

TEST_F(CaptureModulePayloadTest, IsValidPayload)
{
    ASSERT_TRUE(CaptureModulePayload::isValidPayload(message.data(), message.size()));
}
