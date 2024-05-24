#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

class EthernetPayload : public Payload
{
public:
#pragma pack(push, 1)
    class Header
    {
        uint16_t flags{0};
        uint16_t reserved{0};
        uint16_t dataLength{0};

    public:
        uint16_t getFlags() const;
        void setFlags(uint16_t newFlags);
        uint16_t getDataLength() const;
        void setDataLength(uint16_t newDataLength);
        void increaseDataLength(uint16_t length);
    };
#pragma pack(pop)

public:
    EthernetPayload(const uint8_t* data, const size_t size);

    bool addSegment(const uint8_t* data, const size_t size) override;

    uint16_t getDataLength() const;
    const uint8_t* getData() const;

    static bool isValidPayload(const uint8_t* data, const size_t size);

private:
    static constexpr uint16_t errorMask = 0x003B;
};

END_NAMESPACE_ASAM_CMP
