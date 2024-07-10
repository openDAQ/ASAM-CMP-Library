#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

class LinPayload : public Payload
{
public:
    enum class Flags : uint16_t
    {
        checksumErr = 0x0001,
        collisionErr = 0x0002,
        parityErr = 0x0004,
        noSlaveRespErr = 0x0008,
        syncErr = 0x0010,
        framingErr = 0x0020,
        shortDomErr = 0x0040,
        longDomErr = 0x0080,
        wup = 0x0100
    };

#pragma pack(push, 1)
    class Header
    {
        uint16_t flags{0};
        uint16_t reserved1{0};
        uint8_t pid{0};
        uint8_t reserved2{0};
        uint8_t checksum{0};
        uint8_t dataLength{0};

    private:
        static constexpr uint8_t linIdMask = 0x3F;
        static constexpr uint8_t parityMask = 0xC0;
        static constexpr int parityShift = 6;

    public:
        uint16_t getFlags() const;
        void setFlags(uint16_t newFlags);
        bool getFlag(Flags mask) const;
        void setFlag(Flags mask, bool value);
        uint8_t getLinId() const;
        void setLinId(const uint8_t id);
        uint8_t getParityBits() const;
        void setParityBits(const uint8_t parity);
        uint8_t getChecksum() const;
        void setChecksum(const uint8_t newChecksum);
        uint8_t getDataLength() const;
        void setDataLength(const uint8_t length);
    };
#pragma pack(pop)

public:
    LinPayload();
    LinPayload(const uint8_t* data, const size_t size);

    uint16_t getFlags() const;
    void setFlags(uint16_t newFlags);
    bool getFlag(Flags mask) const;
    void setFlag(Flags mask, bool value);
    uint8_t getLinId() const;
    void setLinId(const uint8_t id);
    uint8_t getParityBits() const;
    void setParityBits(const uint8_t parity);
    uint8_t getChecksum() const;
    void setChecksum(const uint8_t checksum);

    uint8_t getDataLength() const;
    const uint8_t* getData() const;
    void setData(const uint8_t* data, const uint8_t dataLength);

    static bool isValidPayload(const uint8_t* data, const size_t size);

protected:
    const Header* getHeader() const;
    Header* getHeader();
};

END_NAMESPACE_ASAM_CMP
