#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

class AnalogPayload : public Payload
{
public:
    enum class SampleDt : uint16_t
    {
        aInt16 = 0x0000,
        aInt32 = 0x0100
    };

    enum class Unit : uint8_t
    {
        undefined = 0x00,

        // SI Base units
        meter = 0x02,
        kilogram = 0x03,
        second = 0x01,
        ampere = 0x04,
        kelvin = 0x05,
        mole = 0x06,
        candela = 0x07,

        // Derived units with special names and symbols
        joule = 0x0D,
        newton = 0x0B,
        coulomb = 0x0F,
        katal = 0x1D,
        lux = 0x19,
        watt = 0x0E,
        _pascal = 0x0C,
        volt = 0x10,
        weber = 0x14,
        tesla = 0x15,
        lumen = 0x18,
        hertz = 0x08,
        becquerel = 0x1A,
        gray = 0x1B,
        ohm = 0x12,
        henry = 0x16,
        farad = 0x11,
        steradian = 0x0A,
        sievert = 0x1C,
        siemens = 0x13,
        radian = 0x09,
        degreeCelsius = 0x17,

        // Kinematic SI derived units
        meterPerSecond = 0x1E,
        meterPerSecondSquared = 0x1F,
        meterPerSecondCubed = 0x20,
        meterPerSecondToTheFourth = 0x21,
        radianPerSecond = 0x22,
        radianPerSecondSquared = 0x23,
        hzPerSecond = 0x24,
        cubicMeterPerSecond = 0x25,

        // Mechanical SI derived units
        squareMeter = 0x26,
        cubicMeter = 0x27,
        newtonSecond = 0x28,
        newtonMeterPerSecond = 0x29,
        newtonMeter = 0x2A,
        kilogramPerSquareMeter = 0x2B,
        kilogramPerCubicMeter = 0x2C,
        cubicMeterPerKilogram = 0x2D,
        jouleSecond = 0x2E,
        joulePerKilogram = 0x2F,
        joulePerCubicMeter = 0x30,
        newtonPerMeter = 0x31,
        wattPerSquareMeter = 0x32,
        squareMeterPerSecond = 0x33,
        pascalSecond = 0x34,
        kilogramPerSecond = 0x35,
        wattPerSteradianCubicMeter = 0x36,
        grayPerSecond = 0x37,
        meterPerCubicMeter = 0x38,
        wattPerCubicMeter = 0x39,
        joulePerSquareMeterSecond = 0x3A,
        kilogramSquareMeter = 0x3B,
        wattPerSteradian = 0x3C,

        // Molar SI derived units
        molPerCubicMeter = 0x3d,
        cubicMeterPerMole = 0x3e,
        joulePerKelvinMole = 0x3f,
        joulePerMole = 0x40,
        molePerKilogram = 0x41,
        kilogramPerMole = 0x42,

        // Electromagnetic SI derived units
        coulombPerMeter = 0x43,
        coulombPerSquareMeter = 0x44,
        coulombPerCubicMeter = 0x45,
        amperePerSquareMeter = 0x46,
        siemensPerMeter = 0x47,
        faradPerMeter = 0x48,
        henryPerMeter = 0x49,
        voltPerMeter = 0x4a,
        amperePerMeter = 0x4b,
        coulombPerKilogram = 0x4c,
        ampereSquareMeter = 0x4d,

        // Photometric SI derived units
        lumenSecond = 0x4e,
        luxSecond = 0x4f,
        candelaPerSquareMeter = 0x50,
        lumenPerW = 0x51,

        // Thermodynamic SI derived units
        joulePerKelvin = 0x52,
        joulePerKilogramKelvin = 0x53,
        wattPerMeterKelvin = 0x54
    };

#pragma pack(push, 1)
    class Header
    {
        static_assert(sizeof(float) == 4, "Supports 4-byte float type only");

        uint16_t flags{0};
        uint8_t reserved{0};
        uint8_t unit{0};
        float sampleInterval{0.f};
        float sampleOffset{0.f};
        float sampleScalar{0.f};

    private:
        static constexpr uint16_t sampleDtMask = 0x0300;

    public:
        uint16_t getFlags() const;
        void setFlags(uint16_t newFlags);
        SampleDt getSampleDt() const;
        void setSampleDt(const SampleDt sampleDt);
        Unit getUnit() const;
        void setUnit(const Unit newUnit);
        float getSampleInterval() const;
        void setSampleInterval(const float interval);
        float getSampleOffset() const;
        void setSampleOffset(const float offset);
        float getSampleScalar() const;
        void setSampleScalar(const float scalar);
    };
#pragma pack(pop)

public:
    AnalogPayload();
    AnalogPayload(const uint8_t* data, const size_t size);

    uint16_t getFlags() const;
    void setFlags(const uint16_t flags);
    SampleDt getSampleDt() const;
    void setSampleDt(const SampleDt sampleDt);
    Unit getUnit() const;
    void setUnit(const Unit unit);
    float getSampleInterval() const;
    void setSampleInterval(const float sampleInterval);
    float getSampleOffset() const;
    void setSampleOffset(const float sampleOffset);
    float getSampleScalar() const;
    void setSampleScalar(const float sampleScalar);

    size_t getSamplesCount() const;
    const uint8_t* getData() const;
    void setData(const uint8_t* data, const size_t size);

    static bool isValidPayload(const uint8_t* data, const size_t size);

protected:
    const Header* getHeader() const;
    Header* getHeader();
};

END_NAMESPACE_ASAM_CMP
