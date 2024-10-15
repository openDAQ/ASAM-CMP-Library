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
