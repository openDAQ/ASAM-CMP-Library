#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/interface_status.h>

BEGIN_NAMESPACE_ASAM_CMP

class DeviceStatus final
{
public:
    void update(const Packet& packet);
    Packet& getPacket();
    const Packet& getPacket() const;

    // Random access to stream status
    std::size_t getInterfaceStatusCount() const;
    size_t getIndexByInterfaceId(const uint32_t interfaceId) const;
    InterfaceStatus& getInterfaceStatus(std::size_t index);
    const InterfaceStatus& getInterfaceStatus(std::size_t index) const;

    void removeInterfaceById(uint32_t interfaceId);

private:
    void updateInterfaces(const Packet& packet);

private:
    std::vector<InterfaceStatus> interfaces;
    Packet devicePacket;
};

END_NAMESPACE_ASAM_CMP
