#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

class CanPayload : public Payload
{
public:
#pragma pack(push, 1)
    class Header
    {
        uint16_t flags{0};
        uint16_t reserved{0};
        uint32_t id{0};
        uint32_t crc{0};
        uint16_t errorPosition{0};
        uint8_t dlc{0};
        uint8_t dataLength{0};

    private:
        static constexpr uint16_t errorMask = 0xFF03;

    public:
        uint32_t getId() const;
        void setId(const uint32_t newId);
        bool hasError() const;
        uint8_t getDataLength() const;
        void setDataLength(const uint8_t length);
    };
#pragma pack(pop)

public:
    CanPayload(const uint8_t* data, const size_t size);

    uint32_t getId() const;
    uint8_t getDataLength() const;
    const uint8_t* getData() const;

    static bool isValidPayload(const uint8_t* data, const size_t size);
};

END_NAMESPACE_ASAM_CMP
