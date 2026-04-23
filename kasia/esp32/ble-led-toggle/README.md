# ble-led-toggle

Minimal Kasia BLE example for ESP32 DevKit v1. Advertises a GATT service that
the [Kasia app](https://github.com/BromleySat/kasia-app) can connect to and
toggle GPIOs on.

## What it does

1. Starts the Kasia BLE server with device name `KasiaTestDevice`.
2. Advertises device type `KASIA_DEVICE_TYPE_ESP32_DEVKIT_V1` so the app
   renders the correct pin layout.
3. Accepts one-byte writes on the Kasia characteristic:
   - bit7 = target state (0 = LOW, 1 = HIGH)
   - bits 0-6 = GPIO number
4. Writes the pin via `pinMode` + `digitalWrite`. The onboard LED is GPIO 2.

## Build & flash

From this directory:

```bash
pio run -t upload
pio device monitor -b 115200
```

Expected serial output:

```
Kasia Device is active :)
```

## Connect with the Kasia app

1. Launch the Kasia app (`C:\dev\kasia-app\KasiaApp`).
2. Tap the centre crosshair to start scanning.
3. The device `KasiaTestDevice` should appear, the app connects, and the
   LED toggle / D32 toggle should drive the ESP32 pins.

## Library pin

While the BLE rewrite is unpublished, this example consumes the local WIP
library via `lib_deps = symlink://../../../../bromleysat-kasia-wip`. Before
releasing this example, switch to the published registry version:

```ini
lib_deps = bromleysat/Kasia@^1.1.0
```
