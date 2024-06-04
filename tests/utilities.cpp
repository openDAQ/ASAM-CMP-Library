#include "utilities.h"

BEGIN_NAMESPACE_ASAM_CMP

bool isEqualPayloads(const Payload& lhs, const Payload& rhs)
{
    if (lhs.getType() != rhs.getType())
        return false;

    if (lhs.getSize() != rhs.getSize())
        return false;

    const uint8_t* lhsRaw = lhs.getRawPayload();
    const uint8_t* rhsRaw = rhs.getRawPayload();

    for (size_t i = 0; i < lhs.getSize(); ++i)
        if (lhsRaw[i] != rhsRaw[i])
            return false;

    return true;
}

bool isSamePacket(const Packet& lhs, const Packet& rhs)
{
    if (lhs.getDeviceId() != rhs.getDeviceId())
        return false;

    if (lhs.getMessageType() != rhs.getMessageType())
        return false;

    if (lhs.getStreamId() != rhs.getStreamId())
        return false;

    if (lhs.getVersion() != rhs.getVersion())
        return false;

    return isEqualPayloads(lhs.getPayload(), rhs.getPayload()) && (lhs.getPayload().getRawPayload() != rhs.getPayload().getRawPayload());
}

END_NAMESPACE_ASAM_CMP
