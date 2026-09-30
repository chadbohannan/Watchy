# GitHub Issue #147 — "Failed to connect to ESP32"

Source: https://github.com/sqfmi/Watchy/issues/147 (open, 38 comments, created 2022-02-19).

## Summary

User cannot upload sketches to Watchy. `A fatal error occurred: Failed to connect to ESP32: Timed out waiting for packet header`. Worked previously; now consistent.

Full log: warnings that `DS3232RTC` and `Rtc_Pcf8563` libraries claim to run on avr and may be incompatible with esp32; sketch uses 1699093 bytes (86%) of 1966080; esptool.py v3.1; `Connecting........_____....._____...` then timeout. Unit restarts as if reset pin applied.

Environment: Arduino IDE 1.8.19 (Windows 10 Pro), Espressif esp v2.0.2, Arduino_JSON 0.1.0, Adafruit GFX 1.10.13, DS3232RTC 1.3.0, GxEPD2 1.4.5, Watchy 1.3.3, WiFiManager 2.0.9-beta.

Tried: different USB cable/port, disconnect/reconnect battery, flashing without battery, updating outside the (aluminum) case. All same result. NTP/weather, vibration motor, and motion sensor all still work.
