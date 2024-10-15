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
