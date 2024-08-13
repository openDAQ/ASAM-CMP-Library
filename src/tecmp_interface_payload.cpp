#include <asam_cmp/tecmp_interface_payload.h>

using ASAM::CMP::swapEndian;

const uint8_t GENERIC_DATA_SIZE = 12;

uint8_t TECMP::InterfacePayload::Header::getVendorId() const
{
    return swapEndian(vendorId);
}

void TECMP::InterfacePayload::Header::setVendorId(uint8_t value)
{
    vendorId = swapEndian(value);
}

uint8_t TECMP::InterfacePayload::Header::getCmVersion() const
{
    return swapEndian(cmVersion);
}

void TECMP::InterfacePayload::Header::setCmVersion(uint8_t value)
{
    cmVersion = swapEndian(value);
}

uint8_t TECMP::InterfacePayload::Header::getCmType() const
{
    return swapEndian(cmType);
}

void TECMP::InterfacePayload::Header::setCmType(uint8_t value)
{
    cmType = swapEndian(value);
}

uint16_t TECMP::InterfacePayload::Header::getVendorDataLength() const
{
    return swapEndian(vendorDataLength);
}

void TECMP::InterfacePayload::Header::setVendorDataLength(uint16_t value)
{
    vendorDataLength = swapEndian(value);
}

uint16_t TECMP::InterfacePayload::Header::getDeviceId() const
{
    return swapEndian(deviceId);
}

void TECMP::InterfacePayload::Header::setDeviceId(uint16_t value)
{
    deviceId = swapEndian(value);
}

uint32_t TECMP::InterfacePayload::Header::getSerialNumber() const
{
    return swapEndian(serialNumber);
}

void TECMP::InterfacePayload::Header::setSerialNumber(uint32_t value)
{
    serialNumber = swapEndian(value);
}

uint32_t TECMP::InterfacePayload::Header::getInterfaceId() const
{
    return swapEndian(busData.interfaceId);
}

void TECMP::InterfacePayload::Header::setInterfaceId(uint32_t value)
{
    busData.interfaceId = swapEndian(value);
}

uint32_t TECMP::InterfacePayload::Header::getMessagesTotal() const
{
    return swapEndian(busData.messagesTotal);
}

void TECMP::InterfacePayload::Header::setMessagesTotal(uint32_t value)
{
    busData.messagesTotal = swapEndian(value);
}

uint32_t TECMP::InterfacePayload::Header::getErrorsTotal() const
{
    return swapEndian(busData.errorsTotal);
}

void TECMP::InterfacePayload::Header::setErrorsTotal(uint32_t value)
{
    busData.errorsTotal = swapEndian(value);
}

uint8_t TECMP::InterfacePayload::Header::getVendorDataLinkStatus() const
{
    return swapEndian(vendorData.linkStatus);
}

void TECMP::InterfacePayload::Header::setVendorDataLinkStatus(uint8_t value)
{
    vendorData.linkStatus = swapEndian(value);
}

uint8_t TECMP::InterfacePayload::Header::getVendorDataLinkQuality() const
{
    return swapEndian(vendorData.linkQuality);
}

void TECMP::InterfacePayload::Header::setVendorDataLinkQuality(uint8_t value)
{
    vendorData.linkQuality = swapEndian(value);
}

uint16_t TECMP::InterfacePayload::Header::getVendorDataLinkupTime() const
{
    return swapEndian(vendorData.linkupTime);
}

void TECMP::InterfacePayload::Header::setVendorDataLinkupTime(uint16_t value)
{
    vendorData.linkupTime = swapEndian(value);
}

TECMP::InterfacePayload::InterfacePayload()
    : Payload(TECMP::PayloadType::ifStatMsg, sizeof(Header))
{
}

TECMP::InterfacePayload::InterfacePayload(const uint8_t* data, const size_t size)
    : Payload(TECMP::PayloadType::ifStatMsg, data, size)
{
}

uint8_t TECMP::InterfacePayload::getVendorId() const
{
    return getHeader()->getVendorId();
}

void TECMP::InterfacePayload::setVendorId(uint8_t value)
{
    getHeader()->setVendorId(value);
}

uint8_t TECMP::InterfacePayload::getCmVersion() const
{
    return getHeader()->getCmVersion();
}

void TECMP::InterfacePayload::setCmVersion(uint8_t value)
{
    getHeader()->setCmVersion(value);
}

uint8_t TECMP::InterfacePayload::getCmType() const
{
    return getHeader()->getCmType();
}

void TECMP::InterfacePayload::setCmType(uint8_t value)
{
    getHeader()->setCmType(value);
}

uint16_t TECMP::InterfacePayload::getVendorDataLength() const
{
    return getHeader()->getVendorDataLength();
}

void TECMP::InterfacePayload::setVendorDataLength(uint16_t value)
{
    getHeader()->setVendorDataLength(value);
}

uint16_t TECMP::InterfacePayload::getDeviceId() const
{
    return getHeader()->getDeviceId();
}

void TECMP::InterfacePayload::setDeviceId(uint16_t value)
{
    getHeader()->setDeviceId(value);
}

uint32_t TECMP::InterfacePayload::getSerialNumber() const
{
    return getHeader()->getSerialNumber();
}

void TECMP::InterfacePayload::setSerialNumber(uint32_t value)
{
    getHeader()->setSerialNumber(value);
}

uint32_t TECMP::InterfacePayload::getInterfaceId() const
{
    return getHeader()->getInterfaceId();
}

void TECMP::InterfacePayload::setInterfaceId(uint32_t value)
{
    getHeader()->setInterfaceId(value);
}

uint32_t TECMP::InterfacePayload::getMessagesTotal() const
{
    return getHeader()->getMessagesTotal();
}

void TECMP::InterfacePayload::setMessagesTotal(uint32_t value)
{
    getHeader()->setMessagesTotal(value);
}

uint32_t TECMP::InterfacePayload::getErrorsTotal() const
{
    return getHeader()->getErrorsTotal();
}

void TECMP::InterfacePayload::setErrorsTotal(uint32_t value)
{
    getHeader()->setErrorsTotal(value);
}

uint8_t TECMP::InterfacePayload::getVendorDataLinkStatus() const
{
    return getHeader()->getVendorDataLinkStatus();
}

void TECMP::InterfacePayload::setVendorDataLinkStatus(uint8_t value)
{
    getHeader()->setVendorDataLinkStatus(value);
}

uint8_t TECMP::InterfacePayload::getVendorDataLinkQuality() const
{
    return getHeader()->getVendorDataLinkQuality();
}

void TECMP::InterfacePayload::setVendorDataLinkQuality(uint8_t value)
{
    getHeader()->setVendorDataLinkQuality(value);
}

uint16_t TECMP::InterfacePayload::getVendorDataLinkupTime() const
{
    return getHeader()->getVendorDataLinkupTime();
}

void TECMP::InterfacePayload::setVendorDataLinkupTime(uint16_t value)
{
    getHeader()->setVendorDataLinkupTime(value);
}

const TECMP::InterfacePayload::Header* TECMP::InterfacePayload::getHeader() const
{
    return reinterpret_cast<const Header*>(payloadData.data());
}

TECMP::InterfacePayload::Header* TECMP::InterfacePayload::getHeader()
{
    return reinterpret_cast<Header*>(payloadData.data());
}
void TECMP::InterfacePayload::setGenericData(const uint8_t* data)
{
    memcpy(payloadData.data(), data, GENERIC_DATA_SIZE);
}
void TECMP::InterfacePayload::setBusData(const uint8_t* data, const uint8_t dataLenght)
{
    memcpy(payloadData.data() + GENERIC_DATA_SIZE, data, dataLenght);
}
