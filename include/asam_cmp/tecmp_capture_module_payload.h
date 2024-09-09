#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/tecmp_payload.h>
#include <string>

BEGIN_NAMESPACE_TECMP

class CaptureModulePayload : public Payload
{
#pragma pack(push, 1)
    class Header
    {
        uint8_t vendorId{0};
        uint8_t deviceVersion{0};

        uint8_t deviceType{0};
        uint8_t reserved{0};
        uint16_t vendorDataLength{0};
        uint16_t deviceId{0};
        uint32_t serialNumber{0};

        struct VendorData
        {
            uint8_t reserved{0};
            struct SwVersion
            {
                uint8_t major{0};
                uint8_t minor{0};
                uint8_t patch{0};
            } swVersion;
            struct HwVersion
            {
                uint8_t major{0};
                uint8_t minor{0};
            } hwVersion;
            uint8_t bufferFill{0};
            uint8_t isBufferOverflow{0};
            uint32_t bufferSize{0};
            uint64_t lifecycle{0};

            struct Voltage
            {
                uint8_t whole{0};
                uint8_t frac{0};
            } voltage;

            uint8_t chassisTemp{0};
            uint8_t silliconTemp{0};
        } vendorData;

    public:
        uint8_t getVendorId() const;
        void setVendorId(uint8_t newId);

        uint8_t getDeviceVersion() const;
        void setDeviceVersion(uint8_t newDeviceVersion);

        uint8_t getDeviceType() const;
        void setDeviceType(uint8_t newDeviceType);

        uint16_t getVendorDataLength() const;
        void setVendorDataLength(uint16_t newVendorDataLength);

        uint16_t getDeviceId() const;
        void setDeviceId(uint16_t newDeviceId);

        uint32_t getSerialNumber() const;
        void setSerialNumber(uint32_t newSerialNumber);

        uint8_t getSwVersionMajor() const;
        void setSwVersionMajor(uint8_t newValue);

        uint8_t getSwVersionMinor() const;
        void setSwVersionMinor(uint8_t newValue);

        uint8_t getSwVersionPatch() const;
        void setSwVersionPatch(uint8_t newValue);

        uint8_t getHwVersionMajor() const;
        void setHwVersionMajor(uint8_t newValue);

        uint8_t getHwVersionMinor() const;
        void setHwVersionMinor(uint8_t newValue);

        uint8_t getVoltageWhole() const;
        void setVoltageWhole(uint8_t newValue);

        uint8_t getVoltageFraction() const;
        void setVoltageFraction(uint8_t newValue);

        uint8_t getBufferFill() const;
        void setBufferFill(uint8_t val);

        uint8_t getIsBufferOverflow() const;
        void setIsBufferOverflow(uint8_t val);

        uint32_t getBufferSize() const;
        void setBufferSize(uint32_t val);

        uint64_t getLifecycle() const;
        void setLifecycle(uint64_t val);

        uint8_t getChassisTemp() const;
        void setChassisTemp(uint8_t val);

        uint8_t getSilliconTemp() const;
        void setSilliconTemp(uint8_t val);
    };
#pragma pack(pop)

public:
    CaptureModulePayload();
    CaptureModulePayload(const uint8_t* data, const size_t size);

    uint8_t getVendorId() const;
    void setVendorId(uint8_t newId);

    uint8_t getDeviceVersion() const;
    void setDeviceVersion(uint8_t newDeviceVersion);

    uint8_t getDeviceType() const;
    void setDeviceType(uint8_t newDeviceType);

    uint16_t getVendorDataLength() const;
    void setVendorDataLength(uint16_t newVendorDataLength);

    uint16_t getDeviceId() const;
    void setDeviceId(uint16_t newDeviceId);

    uint32_t getSerialNumber() const;
    void setSerialNumber(uint32_t newSerialNumber);

    uint8_t getSwVersionMajor() const;
    void setSwVersionMajor(uint8_t newValue);

    uint8_t getSwVersionMinor() const;
    void setSwVersionMinor(uint8_t newValue);

    uint8_t getSwVersionPatch() const;
    void setSwVersionPatch(uint8_t newValue);

    uint8_t getHwVersionMajor() const;
    void setHwVersionMajor(uint8_t newValue);

    uint8_t getHwVersionMinor() const;
    void setHwVersionMinor(uint8_t newValue);

    uint8_t getVoltageWhole() const;
    void setVoltageWhole(uint8_t newValue);

    uint8_t getVoltageFraction() const;
    void setVoltageFraction(uint8_t newValue);

    uint8_t getBufferFill() const;
    void setBufferFill(uint8_t val);

    uint8_t getIsBufferOverflow() const;
    void setIsBufferOverflow(uint8_t val);

    uint32_t getBufferSize() const;
    void setBufferSize(uint32_t val);

    uint64_t getLifecycle() const;
    void setLifecycle(uint64_t val);

    uint8_t getChassisTemp() const;
    void setChassisTemp(uint8_t val);

    uint8_t getSilliconTemp() const;
    void setSilliconTemp(uint8_t val);

    std::string getSwVersion() const;
    std::string getHwVersion() const;
    float getVoltage() const;

protected:
    const Header* getHeader() const;
    Header* getHeader();
};

END_NAMESPACE_TECMP
