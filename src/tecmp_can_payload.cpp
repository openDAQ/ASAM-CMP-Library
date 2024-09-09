#include <asam_cmp/payload_type.h>
#include <asam_cmp/tecmp_can_payload.h>

using ASAM::CMP::swapEndian;
using TECMP::PayloadType;

uint32_t TECMP::CanPayload::Header::getArbId() const
{
    return swapEndian(arbId);
}

void TECMP::CanPayload::Header::setArbId(uint32_t newArbId)
{
    arbId = swapEndian(newArbId);
}

uint8_t TECMP::CanPayload::Header::getDlc() const
{
    return swapEndian(dlc);
}

void TECMP::CanPayload::Header::setDlc(uint8_t newDlc)
{
    dlc = swapEndian(newDlc);
}

uint32_t TECMP::CanPayload::getArbId() const
{
    return getHeader()->getArbId();
}

void TECMP::CanPayload::setArbId(uint32_t newArbId)
{
    getHeader()->setArbId(newArbId);
}

uint8_t TECMP::CanPayload::getDlc() const
{
    return getHeader()->getDlc();
}

void TECMP::CanPayload::setDlc(uint8_t newDlc)
{
    getHeader()->setDlc(newDlc);
}

TECMP::CanPayload::CanPayload()
    : Payload(TECMP::PayloadType::can, sizeof(Header))
{
}

TECMP::CanPayload::CanPayload(const uint8_t* data, const size_t size)
    : Payload(TECMP::PayloadType::can, data, size)
{
}

const uint8_t* TECMP::CanPayload::getData() const
{
    if (payloadData.size() > sizeof(Header))
        return payloadData.data() + sizeof(Header);

    return nullptr;
}

const TECMP::CanPayload::Header* TECMP::CanPayload::getHeader() const
{
    return reinterpret_cast<const Header*>(payloadData.data());
}

TECMP::CanPayload::Header* TECMP::CanPayload::getHeader()
{
    return reinterpret_cast<Header*>(payloadData.data());
}

uint32_t TECMP::CanPayload::getCrc() const
{
    if (payloadData.size() <= sizeof(Header) + getHeader()->getDlc())
        return 0;

    uint32_t result = 0;
    auto crcOffset = sizeof(Header) + getHeader()->getDlc();
    auto crcPtr = payloadData.data() + crcOffset;
    memcpy((void*) &result, crcPtr, 3);
    return result;
}
