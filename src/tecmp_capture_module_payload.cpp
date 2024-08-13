#include <asam_cmp/tecmp_capture_module_payload.h>
#include <sstream>

using ASAM::CMP::swapEndian;

uint8_t TECMP::CaptureModulePayload::getVendorId() const
{
    return getHeader()->getVendorId();
}

void TECMP::CaptureModulePayload::setVendorId(uint8_t newId)
{
    getHeader()->setVendorId(newId);
}

uint8_t TECMP::CaptureModulePayload::getDeviceVersion() const
{
    return getHeader()->getDeviceVersion();
}

void TECMP::CaptureModulePayload::setDeviceVersion(uint8_t newDeviceVersion)
{
    getHeader()->setDeviceVersion(newDeviceVersion);
}

uint8_t TECMP::CaptureModulePayload::getDeviceType() const
{
    return getHeader()->getDeviceType();
}

void TECMP::CaptureModulePayload::setDeviceType(uint8_t newDeviceType)
{
    getHeader()->setDeviceType(newDeviceType);
}

uint16_t TECMP::CaptureModulePayload::getVendorDataLength() const
{
    return getHeader()->getVendorDataLength();
}

void TECMP::CaptureModulePayload::setVendorDataLength(uint16_t newVendorDataLength)
{
    getHeader()->setVendorDataLength(newVendorDataLength);
}

uint16_t TECMP::CaptureModulePayload::getDeviceId() const
{
    return getHeader()->getDeviceId();
}

void TECMP::CaptureModulePayload::setDeviceId(uint16_t newDeviceId)
{
    getHeader()->setDeviceId(newDeviceId);
}

uint32_t TECMP::CaptureModulePayload::getSerialNumber() const
{
    return getHeader()->getSerialNumber();
}

void TECMP::CaptureModulePayload::setSerialNumber(uint32_t newSerialNumber)
{
    getHeader()->setSerialNumber(newSerialNumber);
}

uint8_t TECMP::CaptureModulePayload::getSwVersionMajor() const
{
    return getHeader()->getSwVersionMajor();
}

void TECMP::CaptureModulePayload::setSwVersionMajor(uint8_t newValue)
{
    getHeader()->setSwVersionMajor(newValue);
}

uint8_t TECMP::CaptureModulePayload::getSwVersionMinor() const
{
    return getHeader()->getSwVersionMinor();
}

void TECMP::CaptureModulePayload::setSwVersionMinor(uint8_t newValue)
{
    getHeader()->setSwVersionMinor(newValue);
}

uint8_t TECMP::CaptureModulePayload::getSwVersionPatch() const
{
    return getHeader()->getSwVersionPatch();
}

void TECMP::CaptureModulePayload::setSwVersionPatch(uint8_t newValue)
{
    getHeader()->setSwVersionPatch(newValue);
}

uint8_t TECMP::CaptureModulePayload::getHwVersionMajor() const
{
    return getHeader()->getHwVersionMajor();
}

void TECMP::CaptureModulePayload::setHwVersionMajor(uint8_t newValue)
{
    getHeader()->setHwVersionMajor(newValue);
}

uint8_t TECMP::CaptureModulePayload::getHwVersionMinor() const
{
    return getHeader()->getHwVersionMinor();
}

void TECMP::CaptureModulePayload::setHwVersionMinor(uint8_t newValue)
{
    getHeader()->setHwVersionMinor(newValue);
}

uint8_t TECMP::CaptureModulePayload::getVoltageWhole() const
{
    return getHeader()->getVoltageWhole();
}

void TECMP::CaptureModulePayload::setVoltageWhole(uint8_t newValue)
{
    getHeader()->setVoltageWhole(newValue);
}

uint8_t TECMP::CaptureModulePayload::getVoltageFraction() const
{
    return getHeader()->getVoltageFraction();
}

void TECMP::CaptureModulePayload::setVoltageFraction(uint8_t newValue)
{
    getHeader()->setVoltageFraction(newValue);
}

uint8_t TECMP::CaptureModulePayload::getBufferFill() const
{
    return getHeader()->getBufferFill();
}

void TECMP::CaptureModulePayload::setBufferFill(uint8_t val)
{
    getHeader()->setBufferFill(val);
}

uint8_t TECMP::CaptureModulePayload::getIsBufferOverflow() const
{
    return getHeader()->getIsBufferOverflow();
}

void TECMP::CaptureModulePayload::setIsBufferOverflow(uint8_t val)
{
    getHeader()->setIsBufferOverflow(val);
}

uint32_t TECMP::CaptureModulePayload::getBufferSize() const
{
    return getHeader()->getBufferSize();
}

void TECMP::CaptureModulePayload::setBufferSize(uint32_t val)
{
    getHeader()->setBufferSize(val);
}

uint64_t TECMP::CaptureModulePayload::getLifecycle() const
{
    return getHeader()->getLifecycle();
}

void TECMP::CaptureModulePayload::setLifecycle(uint64_t val)
{
    getHeader()->setLifecycle(val);
}

uint8_t TECMP::CaptureModulePayload::getChassisTemp() const
{
    return getHeader()->getChassisTemp();
}

void TECMP::CaptureModulePayload::setChassisTemp(uint8_t val)
{
    getHeader()->setChassisTemp(val);
}

uint8_t TECMP::CaptureModulePayload::getSilliconTemp() const
{
    return getHeader()->getSilliconTemp();
}

void TECMP::CaptureModulePayload::setSilliconTemp(uint8_t val)
{
    getHeader()->setSilliconTemp(val);
}

std::string TECMP::CaptureModulePayload::getSwVersion() const
{
    std::stringstream ss;
    ss << "v" << std::to_string(getHeader()->getSwVersionMajor()) << "." << std::to_string(getHeader()->getSwVersionMinor()) << "."
       << std::to_string(getHeader()->getSwVersionPatch());
    return ss.str();
}

std::string TECMP::CaptureModulePayload::getHwVersion() const
{
    std::stringstream ss;
    ss << "v" << std::to_string(getHeader()->getHwVersionMajor()) << "." << std::to_string(getHeader()->getHwVersionMinor());
    return ss.str();
}

float TECMP::CaptureModulePayload::getVoltage() const
{
    return static_cast<float>(getHeader()->getVoltageWhole()) + static_cast<float>(getHeader()->getVoltageFraction()) / 100.0f;
}
const TECMP::CaptureModulePayload::Header* TECMP::CaptureModulePayload::getHeader() const
{
    return reinterpret_cast<const Header*>(payloadData.data());
}
TECMP::CaptureModulePayload::Header* TECMP::CaptureModulePayload::getHeader()
{
    return reinterpret_cast<Header*>(payloadData.data());
}
TECMP::CaptureModulePayload::CaptureModulePayload()
    : Payload(TECMP::PayloadType::cmStatMsg, sizeof(Header))
{
}
TECMP::CaptureModulePayload::CaptureModulePayload(const uint8_t* data, const size_t size)
    : Payload(TECMP::PayloadType::cmStatMsg, data, size)
{
}
uint8_t TECMP::CaptureModulePayload::Header::getVendorId() const
{
    return swapEndian(vendorId);
}
void TECMP::CaptureModulePayload::Header::setVendorId(uint8_t newId)
{
    vendorId = swapEndian(newId);
}
uint8_t TECMP::CaptureModulePayload::Header::getDeviceVersion() const
{
    return swapEndian(deviceVersion);
}
void TECMP::CaptureModulePayload::Header::setDeviceVersion(uint8_t newDeviceVersion)
{
    deviceVersion = swapEndian(newDeviceVersion);
}
uint8_t TECMP::CaptureModulePayload::Header::getDeviceType() const
{
    return swapEndian(deviceType);
}
void TECMP::CaptureModulePayload::Header::setDeviceType(uint8_t newDeviceType)
{
    deviceType = swapEndian(newDeviceType);
}
uint16_t TECMP::CaptureModulePayload::Header::getVendorDataLength() const
{
    return swapEndian(vendorDataLength);
}
void TECMP::CaptureModulePayload::Header::setVendorDataLength(uint16_t newVendorDataLength)
{
    vendorDataLength = swapEndian(newVendorDataLength);
}
uint16_t TECMP::CaptureModulePayload::Header::getDeviceId() const
{
    return swapEndian(deviceId);
}
void TECMP::CaptureModulePayload::Header::setDeviceId(uint16_t newDeviceId)
{
    deviceId = swapEndian(newDeviceId);
}
uint32_t TECMP::CaptureModulePayload::Header::getSerialNumber() const
{
    return swapEndian(serialNumber);
}
void TECMP::CaptureModulePayload::Header::setSerialNumber(uint32_t newSerialNumber)
{
    serialNumber = swapEndian(newSerialNumber);
}
uint8_t TECMP::CaptureModulePayload::Header::getSwVersionMajor() const
{
    return swapEndian(vendorData.swVersion.major);
}
void TECMP::CaptureModulePayload::Header::setSwVersionMajor(uint8_t newValue)
{
    vendorData.swVersion.major = swapEndian(newValue);
}
uint8_t TECMP::CaptureModulePayload::Header::getSwVersionMinor() const
{
    return swapEndian(vendorData.swVersion.minor);
}
void TECMP::CaptureModulePayload::Header::setSwVersionMinor(uint8_t newValue)
{
    vendorData.swVersion.minor = swapEndian(newValue);
}
uint8_t TECMP::CaptureModulePayload::Header::getSwVersionPatch() const
{
    return swapEndian(vendorData.swVersion.patch);
}
void TECMP::CaptureModulePayload::Header::setSwVersionPatch(uint8_t newValue)
{
    vendorData.swVersion.patch = swapEndian(newValue);
}
uint8_t TECMP::CaptureModulePayload::Header::getHwVersionMajor() const
{
    return swapEndian(vendorData.hwVersion.major);
}
void TECMP::CaptureModulePayload::Header::setHwVersionMajor(uint8_t newValue)
{
    vendorData.hwVersion.major = swapEndian(newValue);
}
uint8_t TECMP::CaptureModulePayload::Header::getHwVersionMinor() const
{
    return swapEndian(vendorData.hwVersion.minor);
}
void TECMP::CaptureModulePayload::Header::setHwVersionMinor(uint8_t newValue)
{
    vendorData.hwVersion.minor = swapEndian(newValue);
}
uint8_t TECMP::CaptureModulePayload::Header::getVoltageWhole() const
{
    return swapEndian(vendorData.voltage.whole);
}
void TECMP::CaptureModulePayload::Header::setVoltageWhole(uint8_t newValue)
{
    vendorData.voltage.whole = swapEndian(newValue);
}
uint8_t TECMP::CaptureModulePayload::Header::getVoltageFraction() const
{
    return swapEndian(vendorData.voltage.frac);
}
void TECMP::CaptureModulePayload::Header::setVoltageFraction(uint8_t newValue)
{
    vendorData.voltage.frac = swapEndian(newValue);
}
uint8_t TECMP::CaptureModulePayload::Header::getBufferFill() const
{
    return swapEndian(vendorData.bufferFill);
}
void TECMP::CaptureModulePayload::Header::setBufferFill(uint8_t val)
{
    vendorData.bufferFill = swapEndian(val);
}
uint8_t TECMP::CaptureModulePayload::Header::getIsBufferOverflow() const
{
    return swapEndian(vendorData.isBufferOverflow);
}
void TECMP::CaptureModulePayload::Header::setIsBufferOverflow(uint8_t val)
{
    vendorData.isBufferOverflow = swapEndian(val);
}
uint32_t TECMP::CaptureModulePayload::Header::getBufferSize() const
{
    return swapEndian(vendorData.bufferSize);
}
void TECMP::CaptureModulePayload::Header::setBufferSize(uint32_t val)
{
    vendorData.bufferSize = swapEndian(val);
}
uint64_t TECMP::CaptureModulePayload::Header::getLifecycle() const
{
    return swapEndian(vendorData.lifecycle);
}
void TECMP::CaptureModulePayload::Header::setLifecycle(uint64_t val)
{
    vendorData.lifecycle = swapEndian(val);
}
uint8_t TECMP::CaptureModulePayload::Header::getChassisTemp() const
{
    return swapEndian(vendorData.chassisTemp);
}
void TECMP::CaptureModulePayload::Header::setChassisTemp(uint8_t val)
{
    vendorData.chassisTemp = swapEndian(val);
}
uint8_t TECMP::CaptureModulePayload::Header::getSilliconTemp() const
{
    return swapEndian(vendorData.silliconTemp);
}
void TECMP::CaptureModulePayload::Header::setSilliconTemp(uint8_t val)
{
    vendorData.silliconTemp = swapEndian(val);
}
