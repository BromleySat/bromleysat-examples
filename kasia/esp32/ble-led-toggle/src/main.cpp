/*
    ble-led-toggle — minimal Kasia BLE example

    Starts a Kasia BLE server on the ESP32. The Kasia app scans for devices
    whose name is prefixed with "Kasia" (the library adds the prefix, so the
    advertised name will be "KasiaMyDevice"), connects, reads the device
    type byte so it knows which pin layout to render, and writes single-byte
    commands to toggle GPIOs.

    Wire format written by the app (one byte):
      bit7  = target state (0 = LOW, 1 = HIGH)
      bit0-6 = GPIO number
    Examples:
      0x02 -> set GPIO 2 LOW   (LED off)
      0x82 -> set GPIO 2 HIGH  (LED on)
      0x20 -> set GPIO 32 LOW
      0xA0 -> set GPIO 32 HIGH

    Expected serial output after boot:
      Kasia Device is active :)
*/

#include <Arduino.h>
#include <Kasia.h>

void setup()
{
    kasia.start(KASIA_DEVICE_TYPE_ESP32_DEVKIT_V1, "MyDevice");
}

void loop()
{
}
