/**
  * Custom Compile time configuration options for the micro:bit runtime.
  */
  
#ifndef MICROBIT_CUSTOM_CONFIG_H
#define MICROBIT_CUSTOM_CONFIG_H

//
// We don't want to use any of the micro:bit BLE services, so we disable them

#define MICROBIT_BLE_ENABLED        0
#define MICROBIT_BLE_BLUEZONE       0 // We don't need this either

// But we are going to bring up the S130 soft-device.  The DAL assumes that
// if MICROBIT_BLE_ENABLED is not set then the memory from the softdevice
// can be re-used as extra heap, but we don't want this, so we set the memory
// limits explicity

// The lowest address of memory that is safe to use as heap storage when BLE is ENABLED
// Used to define the base of the heap when MICROBIT_HEAP_REUSE_SD is defined.
#define MICROBIT_HEAP_BASE_BLE_ENABLED          0x20002500			// S130 build value
#define MICROBIT_HEAP_BASE_BLE_DISABLED         MICROBIT_HEAP_BASE_BLE_ENABLED  // Force the MicrobitHeapAllocator not to use all the S130 memory

#endif
