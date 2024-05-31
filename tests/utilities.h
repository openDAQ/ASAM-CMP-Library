#pragma once

#include <asam_cmp/packet.h>
#include <asam_cmp/payload.h>
#include <asam_cmp/common.h>

BEGIN_NAMESPACE_ASAM_CMP

bool isEqualPayloads(const Payload& lhs, const Payload& rhs);
bool isSamePacket(const Packet& lhs, const Packet& rhs);

END_NAMESPACE_ASAM_CMP
