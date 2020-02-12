# microbit-beacon-finder

The micro:bit finds various of BLE Beacons, and display the ID to LEDs.

## Supported Beacons
* iBeacon
* AltBeacon
* Eddystone-UID
* Eddystone-URL

## How to Build
This project uses yotta to build, not pxt.
This project uses the another SoftDevice(S130). That enables BLE Central feature.

Follow these steps to build the project.
**Don't forget copying NRF51822_S130.ld to NRF51822.ld !**

```bash
# set target to use S130 SoftDevice.
yt target bbc-microbit-classic-gcc-s130

# the linker uses `NRF51822.ld` file, then copy `NRF51822_S130.ld` to `NRF51822.ld`.
cp yotta_targets/bbc-microbit-classic-gcc-s130/ld/NRF51822_S130.ld yotta_targets/bbc-microbit-classic-gcc-s130/ld/NRF51822.ld

# build the project
yt build

# transfer the hex file to micro:bit. (for example, macOS X)
cp build/bbc-microbit-classic-gcc-s130/source/microbit-ble-bridge-combined.hex /Volumes/MICROBIT/microbit-ble-bridge-combined.hex
```
