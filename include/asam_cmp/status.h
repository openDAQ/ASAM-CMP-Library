/*
 * Copyright 2022-2024 openDAQ d.o.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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

    void removeDeviceById(uint16_t deviceId);

private:
    std::vector<DeviceStatus> devices;
};

END_NAMESPACE_ASAM_CMP
