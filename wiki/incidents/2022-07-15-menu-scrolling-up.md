# Menu Constantly Scrolling Up

A firmware UI fault where the menu selection "constantly cycles up" on its own, from [GitHub issue #174](https://github.com/sqfmi/Watchy/issues/174) (closed 2022-10-08, 11 comments). The user checked the upper-right button for a short but found none.

The root cause was a **board-revision pin mismatch**. `config.h` keys the `UP_BTN_PIN`/`UP_BTN_MASK` to the `ARDUINO_WATCHY_V*` compile-time define:
- V10: `UP_BTN_PIN 32`, `BATT_ADC_PIN 33` (DS3231)
- V15: `UP_BTN_PIN 32`, `BATT_ADC_PIN 35` (PCF8563)
- V2.0: `UP_BTN_PIN 35`, `BATT_ADC_PIN 34` (PCF8563)

When no revision is defined, `config.h` silently defaults to `ARDUINO_WATCHY_V20` (with a `#pragma message`). If the physical board is V1.5 — which uses `UP_BTN_PIN 32` — but the firmware compiled as V2.0 (UP on 35), the Up GPIO is left reading an unconnected/floating line that the `ESP_EXT1_WAKEUP`/`digitalRead` path treats as continuously pressed, so `menuIndex` wraps upward on every poll in [handleButtonPress](../concepts/gui-state-machine.md). Defining `ARDUINO_WATCHY_V15` fixed the perpetual scroll for the original reporter.

Resolution requires defining the correct revision before includes (e.g. `#define ARDUINO_WATCHY_V15` in the `.ino`, or the proper board target). This is the same pin-selection mechanism behind the [battery ADC misread incident](../incidents/2022-03-31-battery-not-charging.md) — a shared `config.h` table feeds both `BATT_ADC_PIN` and the button masks, so a revision mismatch can present as either (or both) symptoms. Because Watchy boards are visually indistinguishable by revision, always confirm the actual board before trusting the compile-time default.
