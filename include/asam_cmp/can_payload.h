#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/can_basic_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

class CanPayload : public CanBasicPayload
{
public:
    CanPayload(const uint8_t* data, const size_t size);

    bool getRtr() const;
    void setRtr(const bool rtr);
    uint16_t getCrc() const;
    void setCrc(const uint16_t crc);
};

END_NAMESPACE_ASAM_CMP
