#pragma once

#include <string>

#include <asam_cmp/common.h>
#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

class InterfacePayload : public Payload
{
public:
    enum class InterfaceStatus : uint8_t
    {
        linkStatusDown = 0x00,
        linkStatusUp = 0x01,
        disabled = 0x02
    };

#pragma pack(push, 1)
    class Header
    {
        uint32_t interfaceId{0};
        uint32_t msgTotalRx{0};
        uint32_t msgTotalTx{0};
        uint32_t msgDroppedRx{0};
        uint32_t msgDroppedTx{0};
        uint32_t errorsTotalRx{0};
        uint32_t errorsTotalTx{0};
        uint8_t interfaceType{0};
        uint8_t interfaceStatus{0};
        uint16_t reserved{0};
        uint32_t featureSupportBitmask{0};

    public:
        uint32_t getInterfaceId() const;
        void setInterfaceId(const uint32_t id);
        uint32_t getMsgTotalRx() const;
        void setMsgTotalRx(const uint32_t msgTotal);
        uint32_t getMsgTotalTx() const;
        void setMsgTotalTx(const uint32_t msgTotal);
        uint32_t getMsgDroppedRx() const;
        void setMsgDroppedRx(const uint32_t msgDropped);
        uint32_t getMsgDroppedTx() const;
        void setMsgDroppedTx(const uint32_t msgDropped);
        uint32_t getErrorsTotalRx() const;
        void setErrorsTotalRx(const uint32_t errorsTotal);
        uint32_t getErrorsTotalTx() const;
        void setErrorsTotalTx(const uint32_t errorsTotal);
        uint8_t getInterfaceType() const;
        void setInterfaceType(const uint8_t ifType);
        InterfaceStatus getInterfaceStatus() const;
        void setInterfaceStatus(const InterfaceStatus status);
        uint32_t getFeatureSupportBitmask() const;
        void setFeatureSupportBitmask(const uint32_t bitmask);
    };
#pragma pack(pop)

public:
    InterfacePayload();
    InterfacePayload(const uint8_t* data, const size_t size);

    uint32_t getInterfaceId() const;
    void setInterfaceId(const uint32_t id);
    uint32_t getMsgTotalRx() const;
    void setMsgTotalRx(const uint32_t msgTotal);
    uint32_t getMsgTotalTx() const;
    void setMsgTotalTx(const uint32_t msgTotal);
    uint32_t getMsgDroppedRx() const;
    void setMsgDroppedRx(const uint32_t msgDropped);
    uint32_t getMsgDroppedTx() const;
    void setMsgDroppedTx(const uint32_t msgDropped);
    uint32_t getErrorsTotalRx() const;
    void setErrorsTotalRx(const uint32_t errorsTotal);
    uint32_t getErrorsTotalTx() const;
    void setErrorsTotalTx(const uint32_t errorsTotal);
    uint8_t getInterfaceType() const;
    void setInterfaceType(const uint8_t ifType);
    InterfaceStatus getInterfaceStatus() const;
    void setInterfaceStatus(const InterfaceStatus status);
    uint32_t getFeatureSupportBitmask() const;
    void setFeatureSupportBitmask(const uint32_t bitmask);

    uint16_t getStreamIdsCount() const;
    const uint8_t* getStreamIds() const;
    uint16_t getVendorDataLength() const;
    const uint8_t* getVendorData() const;
    void setData(const uint8_t* streamIds, const uint16_t streamIdsCount, const uint8_t* vendorData, const uint16_t vendorDataLength);

    static bool isValidPayload(const uint8_t* data, const size_t size);

protected:
    const Header* getHeader() const;
    Header* getHeader();

    static constexpr size_t minPayloadSize = sizeof(Header) + 2 * sizeof(int16_t);

private:
    const uint8_t* getStreamIdCountPtr() const;
    const uint8_t* getVendorDataLengthPtr() const;
    uint16_t toUint16(const uint8_t* ptr) const;
};

END_NAMESPACE_ASAM_CMP
