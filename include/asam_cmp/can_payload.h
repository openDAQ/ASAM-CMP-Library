#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

class CanPayload : public Payload
{
public:
#pragma pack(push, 1)
    struct Header
    {
        uint16_t flags{0};
        uint16_t reserved{0};
        uint32_t id{0};
        uint32_t crc{0};
        uint16_t errorPosition{0};
        uint8_t dlc{0};
        uint8_t dataLength{0};
    };
#pragma pack(pop)

public:
    CanPayload(const uint8_t* data, const size_t size);

    uint32_t getId() const;
    uint8_t getDataLength() const;
    const uint8_t* getData() const;

private:
    static constexpr uint16_t errorMask = 0x03FF;
};

END_NAMESPACE_ASAM_CMP
