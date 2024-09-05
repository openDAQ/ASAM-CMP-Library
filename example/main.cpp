#include <cassert>
#include <chrono>
#include <numeric>
#include <type_traits>

#include <asam_cmp/analog_payload.h>
#include <asam_cmp/can_payload.h>
#include <asam_cmp/capture_module_payload.h>
#include <asam_cmp/decoder.h>
#include <asam_cmp/encoder.h>
#include <asam_cmp/interface_payload.h>
#include <asam_cmp/status.h>

// Returns the number of nanoseconds since the start of time.
uint64_t getCurrentTimestamp()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

// Creates packet with current timestamp.
std::shared_ptr<ASAM::CMP::Packet> createPacket(const uint16_t deviceId, const uint32_t interfaceId, const uint8_t streamId)
{
    auto packet = std::make_shared<ASAM::CMP::Packet>();
    packet->setDeviceId(deviceId);
    packet->setInterfaceId(interfaceId);
    packet->setStreamId(streamId);
    packet->setTimestamp(getCurrentTimestamp());

    return packet;
}

// Creates CAN payload.
ASAM::CMP::CanPayload createCanPayload()
{
    constexpr uint32_t arbId = 14;

    std::vector<uint8_t> canData(8);
    std::iota(canData.begin(), canData.end(), uint8_t{});
    ASAM::CMP::CanPayload canPayload;
    canPayload.setId(arbId);
    canPayload.setData(canData.data(), static_cast<uint8_t>(canData.size()));

    return canPayload;
}

// Creates Analog Payload with AnalogType Sample Datatype.
// According to the standard it can be int16_t or int32_t.
template <typename AnalogType>
ASAM::CMP::AnalogPayload createAnalogPayload()
{
    static_assert(std::disjunction_v<std::is_same<AnalogType, int16_t>, std::is_same<AnalogType, int32_t>>,
                  "AnalogType can be either int16_t or int32_t");

    constexpr size_t samplesCount = 128;
    constexpr float sampleInterval = 1e-3f;
    ASAM::CMP::AnalogPayload analogPayload;
    std::vector<AnalogType> analogData(samplesCount);
    std::iota(analogData.begin(), analogData.end(), AnalogType{});
    analogPayload.setSampleDt(ASAM::CMP::AnalogPayload::SampleDtFromType<AnalogType>::sampleDtType);
    analogPayload.setUnit(ASAM::CMP::AnalogPayload::Unit::volt);
    analogPayload.setSampleInterval(sampleInterval);
    analogPayload.setData(reinterpret_cast<uint8_t*>(analogData.data()), analogData.size() * sizeof(AnalogType));

    return analogPayload;
}

void EncodeDecode()
{
    // ASAM CMP Packet properties
    constexpr uint16_t deviceId = 10;
    constexpr uint32_t interfaceId = 20;
    constexpr uint8_t streamId = 30;

    // Packets for encoding
    std::vector<std::shared_ptr<ASAM::CMP::Packet>> packets;

    // Data Packet with CAN payload
    auto packet = createPacket(deviceId, interfaceId, streamId);
    auto canPayload = createCanPayload();
    packet->setPayload(canPayload);
    packets.push_back(packet);

    // Another packet with CAN payload and different timestamp
    packet->setTimestamp(getCurrentTimestamp());
    packets.push_back(packet);

    // Packet with Analog payload
    auto analogPayload = createAnalogPayload<int32_t>();
    packet->setPayload(analogPayload);
    packet->setTimestamp(getCurrentTimestamp());
    packets.push_back(packet);

    // Initialize encoder
    ASAM::CMP::Encoder encoder;
    encoder.setDeviceId(deviceId);
    encoder.setStreamId(streamId);

    // Encode a packet in a bunch of Ethernet frames, whose size is not less than DataContext::minBytesPerMessage
    // and not greater than DataContext::maxBytesPerMessage
    const auto ethernetFrames = encoder.encode(packets.begin(), packets.end(), ASAM::CMP::DataContext{});

    // Decode Ethernet frames into Packets
    ASAM::CMP::Decoder decoder;
    std::vector<std::shared_ptr<ASAM::CMP::Packet>> decodedPackets;
    for (const auto& frame : ethernetFrames)
    {
        const auto packs = decoder.decode(frame.data(), frame.size());
        decodedPackets.insert(decodedPackets.end(), packs.begin(), packs.end());
    }

    // Check equality of source and decoded packets
    bool res =
        std::equal(packets.begin(), packets.end(), decodedPackets.begin(), [](const auto& lhs, const auto& rhs) { return *lhs == *rhs; });

    assert(res);
    assert(packets.size() == decodedPackets.size());
}

void Status()
{
    // ASAM CMP Packet properties
    constexpr uint16_t deviceId = 10;
    constexpr uint32_t interfaceId = 20;
    constexpr uint8_t streamId = 30;

    // Packet with Capture Module Status Message
    auto packet = createPacket(deviceId, interfaceId, streamId);
    ASAM::CMP::CaptureModulePayload cmPayload;
    cmPayload.setData("Device 10", "Serial Number", "Hardware version", "Software version", std::vector<uint8_t>{});
    packet->setPayload(cmPayload);

    // Update status with Capture Module Message
    ASAM::CMP::Status status;
    status.update(*packet);

    // Packet with Interface Status Message
    ASAM::CMP::InterfacePayload ifPayload;
    ifPayload.setInterfaceId(interfaceId);
    packet->setPayload(ifPayload);
    packet->setTimestamp(getCurrentTimestamp());

    // Updates status with Interface Status Message
    status.update(*packet);

    // Another Interface Status Message with different Interface ID
    const auto interfaceId2 = interfaceId + 1;
    packet->setInterfaceId(interfaceId2);
    static_cast<ASAM::CMP::InterfacePayload&>(packet->getPayload()).setInterfaceId(interfaceId2);
    packet->setTimestamp(getCurrentTimestamp());

    status.update(*packet);

    // In the end there is one device with 2 interfaces
    assert(status.getDeviceStatusCount() == 1);
    const auto& deviceStatus = status.getDeviceStatus(0);
    assert(deviceStatus.getInterfaceStatusCount() == 2);
    const auto& interfaceStatus = deviceStatus.getInterfaceStatus(1);
    assert(interfaceStatus.getInterfaceId() == interfaceId2);
}

int main()
{
    // Encoding and decoding different type of protocol data in/from ASAM Capture Module Protocol
    EncodeDecode();

    // Working with ASAM CMP Status Messages
    Status();

    return 0;
}
