#pragma once

#include <vector>

#include <asam_cmp/common.h>

BEGIN_NAMESPACE_ASAM_CMP

class Payload
{
public:
    enum class Type : uint8_t
    {
        // Data message payload type
        invalid = 0x00,
        can = 0x01,
        canFd = 0x02,
        lin = 0x03,
        flexRay = 0x04,
        digital = 0x05,
        uartRs232 = 0x06,
        analog = 0x07,
        ethernet = 0x08,
        spi = 0x09,
        i2c = 0x0A,
        gigeVision = 0x0B,
        mipiCsi2dPhy = 0x0C,
        userDefined = 0xFF,

        // Status message payload type
        cmStatMsg = 0x01,
        ifStatMsg = 0x02,
        confStatMsg = 0x03,
        dleStatMsg = 0x04,
        tsleStatMsg = 0x05,
        vendorStatMsg = 0xFF
    };

public:
    Payload(const Type type, const uint8_t* data, const size_t size);
    virtual ~Payload() = default;

    Type getType() const;
    size_t getSize() const;
    const uint8_t* getRawPayload() const;

protected:
    Payload() = default;

protected:
    std::vector<uint8_t> payloadData;
    Type type{Type::invalid};
};

END_NAMESPACE_ASAM_CMP
