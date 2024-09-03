#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/packet.h>
#include <asam_cmp/tecmp_capture_module_payload.h>
#include <vector>

BEGIN_NAMESPACE_TECMP

class Decoder final
{
public:
    using PacketPtr = std::shared_ptr<ASAM::CMP::Packet>;
    using TecmpPayloadPtr = std::shared_ptr<TECMP::Payload>;

public:
    static std::vector<std::shared_ptr<ASAM::CMP::Packet>> Decode(const void* data, const std::size_t size);

private:
    static TECMP::CmpHeader GetHeader(const void* data, const std::size_t size, uint8_t** payloadPtr);
    static std::vector<TecmpPayloadPtr> HandlePayload(const uint8_t* data, const std::size_t size, TECMP::CmpHeader& header);

    static std::vector<PacketPtr> ConvertPacketsToAsam(std::vector<TecmpPayloadPtr> payloads, TECMP::CmpHeader& header);

private:
    static std::vector<TecmpPayloadPtr> GetInterfacePayload(const uint8_t* payloadData, const std::size_t size, TECMP::CmpHeader& header);
    static TecmpPayloadPtr GetDataPayload(const uint8_t* payloadData, const std::size_t size, TECMP::CmpHeader& header);
    static TecmpPayloadPtr GetCaptureModulePayload(const uint8_t* payloadData, const std::size_t size);
    static TecmpPayloadPtr GetCanPayload(const uint8_t* payloadData, const std::size_t size);
    static TecmpPayloadPtr GetLinPayload(const uint8_t* payloadData, const std::size_t size);
};

END_NAMESPACE_TECMP
