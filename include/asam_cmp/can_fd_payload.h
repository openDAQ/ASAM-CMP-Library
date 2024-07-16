#pragma once

#include <asam_cmp/can_payload_base.h>
#include <asam_cmp/common.h>

BEGIN_NAMESPACE_ASAM_CMP

class CanFdPayload : public CanPayloadBase
{
public:
    CanFdPayload();
    CanFdPayload(const uint8_t* data, const size_t size);

    bool getRrs() const;
    void setRrs(const bool rrs);
    uint32_t getCrc() const;
    void setCrc(const uint32_t newCrcSbc);
    uint8_t getSbc() const;
    void setSbc(const uint8_t sbc);
    bool getSbcParity() const;
    void setSbcParity(const bool parity);
    bool getSbcSupport() const;
    void setSbcSupport(const bool support);
};

END_NAMESPACE_ASAM_CMP
