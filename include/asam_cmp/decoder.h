#pragma once

#include <memory>
#include <vector>

#include <asam_cmp/common.h>
#include <asam_cmp/packet.h>

BEGIN_NAMESPACE_ASAM_CMP

class Decoder
{
public:
    Decoder() = default;

    std::vector<std::shared_ptr<Packet>> decode(const void* data, std::size_t size);

private:
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

#pragma pack(pop)
};

END_NAMESPACE_ASAM_CMP
