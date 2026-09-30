# GitHub Issue #153 — "Battery does not appear to be charging"

Source: https://github.com/sqfmi/Watchy/issues/153 (open, 15 comments, created 2022-03-31).

## Summary

Battery left plugged in overnight, but "About Watchy" still shows `Batt: 0.28V`. User asks whether a full discharge/recharge cycle is needed to calibrate.

## Comment thread highlights

- A suggestion to try a full discharge/recharge cycle; if it still fails, consider a faulty battery.
- A reporter found the **wrong ADC pin was used** for their build; the up button (top-right) also did not function as it was on the wrong pin, tied to the ADC pin error. Recommended the correct precompiler define (`ARDUINO_WATCHY_V20`), which sets pins correctly for recent builds. Considered a duplicate of issue #151.
- A commenter noted the 7-SEG watchface does not go through the core that uses `ARDUINO_WATCHY_V20`; the Watchy module sniffs pin layouts based on the RTC model.
- Another reporter got a replacement from Mouser with the same behavior; considered a priority bug given instructions say to fully charge before use.
- A reporter found it was **not** a wrong-pin issue: regardless of reading PIN 33/34/35, voltage reported ~0.30V while a multimeter measured 4.1V on the battery. Was adding `Serial.begin(115200)` and reading `analogReadMilliVolts(...)` on ADC pins to investigate.
