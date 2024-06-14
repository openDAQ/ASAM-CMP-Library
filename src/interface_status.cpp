#include <asam_cmp/interface_status.h>
#include <asam_cmp/interface_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

void InterfaceStatus::update(const Packet& packet)
{
    interfaceId = static_cast<const InterfacePayload&>(packet.getPayload()).getInterfaceId();
    interfacePacket = packet;
}

Packet& InterfaceStatus::getPacket()
{
    return interfacePacket;
}

const Packet& InterfaceStatus::getPacket() const
{
    return interfacePacket;
}

uint32_t InterfaceStatus::getInterfaceId() const
{
    return interfaceId;
}

END_NAMESPACE_ASAM_CMP
