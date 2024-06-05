#pragma once

#include <vector>

#include <asam_cmp/common.h>

BEGIN_NAMESPACE_ASAM_CMP

class Payload
{
public:
    enum class Type : uint8_t
    {
        invalid = 0x00,
        can = 0x01,
        canFd = 0x02,
        lin = 0x03,
        ethernet = 0x08
    };

public:
    Payload(const Type type, const uint8_t* data, const size_t size);
    Payload(const Payload& other) noexcept;
    Payload(Payload&& other) noexcept;

    Payload& operator=(const Payload& other) noexcept;
    Payload& operator=(Payload&& other) noexcept;
    friend bool operator==(const Payload& lhs, const Payload& rhs) noexcept;

    virtual ~Payload() = default;

    Type getType() const;
    size_t getSize() const;
    const uint8_t* getRawPayload() const;

protected:
    Payload() = default;

protected:
    std::vector<uint8_t> payloadData;
    Type type{Type::invalid};

private:
    friend void swap(Payload& lhs, Payload& rhs) noexcept;
};

END_NAMESPACE_ASAM_CMP
