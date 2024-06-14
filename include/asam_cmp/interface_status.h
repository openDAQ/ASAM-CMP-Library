#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/packet.h>

BEGIN_NAMESPACE_ASAM_CMP

class InterfaceStatus final
{
public:
    void update(const Packet& packet);
    Packet& getPacket();
    const Packet& getPacket() const;

    uint32_t getInterfaceId() const;

private:
    Packet interfacePacket;
    uint32_t interfaceId{0};
};

END_NAMESPACE_ASAM_CMP
