#pragma once

#include <cstdint>

#include <asam_cmp/cmp_header.h>

BEGIN_NAMESPACE_ASAM_CMP

class PayloadType
{
public:
    enum
    {
        // Data message payload type
        invalid = 0x0000,
        can = 0x0101,
        canFd = 0x0102,
        lin = 0x0103,
        flexRay = 0x0104,
        digital = 0x0105,
        uartRs232 = 0x0106,
        analog = 0x0107,
        ethernet = 0x0108,
        spi = 0x0109,
        i2c = 0x010A,
        gigeVision = 0x010B,
        mipiCsi2dPhy = 0x010C,
        userDefined = 0x01FF,

        // Status message payload type
        cmStatMsg = 0x0301,
        ifStatMsg = 0x0302,
        confStatMsg = 0x0303,
        dleStatMsg = 0x0304,
        tsleStatMsg = 0x0305,
        vendorStatMsg = 0x03FF
    };

private:
    using MessageType = CmpHeader::MessageType;

    static constexpr uint32_t messageTypeMask = 0x0000FF00;
    static constexpr uint32_t messageTypeShift = 8;
    static constexpr uint32_t rawPayloadTypeMask = 0x000000FF;

public:
    constexpr PayloadType(const MessageType msgType, const uint8_t rawPayloadType)
        : type((to_underlying(msgType) << messageTypeShift) | rawPayloadType)
    {
    }

    constexpr PayloadType(uint32_t payloadType)
        : type(payloadType)
    {
    }

    constexpr uint32_t getType() const
    {
        return type;
    }

    constexpr void setType(const uint32_t newType)
    {
        type = newType;
    }

    constexpr PayloadType::MessageType getMessageType() const
    {
        return static_cast<PayloadType::MessageType>((type & messageTypeMask) >> 8);
    }

    constexpr void setMessageType(const PayloadType::MessageType newType)
    {
        type &= ~messageTypeMask;
        type |= to_underlying(newType) << messageTypeShift;
    }

    constexpr uint8_t getRawPayloadType() const
    {
        return static_cast<uint8_t>(type & rawPayloadTypeMask);
    }

    constexpr void setRawPayloadType(const uint8_t newType)
    {
        type &= ~rawPayloadTypeMask;
        type |= newType;
    }

    constexpr bool isValid() const
    {
        return ((type & 0x00FF) != 0) && ((type & 0xFF00) != 0);
    }

private:
    uint32_t type{0};
};

constexpr bool operator==(const PayloadType lhs, const PayloadType rhs) noexcept
{
    return lhs.getType() == rhs.getType();
}

constexpr bool operator!=(const PayloadType lhs, const PayloadType rhs) noexcept
{
    return !operator==(lhs, rhs);
}

END_NAMESPACE_ASAM_CMP
