#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/tecmp_payload.h>

BEGIN_NAMESPACE_TECMP

class CanPayload : public Payload
{
#pragma pack(push, 1)
    class Header
    {
        uint32_t arbId{0};
        uint8_t dlc{0};

    public:
        uint32_t getArbId() const;
        void setArbId(uint32_t newArbId);

        uint8_t getDlc() const;
        void setDlc(uint8_t newDlc);
    };
#pragma pack(pop)
public:
    uint32_t getArbId() const;
    void setArbId(uint32_t newArbId);

    uint8_t getDlc() const;
    void setDlc(uint8_t newDlc);

public:
    CanPayload();
    CanPayload(const uint8_t* data, const size_t size);

    const uint8_t* getData() const;

    uint32_t getCrc() const;

protected:
    const Header* getHeader() const;
    Header* getHeader();
};

END_NAMESPACE_TECMP