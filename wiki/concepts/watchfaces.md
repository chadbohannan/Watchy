# Watchfaces

A watch face is how users customize Watchy, and this is the [Watchy Firmware](../systems/watchy-firmware.md)'s primary extension point. The base class exposes a virtual `drawWatchFace()` (`Watchy.h:102`) that the sketch overrides to draw the current time — and, optionally, weather, step count, battery, or any other data — to the [Display](../components/display.md). *Verified against the repo at `d1d233c` as of 2026-08.*

The default implementation (`Watchy.cpp:674`) renders the time using the `DSEG7_Classic_Bold_53` 7-segment font with leading zeros, a clean placeholder for library users to replace. The Patterns available to a face include:
- `Watchy::RTC` — read `currentTime` (a `tmElements_t` public field) after a `RTC.read(currentTime)`.
- Weather from `Watchy::getWeatherData()` → `weatherData` (`temperature`, `weatherConditionCode`, `weatherDescription`, `sunrise`/`sunset`), refreshed subject to the 30-minute `weatherIntervalCounter` gate ([WiFi & NTP](../components/wifi-ntp.md)).
- Step count from the [BMA423 Accelerometer](../components/bma423-accelerometer.md) sensor instance in [RTC_DATA_ATTR](../concepts/rtc-data-attr.md).
- Battery via `getBatteryVoltage()` and board version via `getBoardRevision()`.

**Display budget.** Faces must respect the e-paper's cost model. `showWatchFace(bool partialRefresh)` (`Watchy.cpp:665`) calls `asyncPowerOn()` to overlap panel warmup with drawing, then `display.display(partialRefresh)`. A face that wants a fast per-minute update should draw only the changing region and pass `true`; the [Display](../components/display.md) partial-refresh path is much cheaper than a full redraw, which matters for the [Power Budget Analysis](../syntheses/power-budget.md).

Each example in `examples/WatchFaces/` (Basic, 7_SEG, DOS, Mario, Pokemon, StarryHorizon, Tetris, MacPaint) is itself a sketch that subclasses `Watchy` (or the firmware entrypoint) and overrides `drawWatchFace()`. The many compiled `.bin`/`.ino.esp32.bin` artifacts in the example folders are prebuilt firmware images for flashing via [serial](../runbooks/flashing-serial.md) or [BLE OTA](../runbooks/ble-ota-update.md). The `extras/WatchFaces/index.json` maps faces to their rendered previews.

**Envelope and settings are always the same.** Regardless of the face, a sketch defines a global `Watchy watchy(settings)` and a `setup()` that calls `watchy.init()` (see `Basic/Basic.ino`), with a no-op `loop()`. The `watchySettings` blob comes from the folder's `settings.h` ([Board & Config](../components/board-config.md)); only `drawWatchFace()` is overridden.

**Shared drawing patterns, observed across the shipped faces:**
- *Time as raw digits* — faces read the public `currentTime` (`tmElements_t`) and build the display string themselves, rather than relying on the base clock render. `7_SEG` supports 12/24hr via `HOUR_12_24` (`drawTime`, `Watchy_7_SEG.cpp:30`); DOS hand-assembles a `hh:mm` char array (`Watchy_DOS.cpp:3`); Mario, Tetris, and Pokemon index bitmap numeral arrays by `currentTime.Hour/10`, `Hour%10`, `Minute/10`, `Minute%10`.
- *Weather via condition-code ranges* — `7_SEG` maps `weatherConditionCode` onto icon bitmaps with an ordered `>=` chain (thunderstorm ≥200, drizzle ≥300, rain ≥500, etc., `Watchy_7_SEG.cpp:145`), using `getWeatherData().external` to decide whether to show a real icon or a placeholder "chip" glyph.
- *Step counter with midnight reset* — `7_SEG` resets `sensor.resetStepCounter()` when the time rolls past 00:00, then `display.drawBitmap` the steps icon and `sensor.getCounter()`.
- *Status badges* — the `RTC_DATA_ATTR` flags `WIFI_CONFIGURED`/`BLE_CONFIGURED`/`USB_PLUGGED_IN` are read directly to draw Wi-Fi, Bluetooth, and charge glyphs ([RTC_DATA_ATTR persistence](../concepts/rtc-data-attr.md)).
- *Battery gauge from voltage* — `7_SEG` thresholds `getBatteryVoltage()` (>4.0V, >3.6V, >3.2V) into discrete segments ([Board & Config](../components/board-config.md)).
- *Dark/light toggle* — `7_SEG` keys its entire palette off a `DARKMODE` constant (`GxEPD_BLACK`/`GxEPD_WHITE`), showing how to keep a face themeable.

Because faces draw to the shared `Watchy::display` singleton and only ever use `currentTime`, the sensor object, and the persistent flags, they form a clean, read-only contract over the [Watchy Firmware](../systems/watchy-firmware.md) — the same contract an on-call engineer must reason about in the [Power Budget Analysis](../syntheses/power-budget.md).
