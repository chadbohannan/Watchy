# GitHub Issue #174 — "Menu constantly scrolling upwards."

Source: https://github.com/sqfmi/Watchy/issues/174 (closed 2022-10-08, 11 comments, created 2022-07-15).

## Summary

In the menu, the selection constantly cycles up. User checked the upper right hand button for a short but could not detect one.

## Comment thread highlights

- Diagnosis pointed at `config.h`: the `UP_BTN_PIN` definition depends on the watchy revision:
  - `ARDUINO_WATCHY_V10`: `UP_BTN_PIN 32`, `BATT_ADC_PIN 33`, `RTC_TYPE 1` (DS3231)
  - `ARDUINO_WATCHY_V15`: `UP_BTN_PIN 32`, `BATT_ADC_PIN 35`, `RTC_TYPE 2` (PCF8563)
  - `ARDUINO_WATCHY_V20`: `UP_BTN_PIN 35`, `BATT_ADC_PIN 34`, `RTC_TYPE 2` (PCF8563)
  - When no revision is defined, the code defaults to `ARDUINO_WATCHY_V20` and prints a warning.
- **Fix confirmed:** hardcoding `#define ARDUINO_WATCHY_V15` (instead of the default `V20`) resolved the perpetual-scroll for that user. The `#warning` also went away when a revision was defined in the ino before includes.
- Not universally fixed: another user on the same issue still scrolled and could not compile after changing to V10/V15, because compiling with a revision defined triggered other problems.

## Relevance

The menu scroll bug is symptomatic of a **pin-map mismatch** when the installed hardware revision differs from the compile-time default (`ARDUINO_WATCHY_V20`). Defining the actual board revision corrects the `UP_BTN_PIN`/`UP_BTN_MASK` and the ADC read pin together (they share a wiring scheme). See the Board & Config wiki page for the full pin table.
