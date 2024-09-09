#include <asam_cmp/lin_payload.h>
#include <asam_cmp/tecmp_lin_payload.h>

using ASAM::CMP::swapEndian;

uint8_t TECMP::LinPayload::Header::getPid() const
{
    return swapEndian(pid);
}
void TECMP::LinPayload::Header::setPid(uint8_t newPid)
{
    pid = swapEndian(newPid);
}
uint8_t TECMP::LinPayload::Header::getDataLength() const
{
    return swapEndian(dataLength);
}
void TECMP::LinPayload::Header::setDataLength(uint8_t newLength)
{
    dataLength = swapEndian(newLength);
}
uint8_t TECMP::LinPayload::getPid() const
{
    return getHeader()->getPid();
}
void TECMP::LinPayload::setPid(uint8_t newPid)
{
    getHeader()->setPid(newPid);
}
uint8_t TECMP::LinPayload::getDataLength() const
{
    return getHeader()->getDataLength();
}
void TECMP::LinPayload::setDataLength(uint8_t newLength)
{
    getHeader()->setDataLength(newLength);
}
TECMP::LinPayload::LinPayload()
    : Payload(TECMP::PayloadType::lin, sizeof(Header))
{
}
TECMP::LinPayload::LinPayload(const uint8_t* data, const size_t size)
    : Payload(TECMP::PayloadType::lin, data, size)
{
}
const uint8_t* TECMP::LinPayload::getData() const
{
    return payloadData.data() + sizeof(Header);
}
void TECMP::LinPayload::setData(const uint8_t* data, const uint8_t dataLength)
{
    Payload::setData<Header>(data, dataLength);
    getHeader()->setDataLength(dataLength);
}
uint8_t TECMP::LinPayload::getCrc() const
{
    if (payloadData.size() <= sizeof(Header) + getHeader()->getDataLength())
        return 0;
    return *(payloadData.data() + sizeof(Header) + getHeader()->getDataLength());
}
const TECMP::LinPayload::Header* TECMP::LinPayload::getHeader() const
{
    return reinterpret_cast<const Header*>(payloadData.data());
}
TECMP::LinPayload::Header* TECMP::LinPayload::getHeader()
{
    return reinterpret_cast<Header*>(payloadData.data());
}
