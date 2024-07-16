#include <algorithm>

#include <asam_cmp/device_status.h>
#include <asam_cmp/interface_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

void DeviceStatus::update(const Packet& packet)
{
    if (packet.getPayload().getType() == PayloadType::ifStatMsg)
        updateInterfaces(packet);

    if (packet.getPayload().getType() == PayloadType::cmStatMsg)
        devicePacket = packet;
}

Packet& DeviceStatus::getPacket()
{
    return devicePacket;
}

const Packet& DeviceStatus::getPacket() const
{
    return devicePacket;
}

std::size_t DeviceStatus::getInterfaceStatusCount() const
{
    return interfaces.size();
}

size_t DeviceStatus::getIndexByInterfaceId(const uint32_t interfaceId) const
{
    auto intIt = std::find_if(interfaces.begin(),
                              interfaces.end(),
                              [&interfaceId](const InterfaceStatus& interface) { return interface.getInterfaceId() == interfaceId; });
    return std::distance(interfaces.begin(), intIt);
}

InterfaceStatus& DeviceStatus::getInterfaceStatus(std::size_t index)
{
    return interfaces[index];
}

const InterfaceStatus& DeviceStatus::getInterfaceStatus(std::size_t index) const
{
    return interfaces[index];
}

void DeviceStatus::updateInterfaces(const Packet& packet)
{
    auto newId = static_cast<const InterfacePayload&>(packet.getPayload()).getInterfaceId();
    auto index = getIndexByInterfaceId(newId);

    if (index != getInterfaceStatusCount())
    {
        interfaces[index].update(packet);
    }
    else
    {
        InterfaceStatus interfaceStatus;
        interfaceStatus.update(packet);
        interfaces.push_back(std::move(interfaceStatus));
    }
}

END_NAMESPACE_ASAM_CMP
