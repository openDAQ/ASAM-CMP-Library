#include <asam_cmp/can_fd_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

CanFdPayload::CanFdPayload(const uint8_t* data, const size_t size)
    : CanPayloadBase(Type::canFd, data, size)
{
}

bool CanFdPayload::getRrs() const
{
    return getHeader()->getRtrRrs();
}

void CanFdPayload::setRrs(const bool rrs)
{
    getHeader()->setRtrRrs(rrs);
}

uint32_t CanFdPayload::getCrc() const
{
    return getHeader()->getCrcSbc();
}

void CanFdPayload::setCrc(const uint32_t newCrcSbc)
{
    getHeader()->setCrcSbc(newCrcSbc);
}

uint8_t CanFdPayload::getSbc() const
{
    return getHeader()->getSbc();
}

void CanFdPayload::setSbc(const uint8_t sbc)
{
    getHeader()->setSbc(sbc);
}

bool CanFdPayload::getSbcParity() const
{
    return getHeader()->getSbcParity();
}

void CanFdPayload::setSbcParity(const bool parity)
{
    getHeader()->setSbcParity(parity);
}

bool CanFdPayload::getSbcSupport() const
{
    return getHeader()->getSbcSupport();
}

void CanFdPayload::setSbcSupport(const bool support)
{
    getHeader()->setSbcSupport(support);
}

END_NAMESPACE_ASAM_CMP
