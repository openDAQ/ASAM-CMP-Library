#include <asam_cmp/can_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

CanPayload::CanPayload(const uint8_t* data, const size_t size)
    : CanBasicPayload(Type::can, data, size)
{
}

bool CanPayload::getRtr() const
{
    return getHeader()->getRtrRrs();
}

void CanPayload::setRtr(const bool rtr)
{
    getHeader()->setRtrRrs(rtr);
}

uint16_t CanPayload::getCrc() const
{
    return getHeader()->getCrc();
}

void CanPayload::setCrc(const uint16_t crc)
{
    getHeader()->setCrc(crc);
}

END_NAMESPACE_ASAM_CMP
