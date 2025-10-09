#include <asam_cmp/tecmp_ethernet_payload.h>

BEGIN_NAMESPACE_TECMP

EthernetPayload::EthernetPayload(const uint8_t* data, size_t size)
    : Payload(TECMP::PayloadType::ethernet, data, size)
{
}

const uint8_t* EthernetPayload::getData() const
{
    return payloadData.data();
}

std::size_t EthernetPayload::getDataLength() const
{
    return payloadData.size();
}

END_NAMESPACE_TECMP
