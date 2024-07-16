#include <algorithm>

#include <asam_cmp/status.h>

BEGIN_NAMESPACE_ASAM_CMP

void Status::update(const Packet& packet)
{
    auto devIt = std::find_if(devices.begin(),
                              devices.end(),
                              [&packet](const DeviceStatus& device) { return device.getPacket().getDeviceId() == packet.getDeviceId(); });
    if (devIt != devices.end())
    {
        auto index = std::distance(devices.begin(), devIt);
        devices[index].update(packet);
    }
    else if (packet.getPayload().getType() == PayloadType::cmStatMsg)
    {
        DeviceStatus deviceStatus;
        deviceStatus.update(packet);
        devices.push_back(std::move(deviceStatus));
    }
}

void Status::clear()
{
    devices.clear();
}

std::size_t Status::getDeviceStatusCount() const
{
    return devices.size();
}

DeviceStatus& Status::getDeviceStatus(std::size_t index)
{
    return devices[index];
}

const DeviceStatus& Status::getDeviceStatus(std::size_t index) const
{
    return devices[index];
}

END_NAMESPACE_ASAM_CMP
