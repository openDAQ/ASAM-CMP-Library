#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/tecmp_payload.h>

BEGIN_NAMESPACE_TECMP

class LinPayload : public Payload
{
#pragma pack(push, 1)
    class Header
    {
        uint8_t pid;
        uint8_t dataLength;

    public:
        uint8_t getPid() const;
        void setPid(uint8_t newPid);

        uint8_t getDataLength() const;
        void setDataLength(uint8_t newLength);
    };
#pragma pack(pop)
public:
    uint8_t getPid() const;
    void setPid(uint8_t newPid);

    uint8_t getDataLength() const;
    void setDataLength(uint8_t newLength);

public:
    LinPayload();
    LinPayload(const uint8_t* data, const size_t size);

    const uint8_t* getData() const;
    void setData(const uint8_t* data, const uint8_t dataLength);

    uint8_t getCrc() const;

protected:
    const Header* getHeader() const;
    Header* getHeader();
};

END_NAMESPACE_TECMP
