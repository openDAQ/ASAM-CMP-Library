#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

class EthernetPayload : public Payload
{
public:
    enum class Flags : uint16_t
    {
        fcsErr = 0x0001,
        frameShorterThan64b = 0x0002,
        txPortDown = 0x0004,
        collision = 0x0008,
        frameTooLongErr = 0x0010,
        phyErr = 0x0020,
        frameTruncated = 0x0040,
        fcsSupport = 0x0080
    };

#pragma pack(push, 1)
    class Header
    {
        uint16_t flags{0};
        uint16_t reserved{0};
        uint16_t dataLength{0};

    public:
        uint16_t getFlags() const;
        void setFlags(uint16_t newFlags);
        bool getFlag(Flags mask) const;
        void setFlag(Flags mask, bool value);
        uint16_t getDataLength() const;
        void setDataLength(uint16_t newDataLength);
    };
#pragma pack(pop)

public:
    EthernetPayload();
    EthernetPayload(const uint8_t* data, const size_t size);

    uint16_t getFlags() const;
    void setFlags(const uint16_t newFlags);
    bool getFlag(const Flags mask) const;
    void setFlag(const Flags mask, const bool value);

    uint16_t getDataLength() const;
    const uint8_t* getData() const;

    static bool isValidPayload(const uint8_t* data, const size_t size);

protected:
    const Header* getHeader() const;
    Header* getHeader();

private:
    static constexpr uint16_t errorMask = 0x003B;
};

END_NAMESPACE_ASAM_CMP
