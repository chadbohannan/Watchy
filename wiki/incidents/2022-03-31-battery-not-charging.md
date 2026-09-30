# Battery Readback Wrong: "Battery does not appear to be charging"

A reported battery charging/readback fault where "About Watchy" showed `Batt: 0.28V` after being plugged in overnight, from [GitHub issue #153](https://github.com/sqfmi/Watchy/issues/153) (open, 15 comments). This is a firmware/hardware readback issue rather than a display bug: the reported voltage is far below the battery's real value.

Two distinct root causes emerged in the thread, and they are easy to conflate:

1. **Wrong ADC pin selected at compile time.** Several reporters traced it to the wrong `BATT_ADC_PIN` for their board revision, which is set in `config.h` — `ARDUINO_WATCHY_V20` uses ADC 34, while V15 uses 35 and V10 uses 33, and the same revision mismatch also breaks the up-button wiring (`UP_BTN_PIN`). Setting the correct `ARDUINO_WATCHY_V20` (or matching) define fixed pin mapping. This overlaps with the [menu-scrolling pin-mismatch incident](../incidents/2022-07-15-menu-scrolling-up.md), which is the same class of board-revision confusion.

2. **A genuine readback-calibration issue.** One reporter measured 4.1V on the battery with a multimeter while the firmware reported ~0.30V regardless of whether they read ADC 33/34/35, so pin selection alone did not explain their case. The [battery sensing](../components/board-config.md) math multiplies the ADC reading by a divider factor — a fixed 2.0 on legacy boards, and `ADC_VOLTAGE_DIVIDER` (`(360+100)/360`) on V3 — and this reporter's readings suggested the divider/ADC configuration in firmware did not match their hardware.

The charging and readback feature depends on the battery chemistry and the charge-status monitoring on [Board & Config](../components/board-config.md). When triaging, first confirm the reported voltage against a multimeter, then check that the compiled `ARDUINO_WATCHY_V*` matches the physical board. This is a recurring, high-impact UX issue (multiple users requested replacements before finding it was a pin/define mismatch), so it is worth treating as a canonical [gui-state / config](../concepts/gui-state-machine.md) and [board-config](../components/board-config.md) troubleshooting path.
