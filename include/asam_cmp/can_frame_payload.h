#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

class CanFramePayload : public Payload
{
public:
    struct CanPayloadHeader
    {
        uint16_t flags{0};
        uint16_t reserved{0};
        uint32_t id{0};
        uint32_t crc{0};
        uint16_t error_position{0};
        uint8_t dlc{0};
        uint8_t data_length{0};
    };

public:
    CanFramePayload(const uint8_t* data, const size_t size);

    uint32_t getArbId() const;
    uint8_t getLength() const;
    const uint8_t* getData() const;

};

END_NAMESPACE_ASAM_CMP
