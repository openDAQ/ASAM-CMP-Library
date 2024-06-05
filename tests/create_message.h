#pragma once

#include <cstdint>
#include <vector>

#include <asam_cmp/can_payload.h>
#include <asam_cmp/capture_module_payload.h>
#include <asam_cmp/common.h>
#include <asam_cmp/decoder.h>
#include <asam_cmp/ethernet_payload.h>

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
    header.setDlc(0x0F);

    return createMessage(header, data);
}

inline std::vector<uint8_t> createEthernetDataMessage(const std::vector<uint8_t>& data)
{
    ASAM::CMP::EthernetPayload::Header header;
    header.setDataLength(static_cast<uint16_t>(data.size()));

    return createMessage(header, data);
}

inline std::vector<uint8_t> createCaptureModuleDataMessage(const std::string_view deviceDescription,
                                                           const std::string_view serialNumber,
                                                           const std::string_view hardwareVersion,
                                                           const std::string_view softwareVersion,
                                                           const std::vector<uint8_t>& vendorData)
{
    ASAM::CMP::CaptureModulePayload::Header header;

    constexpr size_t maxNullsCount = 8;
    const size_t msgSize = sizeof(header) + 5 * sizeof(uint16_t) + deviceDescription.size() + serialNumber.size() + hardwareVersion.size() +
                           softwareVersion.size() + vendorData.size() + maxNullsCount;
    std::vector<uint8_t> message(msgSize);
    memcpy(message.data(), &header, sizeof(header));
    auto ptr = message.data() + sizeof(header);

    auto fillFunc = [](uint8_t* ptr, const std::string_view str)
    {
        uint16_t length = static_cast<uint16_t>(str.size()) + 1;
        if (length % 2)
            length += 1;
        auto swappedLength = ASAM::CMP::swapEndian(length);
        memcpy(ptr, &swappedLength, sizeof(length));
        ptr += sizeof(length);
        memcpy(ptr, str.data(), str.size());
        ptr += str.size();
        char nulls[2] = {0, 0};
        memcpy(ptr, nulls, length - str.size());
        ptr += length - str.size();

        return ptr;
    };

    ptr = fillFunc(ptr, deviceDescription);
    ptr = fillFunc(ptr, serialNumber);
    ptr = fillFunc(ptr, hardwareVersion);
    ptr = fillFunc(ptr, softwareVersion);

    uint16_t length = static_cast<uint16_t>(vendorData.size());
    auto swappedLength = ASAM::CMP::swapEndian(length);
    memcpy(ptr, &swappedLength, sizeof(length));
    ptr += sizeof(length);
    memcpy(ptr, vendorData.data(), vendorData.size());
    ptr += vendorData.size();

    size_t newSize = ptr - message.data();
    message.resize(newSize);

    assert((message.size() % 2) == 0);

    return message;
}

inline std::vector<uint8_t> createDataMessage(ASAM::CMP::Payload::Type payloadType, const std::vector<uint8_t>& payload)
{
    ASAM::CMP::Packet::MessageHeader header;
    header.setPayloadType(payloadType);
    header.setPayloadLength(static_cast<uint16_t>(payload.size()));

    return createMessage(header, payload);
}

inline std::vector<uint8_t> createCmpMessage(const uint16_t deviceId,
                                             const ASAM::CMP::Packet::MessageType messageType,
                                             const uint8_t streamId,
                                             const std::vector<uint8_t>& payload)
{
    ASAM::CMP::Decoder::CmpHeader header;
    header.setDeviceId(deviceId);
    header.setMessageType(messageType);
    header.setStreamId(streamId);

    return createMessage(header, payload);
}
