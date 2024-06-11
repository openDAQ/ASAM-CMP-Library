#pragma once

#include <asam_cmp/common.h>

BEGIN_NAMESPACE_ASAM_CMP

#pragma pack(push, 1)

// This header represents Data Message header, Status Message header,
// Control Message header and Vendor-defined Message header
class MessageHeader
{
    uint64_t timestamp{0};
    union InterfaceVendorUnion
    {
        uint32_t interfaceId{0};
        struct Vendor
        {
            uint16_t reserved;
            uint16_t vendorId;
        } vendor;
    } interfaceVendorUnion;

    uint8_t commonFlags{0};
    uint8_t payloadType{0};
    uint16_t payloadLength{0};

    static constexpr uint8_t segShift = 2;

public:
    enum class CommonFlags : uint8_t
    {
        recalc = 0x01,
        insync = 0x02,
        seg = 0x0C,
        diOnIf = 0x10,
        overflow = 0x20,
        errorInPayload = 0x40
    };

    enum class SegmentType : uint8_t
    {
        unsegmented = 0,
        firstSegment = 0x04,
        intermediarySegment = 0x08,
        lastSegment = 0x0C
    };

public:
    uint64_t getTimestamp() const;
    void setTimestamp(const uint64_t newTimestamp);
    uint32_t getInterfaceId() const;
    void setInterfaceId(const uint32_t id);
    uint16_t getVendorId() const;
    void setVendorId(const uint16_t id);

    uint8_t getCommonFlags() const;
    void setCommonFlags(const uint8_t newFlags);
    bool getCommonFlag(const CommonFlags mask) const;
    void setCommonFlag(const CommonFlags mask, const bool value);
    SegmentType getSegmentType() const;
    void setSegmentType(const SegmentType type);

    uint8_t getPayloadType() const;
    void setPayloadType(const uint8_t type);
    uint16_t getPayloadLength() const;
    void setPayloadLength(const uint16_t length);
};

#pragma pack(pop)

END_NAMESPACE_ASAM_CMP
