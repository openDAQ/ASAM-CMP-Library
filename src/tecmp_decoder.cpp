#include <asam_cmp/tecmp_can_payload.h>
#include <asam_cmp/tecmp_capture_module_payload.h>
#include <asam_cmp/tecmp_converter.h>
#include <asam_cmp/tecmp_decoder.h>
#include <asam_cmp/tecmp_interface_payload.h>
#include <asam_cmp/tecmp_lin_payload.h>

using TECMP::CmpHeader;
using PacketPtr = std::shared_ptr<ASAM::CMP::Packet>;
using TecmpPayloadPtr = std::shared_ptr<TECMP::Payload>;

std::vector<PacketPtr> TECMP::Decoder::Decode(const void* data, const std::size_t size)
{
    std::vector<PacketPtr> packets;
    std::vector<TecmpPayloadPtr> payloads;

    uint8_t* payloadPtr;
    auto header = GetHeader(data, size, &payloadPtr);
    if (!header.isValid() || payloadPtr == nullptr)
        return packets;

    // Handle payload in method - returns vector of payloads
    payloads = HandlePayload(payloadPtr, size - sizeof(header), header);
    if (payloads.empty())
        return packets;

    // Iterate through packets and header, turn into packets
    packets = ConvertPacketsToAsam(payloads, header);

    return packets;
}

TecmpPayloadPtr TECMP::Decoder::GetCaptureModulePayload(const uint8_t* payloadData, const std::size_t size)
{
    CaptureModulePayload payload(payloadData, size);
    if (payload.isValid())
        return std::make_shared<Payload>(payload);

    return {};
}

TECMP::CmpHeader TECMP::Decoder::GetHeader(const void* data, const std::size_t size, uint8_t** payloadPtr)
{
    CmpHeader header;
    *payloadPtr = nullptr;

    if (size < sizeof(CmpHeader))
        return {};

    memcpy(&header, data, sizeof(CmpHeader));

    if (!header.getPayloadLength())
        return {};

    if (size < sizeof(CmpHeader) + header.getPayloadLength())
        return {};

    auto tempPtr = reinterpret_cast<const uint8_t*>(data) + sizeof(CmpHeader);
    *payloadPtr = const_cast<uint8_t*>(tempPtr);
    return header;
}

std::vector<TecmpPayloadPtr> TECMP::Decoder::HandlePayload(const uint8_t* data, const std::size_t size, TECMP::CmpHeader& header)
{
    std::vector<TecmpPayloadPtr> payloads;
    switch (header.getMessageType())
    {
        case CmpHeader::MessageType::cmStatus:
        {
            auto payload = GetCaptureModulePayload(data, size);
            if (payload->getMessageType() == CmpHeader::MessageType::cmStatus)
                payloads.push_back(payload);
            break;
        }
        case CmpHeader::MessageType::data:
        {
            auto payload = GetDataPayload(data, size, header);
            if (payload->getMessageType() == CmpHeader::MessageType::data)
                payloads.push_back(payload);
            break;
        }
        case CmpHeader::MessageType::busStatus:
        {
            payloads = GetInterfacePayload(data, size, header);
            break;
        }
        case CmpHeader::MessageType::configStatus:
        case CmpHeader::MessageType::replayData:
        case CmpHeader::MessageType::invalid:
        case CmpHeader::MessageType::control:
            break;  // Not implimented
    }
    return payloads;
}
TecmpPayloadPtr TECMP::Decoder::GetDataPayload(const uint8_t* payloadData, const std::size_t size, TECMP::CmpHeader& header)
{
    switch (header.getDataType())
    {
        case CmpHeader::DataType::can:
        case CmpHeader::DataType::canFd:
        {
            auto payload = GetCanPayload(payloadData, size);
            if (payload->getMessageType() == CmpHeader::MessageType::data && payload->getType() == PayloadType::can)
                return payload;
            break;
        }
        case CmpHeader::DataType::lin:
        {
            auto payload = GetLinPayload(payloadData, size);
            if (payload->getMessageType() == CmpHeader::MessageType::data && payload->getType() == PayloadType::lin)
                return payload;
            break;
        }
        case CmpHeader::DataType::flexRay:
        case CmpHeader::DataType::uartRs232:
        case CmpHeader::DataType::analog:
        case CmpHeader::DataType::ethernet:
        case CmpHeader::DataType::invalid:
            break;  // Not implimented
    }
    return nullptr;
}
std::vector<TecmpPayloadPtr> TECMP::Decoder::GetInterfacePayload(const uint8_t* payloadData,
                                                                 const std::size_t size,
                                                                 TECMP::CmpHeader& header)
{
    std::vector<TecmpPayloadPtr> payloads;
    if (header.getMessageType() != CmpHeader::MessageType::busStatus)
        return payloads;

    // Get base values
    InterfacePayload payload;
    std::size_t busDataOffset = 12;
    payload.setGenericData(payloadData);

    while (size - busDataOffset >= 12)
    {
        auto tempPayload = payload;
        tempPayload.setBusData(payloadData + busDataOffset, 12);
        payloads.push_back(std::make_shared<Payload>(tempPayload));
        busDataOffset += 12;
    }

    return payloads;
}
TecmpPayloadPtr TECMP::Decoder::GetCanPayload(const uint8_t* payloadData, const std::size_t size)
{
    CanPayload payload(payloadData, size);
    if (payload.isValid())
        return std::make_shared<Payload>(payload);

    return {};
}

TecmpPayloadPtr TECMP::Decoder::GetLinPayload(const uint8_t* payloadData, const std::size_t size)
{
    LinPayload payload(payloadData, size);
    if (payload.isValid())
        return std::make_shared<Payload>(payload);

    return {};
}
std::vector<PacketPtr> TECMP::Decoder::ConvertPacketsToAsam(std::vector<TecmpPayloadPtr> payloads, TECMP::CmpHeader& header)
{
    std::vector<PacketPtr> packets;

    for (const auto& payload : payloads)
    {
        auto packet = TECMP::Converter::ConvertPacket(header, payload);
        if (packet)
            packets.push_back(packet);
    }

    return packets;
}
