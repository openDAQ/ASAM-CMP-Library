#include <cassert>

#include <asam_cmp/capture_module_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

uint64_t CaptureModulePayload::Header::getUptime() const
{
    return swapEndian(uptime);
}

void CaptureModulePayload::Header::setUptime(const uint64_t newUptime)
{
    uptime = swapEndian(newUptime);
}

uint64_t CaptureModulePayload::Header::getGmIdentity() const
{
    return swapEndian(gmIdentity);
}

void CaptureModulePayload::Header::setGmIdentity(const uint64_t identity)
{
    gmIdentity = swapEndian(identity);
}

uint32_t CaptureModulePayload::Header::getGmClockQuality() const
{
    return swapEndian(gmClockQuality);
}

void CaptureModulePayload::Header::setGmClockQuality(const uint32_t quality)
{
    gmClockQuality = swapEndian(quality);
}

uint16_t CaptureModulePayload::Header::getCurrentUtcOffset() const
{
    return swapEndian(currentUtcOffset);
}

void CaptureModulePayload::Header::setCurrentUtcOffset(const uint16_t offset)
{
    currentUtcOffset = swapEndian(offset);
}

uint8_t CaptureModulePayload::Header::getTimeSource() const
{
    return timeSource;
}

void CaptureModulePayload::Header::setTimeSource(const uint8_t source)
{
    timeSource = source;
}

uint8_t CaptureModulePayload::Header::getDomainNumber() const
{
    return domainNumber;
}

void CaptureModulePayload::Header::setDomainNumber(const uint8_t number)
{
    domainNumber = number;
}

uint8_t CaptureModulePayload::Header::getGptpFlags() const
{
    return gPtpFlags;
}

void CaptureModulePayload::Header::setGptpFlags(const uint8_t flags)
{
    gPtpFlags = flags;
}

CaptureModulePayload::CaptureModulePayload()
    : Payload(PayloadType::cmStatMsg, minPayloadSize)
{
}

CaptureModulePayload::CaptureModulePayload(const uint8_t* data, const size_t size)
    : Payload(PayloadType::cmStatMsg, data, size)
{
}

uint64_t CaptureModulePayload::getUptime() const
{
    return getHeader()->getUptime();
}

void CaptureModulePayload::setUptime(const uint64_t newUptime)
{
    getHeader()->setUptime(newUptime);
}

uint64_t CaptureModulePayload::getGmIdentity() const
{
    return getHeader()->getGmIdentity();
}

void CaptureModulePayload::setGmIdentity(const uint64_t identity)
{
    getHeader()->setGmIdentity(identity);
}

uint32_t CaptureModulePayload::getGmClockQuality() const
{
    return getHeader()->getGmClockQuality();
}

void CaptureModulePayload::setGmClockQuality(const uint32_t quality)
{
    getHeader()->setGmClockQuality(quality);
}

uint16_t CaptureModulePayload::getCurrentUtcOffset() const
{
    return getHeader()->getCurrentUtcOffset();
}

void CaptureModulePayload::setCurrentUtcOffset(const uint16_t offset)
{
    getHeader()->setCurrentUtcOffset(offset);
}

uint8_t CaptureModulePayload::getTimeSource() const
{
    return getHeader()->getTimeSource();
}

void CaptureModulePayload::setTimeSource(const uint8_t source)
{
    getHeader()->setTimeSource(source);
}

uint8_t CaptureModulePayload::getDomainNumber() const
{
    return getHeader()->getDomainNumber();
}

void CaptureModulePayload::setDomainNumber(const uint8_t number)
{
    getHeader()->setDomainNumber(number);
}

uint8_t CaptureModulePayload::getGptpFlags() const
{
    return getHeader()->getGptpFlags();
}

void CaptureModulePayload::setGptpFlags(const uint8_t flags)
{
    getHeader()->setGptpFlags(flags);
}

std::string_view CaptureModulePayload::getDeviceDescription() const
{
    std::string_view deviceDescription;
    initStringView(payloadData.data() + sizeof(Header), deviceDescription);
    return removeTrailingNulls(deviceDescription);
}

std::string_view CaptureModulePayload::getSerialNumber() const
{
    std::string_view serialNumber;
    const uint8_t* ptr = initStringView(payloadData.data() + sizeof(Header), serialNumber);
    initStringView(ptr, serialNumber);
    return removeTrailingNulls(serialNumber);
}

std::string_view CaptureModulePayload::getHardwareVersion() const
{
    std::string_view hardwareVersion;
    const uint8_t* ptr = initStringView(payloadData.data() + sizeof(Header), hardwareVersion);
    ptr = initStringView(ptr, hardwareVersion);
    initStringView(ptr, hardwareVersion);
    return removeTrailingNulls(hardwareVersion);
}

std::string_view CaptureModulePayload::getSoftwareVersion() const
{
    std::string_view softwareVersion;
    const uint8_t* ptr = initStringView(payloadData.data() + sizeof(Header), softwareVersion);
    ptr = initStringView(ptr, softwareVersion);
    ptr = initStringView(ptr, softwareVersion);
    initStringView(ptr, softwareVersion);
    return removeTrailingNulls(softwareVersion);
}

uint16_t CaptureModulePayload::getVendorDataLength() const
{
    std::string_view vendorData;
    const uint8_t* ptr = initStringView(payloadData.data() + sizeof(Header), vendorData);
    ptr = initStringView(ptr, vendorData);
    ptr = initStringView(ptr, vendorData);
    ptr = initStringView(ptr, vendorData);
    initStringView(ptr, vendorData);

    return static_cast<uint16_t>(vendorData.size());
}

const uint8_t* CaptureModulePayload::getVendorData() const
{
    std::string_view vendorData;
    const uint8_t* ptr = initStringView(payloadData.data() + sizeof(Header), vendorData);
    ptr = initStringView(ptr, vendorData);
    ptr = initStringView(ptr, vendorData);
    ptr = initStringView(ptr, vendorData);
    initStringView(ptr, vendorData);

    return reinterpret_cast<const uint8_t*>(vendorData.data());
}

std::string_view CaptureModulePayload::getVendorDataStringView() const
{
    std::string_view vendorData;
    const uint8_t* ptr = initStringView(payloadData.data() + sizeof(Header), vendorData);
    ptr = initStringView(ptr, vendorData);
    ptr = initStringView(ptr, vendorData);
    ptr = initStringView(ptr, vendorData);
    initStringView(ptr, vendorData);

    return vendorData;
}

void CaptureModulePayload::setData(const std::string_view deviceDescription,
                                   const std::string_view serialNumber,
                                   const std::string_view hardwareVersion,
                                   const std::string_view softwareVersion,
                                   const std::vector<uint8_t>& vendorData)
{
    constexpr size_t maxNullsCount = 8;
    const size_t payloadSize = sizeof(Header) + 5 * sizeof(uint16_t) + deviceDescription.size() + serialNumber.size() +
                               hardwareVersion.size() + softwareVersion.size() + vendorData.size() + maxNullsCount;
    payloadData.resize(payloadSize);
    auto ptr = payloadData.data() + sizeof(Header);

    ptr = fillWithString(ptr, deviceDescription);
    ptr = fillWithString(ptr, serialNumber);
    ptr = fillWithString(ptr, hardwareVersion);
    ptr = fillWithString(ptr, softwareVersion);

    uint16_t length = static_cast<uint16_t>(vendorData.size());
    auto swappedLength = ASAM::CMP::swapEndian(length);
    memcpy(ptr, &swappedLength, sizeof(length));
    ptr += sizeof(length);
    memcpy(ptr, vendorData.data(), vendorData.size());
    ptr += vendorData.size();

    size_t newSize = ptr - payloadData.data();
    payloadData.resize(newSize);
}

bool CaptureModulePayload::isValidPayload([[maybe_unused]] const uint8_t* data, const size_t size)
{
    return (size >= sizeof(Header));
}

const CaptureModulePayload::Header* CaptureModulePayload::getHeader() const
{
    return reinterpret_cast<const Header*>(payloadData.data());
}

CaptureModulePayload::Header* CaptureModulePayload::getHeader()
{
    return reinterpret_cast<Header*>(payloadData.data());
}

uint8_t* CaptureModulePayload::fillWithString(uint8_t* ptr, const std::string_view str)
{
    uint16_t length = static_cast<uint16_t>(str.size()) + 1;
    if (length % 2)
        length += 1;
    auto swappedLength = ASAM::CMP::swapEndian(length);
    memcpy(ptr, &swappedLength, sizeof(length));
    ptr += sizeof(length);
    memcpy(ptr, str.data(), str.size());
    ptr += str.size();
    char nulls[2] = {0, 0};
    memcpy(ptr, nulls, length - str.size());
    ptr += length - str.size();

    return ptr;
}

const uint8_t* CaptureModulePayload::initStringView(const uint8_t* ptr, std::string_view& str)
{
    uint16_t length = swapEndian(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += sizeof(uint16_t);
    str = std::string_view{reinterpret_cast<const char*>(ptr), length};
    ptr += length;

    return ptr;
}

std::string_view CaptureModulePayload::removeTrailingNulls(std::string_view str)
{
    auto trim_pos = str.find('\0');
    if (trim_pos != str.npos)
        str.remove_suffix(str.size() - trim_pos);

    return str;
}

END_NAMESPACE_ASAM_CMP
