#pragma once

#include <vector>

#include <asam_cmp/common.h>

BEGIN_NAMESPACE_ASAM_CMP

class Payload
{
public:
    enum class PayloadType : uint8_t
    {
        invalid = 0x00,
        can = 0x01,
        can_fd = 0x02,
        lin = 0x03,
        ethernet = 0x08
    };

public:
    Payload(const PayloadType type, const uint8_t* data, const size_t size);
    virtual ~Payload() = default;

    PayloadType getType() const;

protected:
    std::vector<uint8_t> payloadData;

private:
    PayloadType type;
};

END_NAMESPACE_ASAM_CMP
