#include <algorithm>

#include <asam_cmp/status.h>

BEGIN_NAMESPACE_ASAM_CMP

void Status::update(const Packet& packet)
{
    auto index = getIndexByDeviceId(packet.getDeviceId());
    if (index < getDeviceStatusCount())
    {
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

size_t Status::getIndexByDeviceId(const uint16_t deviceId) const
{
    auto devIt = std::find_if(
        devices.begin(), devices.end(), [&deviceId](const DeviceStatus& device) { return device.getPacket().getDeviceId() == deviceId; });
    return std::distance(devices.begin(), devIt);
}

DeviceStatus& Status::getDeviceStatus(std::size_t index)
{
    return devices[index];
}

const DeviceStatus& Status::getDeviceStatus(std::size_t index) const
{
    return devices[index];
}
void Status::removeDeviceById(uint16_t deviceId)
{
    auto index = getIndexByDeviceId(deviceId);
    if (index != getDeviceStatusCount())
    {
        std::swap(devices[index], devices[devices.size() - 1]);
        devices.pop_back();
    }
}

END_NAMESPACE_ASAM_CMP
