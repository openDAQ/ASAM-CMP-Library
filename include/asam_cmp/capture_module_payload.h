#pragma once

#include <string>

#include <asam_cmp/common.h>
#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

class CaptureModulePayload : public Payload
{
public:
#pragma pack(push, 1)
    class Header
    {
        uint64_t uptime{0};
        uint64_t gmIdentity{0};
        uint32_t gmClockQuality{0};
        uint16_t currentUtcOffset{0};
        uint8_t timeSource{0};
        uint8_t domainNumber{0};
        uint8_t reserved{0};
        uint8_t gPtpFlags{0};

    public:
        uint64_t getUptime() const;
        void setUptime(const uint64_t newUptime);
        uint64_t getGmIdentity() const;
        void setGmIdentity(const uint64_t identity);
        uint32_t getGmClockQuality() const;
        void setGmClockQuality(const uint32_t quality);
        uint16_t getCurrentUtcOffset() const;
        void setCurrentUtcOffset(const uint16_t offset);
        uint8_t getTimeSource() const;
        void setTimeSource(const uint8_t source);
        uint8_t getDomainNumber() const;
        void setDomainNumber(const uint8_t number);
        uint8_t getGptpFlags() const;
        void setGptpFlags(const uint8_t flags);
    };
#pragma pack(pop)

public:
    CaptureModulePayload() = default;
    CaptureModulePayload(const uint8_t* data, const size_t size);

    uint64_t getUptime() const;
    void setUptime(const uint64_t newUptime);
    uint64_t getGmIdentity() const;
    void setGmIdentity(const uint64_t identity);
    uint32_t getGmClockQuality() const;
    void setGmClockQuality(const uint32_t quality);
    uint16_t getCurrentUtcOffset() const;
    void setCurrentUtcOffset(const uint16_t offset);
    uint8_t getTimeSource() const;
    void setTimeSource(const uint8_t source);
    uint8_t getDomainNumber() const;
    void setDomainNumber(const uint8_t number);
    uint8_t getGptpFlags() const;
    void setGptpFlags(const uint8_t flags);

    std::string_view getDeviceDescription() const;
    std::string_view getSerialNumber() const;
    std::string_view getHardwareVersion() const;
    std::string_view getSoftwareVersion() const;
    uint16_t getVendorDataLength() const;
    const uint8_t* getVendorData() const;
    std::string_view getVendorDataStringView() const;

    static bool isValidPayload(const uint8_t* data, const size_t size);

protected:
    const Header* getHeader() const;
    Header* getHeader();

private:
    static const uint8_t* initStringView(const uint8_t* ptr, std::string_view& str);
    static std::string_view removeTrailingNulls(std::string_view str);
};

END_NAMESPACE_ASAM_CMP
