#pragma once

#include <cstdint>
#include <vector>

#include <asam_cmp/common.h>
#include <asam_cmp/can_payload.h>
#include <asam_cmp/ethernet_payload.h>
#include <asam_cmp/decoder.h>

template <typename Header>
std::vector<uint8_t> createMessage(Header&& header, const std::vector<uint8_t>& data)
{
    size_t messageSize = sizeof(header) + data.size();
    std::vector<uint8_t> message(messageSize);
    memcpy(message.data(), &header, sizeof(header));
    std::copy(data.begin(), data.end(), std::next(message.begin(), sizeof(header)));

    return message;
}

inline std::vector<uint8_t> createCanDataMessage(const uint32_t arbId, const std::vector<uint8_t>& data)
{
    ASAM::CMP::CanPayload::Header header;
    header.setId(arbId);
    header.setDataLength(static_cast<uint8_t>(data.size()));

    return createMessage(header, data);
}

inline std::vector<uint8_t> createEthernetDataMessage(const std::vector<uint8_t>& data)
{
    ASAM::CMP::EthernetPayload::Header header;
    header.setDataLength(static_cast<uint16_t>(data.size()));

    return createMessage(header, data);
}

inline std::vector<uint8_t> createDataMessage(const ASAM::CMP::Payload::Type payloadType, const std::vector<uint8_t>& payload)
{
    ASAM::CMP::Packet::MessageHeader header;
    header.setPayloadType(payloadType);
    header.setPayloadLength(static_cast<uint16_t>(payload.size()));

    return createMessage(header, payload);
}

inline std::vector<uint8_t> createCmpMessage(const uint16_t deviceId, const uint8_t messageType, const uint8_t streamId, const std::vector<uint8_t>& payload)
{
    ASAM::CMP::Decoder::CmpHeader header;
    header.setDeviceId(deviceId);
    header.setMessageType(messageType);
    header.setStreamId(streamId);

    return createMessage(header, payload);
}
