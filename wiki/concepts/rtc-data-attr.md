# RTC_DATA_ATTR Persistence

`RTC_DATA_ATTR` is the key mechanism that lets the Watchy firmware keep state across deep sleeps. Variables declared with this attribute live in the RTC fast memory region of the ESP32, which is retained through `esp_deep_sleep_start()`, unlike normal RAM that is lost. The attribute is central to the [deep sleep & wake model](../concepts/deep-sleep-wake.md)'s design: because the watch sleeps between every action, *all* interactive state must survive in RTC memory. *Verified against the repo at `d1d233c` as of 2026-08.*

The persistent variables are declared as externs in `src/Watchy.h` and defined with `RTC_DATA_ATTR` in `Watchy.cpp:13-25`:

- `guiState` — the current [GUI state machine](../concepts/gui-state-machine.md) state.
- `menuIndex` — the highlighted menu item.
- `sensor` — the **BMA423 accelerometer object**. Storing the driver object itself in RTC memory is significant ([BMA423 Accelerometer](../components/bma423-accelerometer.md)): it means the sensor's configured state survives sleep, so `_bmaConfig()` only runs on a reset wake rather than on every tick.
- `WIFI_CONFIGURED`, `BLE_CONFIGURED` — connection flags.
- `USB_PLUGGED_IN` — persists the USB-detect state read at wake.
- `currentWeather`, `weatherIntervalCounter`, `gmtOffset` — weather cache and refresh rate-limit state ([WiFi & NTP](../components/wifi-ntp.md)); `weatherIntervalCounter` is initialized to `-1` to force a first fetch, and `gmtOffset` carries the timezone learned from a weather call or derived from the city picked in Set Time. Three more fields — `tzCitySel` (picked city index, -1 = none), `tzDstOn` (that city's DST rule is applied to the RTC) and `useNTP` — are reset in the `init()` reset branch with `gmtOffset`; see the [Set Time + NTP integration plan](../syntheses/set-time-ntp-integration-plan.md).
- `alreadyInMenu` — used to make the menu auto-return to the watchface after one tick; initialized `true`.
- `bootTime`, `lastIPAddress`, `lastSSID` — boot timestamp and the last known WiFi identity.

Because these values must survive not only sleep but also a reset (where the `default` case in `init()` reconfigures the RTC and redraws), RTC memory is the connective tissue between the wake handler, the button handler, and the drawing code. Care is needed: RTC memory is limited on the ESP32, and the namespace is shared system-wide, so that is why `sensor` and the weather structs are stored whole rather than reconstructed — reconstructing them would be both slow and state-destroying.
