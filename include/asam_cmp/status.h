#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/device_status.h>
#include <asam_cmp/packet.h>

BEGIN_NAMESPACE_ASAM_CMP

class Status final
{
public:
    void update(const Packet& packet);
    void clear();

    // Random access to device status
    std::size_t getDeviceStatusCount() const;
    size_t getIndexByDeviceId(const uint16_t deviceId) const;
    DeviceStatus& getDeviceStatus(std::size_t index);
    const DeviceStatus& getDeviceStatus(std::size_t index) const;

private:
    std::vector<DeviceStatus> devices;
};

END_NAMESPACE_ASAM_CMP
