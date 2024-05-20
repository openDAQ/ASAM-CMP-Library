#pragma once

#include <cstdint>
#include <vector>

#include <asam_cmp/common.h>

#pragma pack(push, 1)

struct CmpMessageHeader
{
    uint8_t version{1};
    uint8_t reserved{0};
    uint16_t deviceId{0};
    uint8_t messageType{0};
    uint8_t streamId{0};
    uint16_t sequenceCounter{0};
};

struct DataMessageHeader
{
    uint64_t timestamp{0};
    uint32_t interfaceId{0};
    uint8_t flags{0};
    uint8_t payloadType{0};
    uint16_t payloadLength{0};
};

struct CanDataMessageHeader
{
    uint16_t flags{0};
    uint16_t reserved{0};
    uint32_t id{0};
    uint32_t crc{0};
    uint16_t errorPosition{0};
    uint8_t dlc{0};
    uint8_t dataLength{0};
};

#pragma pack(pop)

template <typename Header>
std::vector<uint8_t> createMessage(Header&& header, const std::vector<uint8_t>& data)
{
    size_t messageSize = sizeof(header) + data.size();
    std::vector<uint8_t> message(messageSize);
    memcpy(message.data(), &header, sizeof(header));
    std::copy(data.begin(), data.end(), std::next(message.begin(), sizeof(header)));

    return message;
}

inline std::vector<uint8_t> createCanDataMessage(uint32_t arbId, const std::vector<uint8_t>& data)
{
    CanDataMessageHeader header;
    header.id = ASAM::CMP::swapEndian(arbId);
    header.dataLength = ASAM::CMP::swapEndian(static_cast<uint8_t>(data.size()));

    return createMessage(header, data);
}

inline std::vector<uint8_t> createDataMessage(uint8_t payloadType, std::vector<uint8_t> payload)
{
    DataMessageHeader header;
    header.payloadType = ASAM::CMP::swapEndian(payloadType);
    header.payloadLength = ASAM::CMP::swapEndian(static_cast<uint16_t>(payload.size()));

    return createMessage(header, payload);
}

inline std::vector<uint8_t> createCmpMessage(uint16_t deviceId, uint8_t messageType, uint8_t streamId, std::vector<uint8_t> payload)
{
    CmpMessageHeader header;
    header.deviceId = ASAM::CMP::swapEndian(deviceId);
    header.messageType = ASAM::CMP::swapEndian(messageType);
    header.streamId = ASAM::CMP::swapEndian(streamId);

    return createMessage(header, payload);
}
