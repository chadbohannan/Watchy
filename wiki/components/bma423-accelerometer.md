# BMA423 Accelerometer

The watch uses a BMA423 accelerometer (Vendor/Bosch BMA4 family driver vendored under `src/bma.cpp`/`bma423.cpp` and relatives) exposed through the `BMA423` class. It is the source of the watch's always-on gesture features. This component pairs with the [Deep Sleep & Wake Model](../concepts/deep-sleep-wake.md) for how its interrupt wakes the CPU. *Verified against the repo at `d1d233c` as of 2026-08.*

Configuration happens once on a reset wake in `Watchy::_bmaConfig()` (`Watchy.cpp:815`), called from `init()` only in the `default` (reset) case. Because the sensor object lives in [RTC_DATA_ATTR](../concepts/rtc-data-attr.md) memory, its configuration persists across deep sleeps and does not need to be reapplied on every wake. The sequence is:

1. `sensor.begin(_readRegister, _writeRegister, delay)` — the accel is driven over I2C through the static helpers `_readRegister`/`_writeRegister` (`Watchy.cpp:794`/`807`), which are thin `Wire` wrappers. If `begin` returns false, config aborts.
2. A 100 Hz output data rate (`BMA4_OUTPUT_DATA_RATE_100HZ`) with a 2 g range (`BMA4_ACCEL_RANGE_2G`), `BMA4_ACCEL_NORMAL_AVG4` bandwidth, and `BMA4_CONTINUOUS_MODE` performance are applied.
3. The level-triggered, active-high, push-pull interrupt is mapped to `BMA4_INTR1_MAP`, and the axes are remapped (`setRemapAxes`) so gesture detection is oriented correctly; the wrist-raise feature requires the right axis orientation.
4. The step counter, tilt, and wakeup (double-click) features are enabled (`BMA423_STEP_CNTR`, `BMA423_TILT`, `BMA423_WAKEUP`), the step counter is reset, and interrupts for step count, tilt, and wakeup are enabled.

Which wake source actually fires depends on the `ACC_INT_MASK`/`ACC_INT_1_PIN` wiring: the accel's interrupt pin is part of the `BTN_PIN_MASK` ext1 wakeup in `deepSleep()`, so a step, tilt, or double-click can wake the ESP32 as if a button were pressed, and [handleButtonPress](../concepts/gui-state-machine.md) runs.

The "Show Accelerometer" menu app (`showAccelerometer()`, `Watchy.cpp:593`) demonstrates the read surface a face or the firmware uses. It polls `sensor.getAccel(acc)` and `sensor.getDirection()` every 200 ms in a tight loop that stays awake until Back is pressed, printing X/Y/Z registers plus a human-readable orientation — `DIRECTION_DISP_UP`, `DIRECTION_DISP_DOWN`, a screen edge (`DIRECTION_LEFT/RIGHT_EDGE`, `DIRECTION_TOP/BOTTOM_EDGE`), or "ERROR!!!" for an unrecognized direction. Note this app intentionally keeps the CPU awake (no deep sleep in the loop), so it is a short-lived diagnostic rather than the steady-state behavior. Step-count data is available to [watchfaces](../concepts/watchfaces.md) through the same `sensor` object.

A second, independent temperature source lives in the accel (`sensor.readTemperature()`): when WiFi is unavailable, `_getWeatherData()` (`Watchy.cpp:740`) falls back to this to report a local temperature with `weatherConditionCode 800` and `external = false` ([WiFi & NTP](../components/wifi-ntp.md)).

Relevant registration points: `ACC_INT_1_PIN` 14, `ACC_INT_2_PIN` 13 (V3) or 12, `ACC_INT_MASK` `BIT64(14)` — all defined in [Board & Config](../components/board-config.md).
