# BLE OTA

The BLE OTA component (`src/BLE.h`/`src/BLE.cpp`) lets the watch accept firmware updates over Bluetooth Low Energy, layered on the ESP-IDF OTA APIs. It is invoked by the firmware-update flow in [Watchy Firmware](../systems/watchy-firmware.md) and pairs with the [BLE OTA runbook](../runbooks/ble-ota-update.md). *Verified against the repo at `d1d233c` as of 2026-08.*

`BLE::begin()` (called as `BT.begin("Watchy BLE OTA")` in `updateFWBegin()`, `Watchy.cpp:1014`) initializes a BLE server that advertises two services:
- **`SERVICE_UUID_ESPOTA`** (`cd77498e-1ac8-48b6-aba8-4161c7342fce`) — the SQFMI identification service, with a single read-only `CHARACTERISTIC_UUID_ID`. Advertising exposes this UUID so a client can discover the Watchy.
- **`SERVICE_UUID_OTA`** (`86b12865-4b70-4893-8ce6-9864fc00374d`) — the firmware service with three characteristics: a read-only hardware-version characteristic, a read-only watchface-name characteristic (`WATCHFACE_NAME`, set to "Watchy 7 Segment"), and the primary `CHARACTERISTIC_UUID_FW`, which is `NOTIFY | WRITE` and carries the firmware image. All four UUIDs share the space `86b1286x`. A `BLE2902` descriptor (client characteristic configuration) is attached to the OTA characteristic so the client can subscribe to notifications.

Firmware arrives as a stream of 512-byte packets (`FULL_PACKET`). The write callback `otaCallback::onWrite` (`BLE.cpp:40`) drives the OTA state machine:
1. On the first packet since boot, `esp_ota_begin` is called against `esp_ota_get_next_update_partition(NULL)` and status becomes `STATUS_UPDATING` (1).
2. Each payload is written with `esp_ota_write` and `bytesReceived` accumulates. When a packet is shorter than `FULL_PACKET` it signals end-of-file: `esp_ota_end` runs, then `esp_ota_set_boot_partition` marks the new image for the next boot, setting `STATUS_READY` (2) on success.
3. After every write the callback echoes a fixed 5-byte `txData = {1,2,3,4,5}` notification back to the client.

Status is surfaced through `BLE::updateStatus()`, which the update loop in `updateFWBegin()` polls to drive the on-screen progress: `STATUS_CONNECTED` (0), `STATUS_UPDATING` (1), `STATUS_READY` (2), and `STATUS_DISCONNECTED` (4). The loop draws "BLE Connected", a byte-count partial refresh during download, and "Rebooting" before calling `esp_restart()` on READY. On disconnect it turns off radios with `WiFi.mode(WIFI_OFF); btStop()` and returns to the menu.

Note the hardware version characteristic is populated from the `HARDWARE_VERSION_MAJOR/MINOR` and `SOFTWARE_VERSION_MAJOR/MINOR/PATCH` macros in `config.h`, packing five bytes. The `showUpdateFW()` / `updateFWBegin()` calls are currently commented out of the menu in `Watchy.cpp`, but the BLE service code remains fully wired.
