#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

class CanPayloadBase : public Payload
{
public:
    enum class Flags : uint16_t
    {
        crcErr = 0x0001,
        ackErr = 0x0002,
        passiveAckErr = 0x0004,
        activeAckErr = 0x0008,
        ackDelErr = 0x0010,
        formErr = 0x0020,
        stuffErr = 0x0040,
        crcDelErr = 0x0080,
        eofErr = 0x0100,
        bitErr = 0x0200,
        r0 = 0x0400,
        rsvd = r0,
        srrDom = 0x0800,
        brs = 0x1000,
        esi = 0x2000
    };

#pragma pack(push, 1)
    class Header final
    {
        uint16_t flags{0};
        uint16_t reserved{0};
        uint32_t id{0};
        uint32_t crc{0};
        uint16_t errorPosition{0};
        uint8_t dlc{0};
        uint8_t dataLength{0};

    private:
        static constexpr uint16_t errorMask =   0xFF03;

        static constexpr uint32_t idMask =      0xFFFFFF1F;

        static constexpr uint32_t rsvdMask =    0x00000020;
        static constexpr uint32_t rtrMask =     0x00000040;
        static constexpr uint32_t ideMask =     0x00000080;

        static constexpr uint32_t crcMask =         0xFF7F0000;
        static constexpr uint32_t crcSupportMask =  0x00000080;
        static constexpr uint32_t crcSbcMask =      0xFFFF1F00;
        static constexpr uint32_t crcSbcSbcMask =   0x0000E000;
        static constexpr uint32_t crcSbcSbcShift = 21;
        static constexpr uint32_t crcSbcParityMask =  0x00000001;
        static constexpr uint32_t crcSbcSupportMask = 0x00000040;

    public:
        uint16_t getFlags() const;
        void setFlags(const uint16_t flags);
        bool getFlag(Flags mask) const;
        void setFlag(Flags mask, bool value);
        uint32_t getId() const;
        void setId(const uint32_t newId);
        bool getRsvd() const;
        void setRsvd(const bool rsvd);
        bool getRtrRrs() const;
        void setRtrRrs(const bool rtrRrs);
        bool getIde() const;
        void setIde(const bool ide);
        uint16_t getCrc() const;
        void setCrc(const uint16_t newCrc);
        bool getCrcSupport() const;
        void setCrcSupport(const bool support);
        uint32_t getCrcSbc() const;
        void setCrcSbc(const uint32_t newCrcSbc);
        uint8_t getSbc() const;
        void setSbc(const uint8_t sbc);
        bool getSbcParity() const;
        void setSbcParity(const bool parity);
        bool getSbcSupport() const;
        void setSbcSupport(const bool support);
        uint16_t getErrorPosition() const;
        void setErrorPosition(const uint16_t position);
        bool hasError() const;
        uint8_t getDlc() const;
        void setDlc(const uint8_t newDlc);
        uint8_t getDataLength() const;
        void setDataLength(const uint8_t length);
    };
#pragma pack(pop)

public:
    uint16_t getFlags() const;
    void setFlags(const uint16_t flags);
    bool getFlag(const Flags mask) const;
    void setFlag(const Flags mask, const bool value);
    uint32_t getId() const;
    void setId(const uint32_t id);
    bool getRsvd() const;
    void setRsvd(const bool rsvd);
    bool getIde() const;
    void setIde(const bool ide);
    bool getCrcSupport() const;
    void setCrcSupport(const bool support);
    uint16_t getErrorPosition() const;
    void setErrorPosition(const uint16_t position);

    uint8_t getDlc() const;
    uint8_t getDataLength() const;
    const uint8_t* getData() const;
    void setData(const uint8_t* data, const uint8_t dataLength);

    static bool isValidPayload(const uint8_t* data, const size_t size);

protected:
    CanPayloadBase(const PayloadType type, const size_t size);
    CanPayloadBase(const PayloadType type, const uint8_t* data, const size_t size);

    const Header* getHeader() const;
    Header* getHeader();
    uint8_t encodeDlc(const uint8_t dataLength);
};

END_NAMESPACE_ASAM_CMP
