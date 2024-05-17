#include <asam_cmp/can_frame_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

CanFramePayload::CanFramePayload(const uint8_t* data, const size_t size)
    : Payload(Payload::PayloadType::can, data, size)
{
}

uint32_t CanFramePayload::getArbId() const
{
    return swap_endian(reinterpret_cast<const CanPayloadHeader*>(payloadData.data())->id);
}

uint8_t CanFramePayload::getLength() const
{
    return reinterpret_cast<const CanPayloadHeader*>(payloadData.data())->data_length;
}

const uint8_t* CanFramePayload::getData() const
{
    return payloadData.data() + sizeof(CanPayloadHeader);
}

END_NAMESPACE_ASAM_CMP
