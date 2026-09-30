# 2026-08-14: esptool Rejects Flash — "This chip is ESP32, not ESP32-S3"

First-ever board flash of a prebuilt example. The upload was rejected by esptool because the firmware was built for the ESP32-S3 (Watchy V3) while the connected board is a classic ESP32. Cause and duration were both trivial — seconds — but the failure exposed a real documentation-vs-hardware divergence that a first-time builder is guaranteed to hit. The wiki's existing [serial flash timeout incident](../incidents/2022-02-19-serial-flash-timeout.md) covers an environmental flash-path failure; this one is the *mirror* case, a correctly-plumbed serial path rejected on a board/arch mismatch.

## Timeline

- The firmware was compiled for `esp32:esp32:esp32s3` — the FQBN the official [getting-started guide](../../sources/2026-08-14-getting-started.md) prescribes (ESP32S3 Dev Module, 8 MB flash, 8M-with-spiffs partition), which targets the newer Watchy V3 board.
- `arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32s3:... ./examples/WatchFaces/7_SEG` connected on the CP2102N UART but failed before writing anything: `A fatal error occurred: This chip is ESP32, not ESP32-S3. Wrong chip argument?`
- Recompiled and re-uploaded against the generic classic board `esp32:esp32:esp32` (with the default 4 MB partition). This succeeded — the [flashing runbook](../runbooks/flashing-serial.md) records the exact form.

## Root cause

The official docs and the library's `config.h` pragma warnings target the ESP32-S3 (V3) board, and the toolchain probing had inferred an S3 from that documentation rather than from the live silicon. The connected Watchy is a classic ESP32 (ESP32-PICO D4, 4 MB flash). esptool refuses to flash an image whose chip family (`--chip esp32s3`) contradicts the chip it reads over the serial ROM bootloader — a hard, unambiguous rejection rather than a bricking.

## Detection

esptool itself reported the mismatch verbatim at connect time. No data corruption or failed verify occurred; the flash was rejected before the first byte.

## Resolution

Select the board from the live device, not from the docs: rebuild with `--fqbn esp32:esp32:esp32`. Because a classic ESP32 uses a different arch, linker script, and flash layout than the S3, the whole sketch must be recompiled for the target — you cannot reuse the S3 build's `.bin`. The watch then took the firmware and this is the path to use for this hardware now.

## Contributing factors

- The getting-started guide and Watchy V3 (ESP32-S3) are the default target of the official material, so documentation-based inference points at the S3 rather than the classic ESP32 silently.
- The library emits `#pragma message: Hardware revision is not defined at the project level... Defaulting to ARDUINO_WATCHY_V20` during compilation, but these are build-time notes, easy to miss, and identify the *board revision*, not the CPU family.
- This threads into the broader [board-revision pin-mismatch](../incidents/2022-07-15-menu-scrolling-up.md) theme: multiple failures on this hardware come from treating the board identity as a default rather than a fact to be confirmed. See [Board & Config](../components/board-config.md) for how revision selection is meant to work.

## Patterns

Divergence between documented target and live hardware, resolved only by asking the device itself, recurs across this hardware line ([menu scrolling](../incidents/2022-07-15-menu-scrolling-up.md), [battery readback](../incidents/2022-03-31-battery-not-charging.md)). The lesson to reuse: when a [runbook](../runbooks/flashing-serial.md) describes a board target, confirm the actual chip before compiling rather than trusting the doc's board selection.
