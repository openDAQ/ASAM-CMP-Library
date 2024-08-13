#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/tecmp_header.h>

BEGIN_NAMESPACE_TECMP

class PayloadType
{
public:
    enum
    {
        control = 0x0000,
        cmStatMsg = 0x0100,
        ifStatMsg = 0x0200,
        confStatMsg = 0x0400,

        // Data message (Msg type = 0x03)
        can = 0x0302,
        canFd = 0x0303,
        lin = 0x0304,
        flexRay = 0x0308,
        uartRs232 = 0x0310,
        analog = 0x0320,
        ethernet = 0x0380,

        // Tx Messages (Msg type = 0x0A)
        canTx = 0x0A02,
        canFdTx = 0x0A03,
        linTx = 0x0A04,
        flexRayTx = 0x0A08,

        invalid = 0xFFFF
    };

private:
    using MessageType = TECMP::CmpHeader::MessageType;
    static constexpr uint32_t messageTypeMask = 0x0000FF00;
    static constexpr uint32_t messageTypeShift = 8;
    static constexpr uint32_t rawPayloadTypeMask = 0x000000FF;

public:
    constexpr PayloadType(const MessageType msgType, const uint8_t rawPayloadType)
        : type((ASAM::CMP::to_underlying(msgType) << messageTypeShift) | rawPayloadType)
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
        type |= ASAM::CMP::to_underlying(newType) << messageTypeShift;
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
        return (type != 0xFFFF);
    }

private:
    uint32_t type{0xFFFF};
};

constexpr bool operator==(const PayloadType lhs, const PayloadType rhs) noexcept
{
    return lhs.getType() == rhs.getType();
}

constexpr bool operator!=(const PayloadType lhs, const PayloadType rhs) noexcept
{
    return !operator==(lhs, rhs);
}

END_NAMESPACE_TECMP