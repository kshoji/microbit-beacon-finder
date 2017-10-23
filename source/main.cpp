#include "MicroBitCustomConfig.h"
#include "MicroBit.h"
#include "ble/DiscoveredCharacteristic.h"
#include "ble/DiscoveredService.h"

#if YOTTA_CFG_MICROBIT_S130 != 1
#error This code *only* works with the Nordic S130 softdevice
#endif

#if CONFIG_ENABLED(MICROBIT_DBG)
#error use of the serial port by MICROBIT_DBG clashes with our use of the serial port - not uspported
#endif

MicroBit uBit;

void advertisementCallback(const Gap::AdvertisementCallbackParams_t *params)
{
    const uint8_t len = params->advertisingDataLen;
    if (len == 27)
    {
        // check iBeacon header
        if (
            params->advertisingData[0] == 0x1a &&
            params->advertisingData[1] == 0xff &&
            params->advertisingData[2] == 0x4c &&
            params->advertisingData[3] == 0x00 &&
            params->advertisingData[4] == 0x02 &&
            params->advertisingData[5] == 0x15)
        {
            char uuid[33]{};
            uint8_t unib{};
            uint8_t lnib{};
            for (int i = 0; i < 16; ++i)
            {
                unib = params->advertisingData[6 + i] >> 4;
                lnib = params->advertisingData[6 + i] & 0xf;

                if (unib < 10)
                {
                    uuid[i * 2] = '0' + unib;
                }
                else
                {
                    uuid[i * 2] = 'A' + unib - 10;
                }
                if (lnib < 10)
                {
                    uuid[i * 2 + 1] = '0' + lnib;
                }
                else
                {
                    uuid[i * 2 + 1] = 'A' + lnib - 10;
                }
            }

            uBit.display.scrollAsync(uuid);
        }
    }
    else if (len == 28)
    {
        // AltBeacon https://github.com/AltBeacon/spec
        // check AltBeacon header
        if (
            params->advertisingData[0] == 0x1b &&
            params->advertisingData[1] == 0xff &&
            params->advertisingData[4] == 0xbe &&
            params->advertisingData[5] == 0xac)
        {
            char beaconId[41]{};
            uint8_t unib{};
            uint8_t lnib{};
            for (int i = 0; i < 20; ++i)
            {
                unib = params->advertisingData[6 + i] >> 4;
                lnib = params->advertisingData[6 + i] & 0xf;

                if (unib < 10)
                {
                    beaconId[i * 2] = '0' + unib;
                }
                else
                {
                    beaconId[i * 2] = 'A' + unib - 10;
                }
                if (lnib < 10)
                {
                    beaconId[i * 2 + 1] = '0' + lnib;
                }
                else
                {
                    beaconId[i * 2 + 1] = 'A' + lnib - 10;
                }
            }

            uBit.display.scrollAsync(beaconId);
        }
    }
    else
    {
        if (
            params->advertisingData[0] == 0x03 &&
            params->advertisingData[1] == 0x03 &&
            params->advertisingData[2] == 0xaa &&
            params->advertisingData[3] == 0xfe)
        {
            // Eddystone https://github.com/google/eddystone/blob/master/protocol-specification.md
            if (params->advertisingData[8] == 0x00)
            {
                // Eddystone UID
                // [10bytes]-[6bytes]
                char beaconId[34]{};
                beaconId[20] = '-';
                uint8_t unib{};
                uint8_t lnib{};
                for (int i = 0; i < 10; ++i)
                {
                    unib = params->advertisingData[10 + i] >> 4;
                    lnib = params->advertisingData[10 + i] & 0xf;

                    if (unib < 10)
                    {
                        beaconId[i * 2] = '0' + unib;
                    }
                    else
                    {
                        beaconId[i * 2] = 'A' + unib - 10;
                    }
                    if (lnib < 10)
                    {
                        beaconId[i * 2 + 1] = '0' + lnib;
                    }
                    else
                    {
                        beaconId[i * 2 + 1] = 'A' + lnib - 10;
                    }
                }
                for (int i = 10; i < 16; ++i)
                {
                    unib = params->advertisingData[10 + i] >> 4;
                    lnib = params->advertisingData[10 + i] & 0xf;

                    if (unib < 10)
                    {
                        beaconId[i * 2 + 1] = '0' + unib;
                    }
                    else
                    {
                        beaconId[i * 2 + 1] = 'A' + unib - 10;
                    }
                    if (lnib < 10)
                    {
                        beaconId[i * 2 + 2] = '0' + lnib;
                    }
                    else
                    {
                        beaconId[i * 2 + 2] = 'A' + lnib - 10;
                    }
                }

                uBit.display.scrollAsync(beaconId);
            }
            else if (params->advertisingData[8] == 0x10)
            {
                // Eddystone URL
                uint8_t length = params->advertisingData[4];
                if (length < 6)
                {
                    // invalid data
                    return;
                }
                ManagedString urlStr("");
                switch (params->advertisingData[10])
                {
                case 0x00:
                    urlStr = urlStr + "http://www.";
                    break;
                case 0x01:
                    urlStr = urlStr + "https://www.";
                    break;
                case 0x02:
                    urlStr = urlStr + "http://";
                    break;
                case 0x03:
                    urlStr = urlStr + "https://";
                    break;
                }
                for (int i = 0; i < length - 6; ++i)
                {
                    uint8_t urlChar = params->advertisingData[11 + i];
                    if (urlChar > 0x20 && urlChar < 0x7f)
                    {
                        urlStr = urlStr + ManagedString((const char)urlChar);
                    }
                    else
                    {
                        switch (urlChar)
                        {
                        case 0x00:
                            urlStr = urlStr + ".com/";
                            break;
                        case 0x01:
                            urlStr = urlStr + ".org/";
                            break;
                        case 0x02:
                            urlStr = urlStr + ".edu/";
                            break;
                        case 0x03:
                            urlStr = urlStr + ".net/";
                            break;
                        case 0x04:
                            urlStr = urlStr + ".info/";
                            break;
                        case 0x05:
                            urlStr = urlStr + ".biz/";
                            break;
                        case 0x06:
                            urlStr = urlStr + ".gov/";
                            break;
                        case 0x07:
                            urlStr = urlStr + ".com";
                            break;
                        case 0x08:
                            urlStr = urlStr + ".org";
                            break;
                        case 0x09:
                            urlStr = urlStr + ".edu";
                            break;
                        case 0x0a:
                            urlStr = urlStr + ".net";
                            break;
                        case 0x0b:
                            urlStr = urlStr + ".info";
                            break;
                        case 0x0c:
                            urlStr = urlStr + ".biz";
                            break;
                        case 0x0d:
                            urlStr = urlStr + ".gov";
                            break;
                        }
                    }
                }

                uBit.display.scrollAsync(urlStr);
            }
        }
        else
        {
#if 0
            // The other data
            char data[len]{};
            uint8_t unib{};
            uint8_t lnib{};
            for (int i = 0; i < len; ++i)
            {
                unib = params->advertisingData[i] >> 4;
                lnib = params->advertisingData[i] & 0xf;

                if (unib < 10)
                {
                    data[i * 2] = '0' + unib;
                }
                else
                {
                    data[i * 2] = 'A' + unib - 10;
                }
                if (lnib < 10)
                {
                    data[i * 2 + 1] = '0' + lnib;
                }
                else
                {
                    data[i * 2 + 1] = 'A' + lnib - 10;
                }
            }

            uBit.display.scrollAsync(data);
#endif
        }
    }
}

int main()
{
    uBit.ble = new BLEDevice();
    uBit.ble->init();

    uBit.ble->gap().setScanParams(500, 400);
    uBit.ble->gap().startScan(advertisementCallback);

    while (true)
    {
        uBit.ble->waitForEvent();
    }
    return 0;
}
