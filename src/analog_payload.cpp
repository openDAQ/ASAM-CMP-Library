#include <asam_cmp/analog_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

uint16_t AnalogPayload::Header::getFlags() const
{
    return swapEndian(flags);
}

void AnalogPayload::Header::setFlags(const uint16_t newFlags)
{
    flags = swapEndian(newFlags);
}

AnalogPayload::SampleDt AnalogPayload::Header::getSampleDt() const
{
    return static_cast<SampleDt>(flags & sampleDtMask);
}

void AnalogPayload::Header::setSampleDt(const SampleDt sampleDt)
{
    flags &= ~sampleDtMask;
    flags |= to_underlying(sampleDt);
}

AnalogPayload::Unit AnalogPayload::Header::getUnit() const
{
    return static_cast<AnalogPayload::Unit>(unit);
}

void AnalogPayload::Header::setUnit(const Unit newUnit)
{
    unit = to_underlying(newUnit);
}

float AnalogPayload::Header::getSampleInterval() const
{
    return swapEndian(sampleInterval);
}

void AnalogPayload::Header::setSampleInterval(const float interval)
{
    sampleInterval = swapEndian(interval);
}

float AnalogPayload::Header::getSampleOffset() const
{
    return swapEndian(sampleOffset);
}

void AnalogPayload::Header::setSampleOffset(const float offset)
{
    sampleOffset = swapEndian(offset);
}

float AnalogPayload::Header::getSampleScalar() const
{
    return swapEndian(sampleScalar);
}

void AnalogPayload::Header::setSampleScalar(const float scalar)
{
    sampleScalar = swapEndian(scalar);
}

AnalogPayload::AnalogPayload(const uint8_t* data, const size_t size)
    : Payload(Type::analog, data, size)
{
}

uint16_t AnalogPayload::getFlags() const
{
    return getHeader()->getFlags();
}

void AnalogPayload::setFlags(uint16_t flags)
{
    getHeader()->setFlags(flags);
}

AnalogPayload::SampleDt AnalogPayload::getSampleDt() const
{
    return getHeader()->getSampleDt();
}

void AnalogPayload::setSampleDt(const SampleDt sampleDt)
{
    getHeader()->setSampleDt(sampleDt);
}

AnalogPayload::Unit AnalogPayload::getUnit() const
{
    return getHeader()->getUnit();
}

void AnalogPayload::setUnit(const Unit unit)
{
    getHeader()->setUnit(unit);
}

float AnalogPayload::getSampleInterval() const
{
    return getHeader()->getSampleInterval();
}

void AnalogPayload::setSampleInterval(const float sampleInterval)
{
    getHeader()->setSampleInterval(sampleInterval);
}

float AnalogPayload::getSampleOffset() const
{
    return getHeader()->getSampleOffset();
}

void AnalogPayload::setSampleOffset(const float sampleOffset)
{
    getHeader()->setSampleOffset(sampleOffset);
}

float AnalogPayload::getSampleScalar() const
{
    return getHeader()->getSampleScalar();
}

void AnalogPayload::setSampleScalar(const float sampleScalar)
{
    getHeader()->setSampleScalar(sampleScalar);
}

size_t AnalogPayload::getSamplesCount() const
{
    auto samplesSize = (getSize() - sizeof(Header));
    return getHeader()->getSampleDt() == SampleDt::aInt16 ? samplesSize / 2 : samplesSize / 4;
}

const uint8_t* AnalogPayload::getData() const
{
    return payloadData.data() + sizeof(Header);
}

bool AnalogPayload::isValidPayload(const uint8_t* data, const size_t size)
{
    auto header = reinterpret_cast<const Header*>(data);
    return (size >= sizeof(Header)) &&
           ((header->getSampleDt() == AnalogPayload::SampleDt::aInt16) || (header->getSampleDt() == AnalogPayload::SampleDt::aInt32));
}

const AnalogPayload::Header* AnalogPayload::getHeader() const
{
    return reinterpret_cast<const Header*>(payloadData.data());
}

AnalogPayload::Header* AnalogPayload::getHeader()
{
    return reinterpret_cast<Header*>(payloadData.data());
}

END_NAMESPACE_ASAM_CMP
