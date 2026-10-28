# Wiki Index

The structured routing layer for the wiki. Each entry links to a page with a one-line description. This file is updated on every ingest.

## Systems

- [Watchy Firmware](systems/watchy-firmware.md) — the ESP32 e-paper watch runtime: boot/reset path, wake reasons, and the main loop

## Incidents

- [2022-02-19: Serial Upload Timeout](incidents/2022-02-19-serial-flash-timeout.md) — `esptool` "Failed to connect to ESP32"; environmental flash-path failure (38-comment issue)
- [2022-03-31: Battery Readback Wrong](incidents/2022-03-31-battery-not-charging.md) — `Batt: 0.28V`; wrong `BATT_ADC_PIN` / board-revision define vs. divider mismatch
- [2022-07-15: Menu Constantly Scrolling Up](incidents/2022-07-15-menu-scrolling-up.md) — perpetual menu scroll from board-revision pin mismatch (`UP_BTN_PIN` default V20)
- [2023-04-18: Compile Break on GxEPD2 1.5.2](incidents/2023-04-18-gxepd2-width-visible.md) — `WIDTH_VISIBLE` template error; dependency drift, fixed in v1.4.7
- [2023-10-01: RTC Time Stuck](incidents/2023-10-01-rtc-time-garbled.md) — garbled RTC time (`45:85 2165/25/45`) surviving NTP/set/reflash; open
- [2026-08-14: Wrong Chip Flash Reject](incidents/2026-08-14-wrong-chip-upload-reject.md) — esptool `ESP32 is not ESP32-S3`; compiled for docs' S3 target, board is classic ESP32

## Components

- [Display (WatchyDisplay)](components/display.md) — the GxEPD2-derived 200×200 e-paper driver with fast/partial update support
- [RTC](components/rtc.md) — the real-time clock abstraction (DS3232RTC/PCF8563/Watchy32KRTC)
- [BLE OTA](components/ble-ota.md) — the Bluetooth LE firmware-update service
- [BMA423 Accelerometer](components/bma423-accelerometer.md) — the step/tilt/wakeup sensor and its deep-sleep interrupt wiring
- [WiFi & NTP](components/wifi-ntp.md) — WiFiManager provisioning, weather fetch, and time sync
- [Board & Config](components/board-config.md) — pin maps, settings struct, and per-revision compile-time selection

## Concepts

- [RTC_DATA_ATTR Persistence](concepts/rtc-data-attr.md) — state that survives deep sleep in RTC memory
- [Deep Sleep & Wake Model](concepts/deep-sleep-wake.md) — how the watch sleeps and what wakes it (timer, buttons, USB)
- [GUI State Machine](concepts/gui-state-machine.md) — the menu/watchface/app state transitions
- [Watchfaces](concepts/watchfaces.md) — how example watch faces override `drawWatchFace` and the display budget

## Runbooks

- [Flashing Firmware via Serial](runbooks/flashing-serial.md) — load the Watchy library/Firmware uploader onto the ESP32
- [BLE OTA Firmware Update](runbooks/ble-ota-update.md) — update firmware wirelessly over Bluetooth

## Syntheses

- [Power Budget Analysis](syntheses/power-budget.md) — how deep sleep, partial refresh, and radio-off policy control battery life
- [Set Time + NTP Integration Plan](syntheses/set-time-ntp-integration-plan.md) — Set Time list view with Use NTP checkbox, Sync, and a DST-aware timezone picker (four seasonal city tables); implemented, hardware-untested
