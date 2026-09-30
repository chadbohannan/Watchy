# Battery Life — Watchy (official docs)

Source: https://watchy.sqfmi.com/docs/battery-life (fetched 2026-08-14, Docusaurus site, copyright Squarofumi).

## Summary

Watchy wakes up every 60 seconds to check the accelerometer, real-time clock, and update the E-Paper display. Certain watch faces also turn on WiFi to fetch weather data.

- With only time keeping: battery life **5–7 days**.
- With fetching data over WiFi: **2–3 days**.
- Extensible through further optimizations (sleep during off hours, waking only on motion/tilt, etc.).

## Tips

- **Deep sleep when idle.** The ESP32 is powerful and power-hungry; idling drains the battery and creates heat. This is already done in `init()` — no need to call explicitly unless working outside the Watchy class.

```cpp
void Watchy::deepSleep(){
    esp_sleep_enable_ext0_wakeup(RTC_PIN, 0);   // enable deep sleep wake on RTC interrupt
    esp_sleep_enable_ext1_wakeup(BTN_PIN_MASK, ESP_EXT1_WAKEUP_ANY_HIGH); // wake on button press
    esp_deep_sleep_start();
}
```

- **Turn off the radio** after using WiFi/Bluetooth communications:

```cpp
WiFi.mode(WIFI_OFF);
btStop();
```

- **Put the display to sleep** after updating: `display.hibernate()` (already done for you).
- If the face **does not require a step counter**, skip initializing/reading the BMA423 accelerometer.
- If the face **does not require 60-second updates** (e.g. a word clock), change the RTC alarm trigger to a longer period.
