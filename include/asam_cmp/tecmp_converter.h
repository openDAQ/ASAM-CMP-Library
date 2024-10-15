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
#include <asam_cmp/tecmp_can_payload.h>
#include <asam_cmp/tecmp_header.h>
#include <asam_cmp/tecmp_payload.h>

BEGIN_NAMESPACE_TECMP

class Converter final
{
    using PacketPtr = std::shared_ptr<ASAM::CMP::Packet>;
    using TecmpPayloadPtr = std::shared_ptr<TECMP::Payload>;

public:
    static PacketPtr ConvertPacket(CmpHeader& header, const TecmpPayloadPtr& payload);

private:
    static PacketPtr ConvertCaptureModulePayload(CmpHeader& header, const TecmpPayloadPtr& payload);
    static PacketPtr ConvertInterfacePayload(CmpHeader& header, const TecmpPayloadPtr& payload);
    static PacketPtr ConvertDataPayload(CmpHeader& header, const TecmpPayloadPtr& payload);
    static PacketPtr GetPackageFromTecmpHeader(const TECMP::CmpHeader& header);

private:
    static PacketPtr ConvertCanPayload(CmpHeader& header, const TecmpPayloadPtr& payload);
    static PacketPtr ConvertCanFdPayload(TECMP::CanPayload* canPayload, PacketPtr packet);
    static PacketPtr convertLinPayload(CmpHeader& header, const TecmpPayloadPtr& payload);
};

END_NAMESPACE_TECMP