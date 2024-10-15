/*
 * Copyright 2022-2024 openDAQ d.o.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <asam_cmp/common.h>
#include <asam_cmp/tecmp_payload.h>

BEGIN_NAMESPACE_TECMP

class InterfacePayload : public Payload
{
#pragma pack(push, 1)
    class Header
    {
        uint8_t vendorId{0};
        uint8_t cmVersion{0};
        uint8_t cmType{0};
        uint8_t reserved{0};

        uint16_t vendorDataLength{0};
        uint16_t deviceId{0};
        uint32_t serialNumber{0};

        struct BusData
        {
            uint32_t interfaceId{0};
            uint32_t messagesTotal{0};
            uint32_t errorsTotal{0};
        } busData;

        struct VendorData
        {
            uint8_t linkStatus{0};
            uint8_t linkQuality{0};
            uint16_t linkupTime{0};
        } vendorData;

    public:
        uint8_t getVendorId() const;
        void setVendorId(uint8_t value);

        uint8_t getCmVersion() const;
        void setCmVersion(uint8_t value);

        uint8_t getCmType() const;
        void setCmType(uint8_t value);

        uint16_t getVendorDataLength() const;
        void setVendorDataLength(uint16_t value);

        uint16_t getDeviceId() const;
        void setDeviceId(uint16_t value);

        uint32_t getSerialNumber() const;
        void setSerialNumber(uint32_t value);

        uint32_t getInterfaceId() const;
        void setInterfaceId(uint32_t value);

        uint32_t getMessagesTotal() const;
        void setMessagesTotal(uint32_t value);

        uint32_t getErrorsTotal() const;
        void setErrorsTotal(uint32_t value);

        uint8_t getVendorDataLinkStatus() const;
        void setVendorDataLinkStatus(uint8_t value);

        uint8_t getVendorDataLinkQuality() const;
        void setVendorDataLinkQuality(uint8_t value);

        uint16_t getVendorDataLinkupTime() const;
        void setVendorDataLinkupTime(uint16_t value);
    };
#pragma pack(pop)
public:
    InterfacePayload();
    InterfacePayload(const uint8_t* data, const size_t size);

    uint8_t getVendorId() const;
    void setVendorId(uint8_t value);

    uint8_t getCmVersion() const;
    void setCmVersion(uint8_t value);

    uint8_t getCmType() const;
    void setCmType(uint8_t value);

    uint16_t getVendorDataLength() const;
    void setVendorDataLength(uint16_t value);

    uint16_t getDeviceId() const;
    void setDeviceId(uint16_t value);

    uint32_t getSerialNumber() const;
    void setSerialNumber(uint32_t value);

    uint32_t getInterfaceId() const;
    void setInterfaceId(uint32_t value);

    uint32_t getMessagesTotal() const;
    void setMessagesTotal(uint32_t value);

    uint32_t getErrorsTotal() const;
    void setErrorsTotal(uint32_t value);

    uint8_t getVendorDataLinkStatus() const;
    void setVendorDataLinkStatus(uint8_t value);

    uint8_t getVendorDataLinkQuality() const;
    void setVendorDataLinkQuality(uint8_t value);

    uint16_t getVendorDataLinkupTime() const;
    void setVendorDataLinkupTime(uint16_t value);

    void setGenericData(const uint8_t* data);
    void setBusData(const uint8_t* data, const uint8_t dataLenght);

protected:
    const Header* getHeader() const;
    Header* getHeader();
};
END_NAMESPACE_TECMP