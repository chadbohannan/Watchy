# Flashing Firmware via Serial

Load the Watchy library or a prebuilt firmware image onto the ESP32 over USB serial. This is the general-purpose loading path and the fallback when [BLE OTA](../runbooks/ble-ota-update.md) is unavailable. The procedures below are drawn from the official [getting-started documentation](../../sources/2026-08-14-getting-started.md).

1. **Install the toolchain.** Install the Arduino IDE and the ESP32 Arduino core 2.0.5+. Add the Espressif board-manager URL under File > Preferences > Additional Board Manager URLs:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` — then in Boards Manager install the latest **esp32** by Espressif (not "Arduino ESP32 Boards").
2. **Open a sketch.** For a custom watchface, open the example in `examples/WatchFaces/<face>/` (e.g. `7_SEG.ino`); for the base library, open a minimal sketch that includes `Watchy.h`. Each WatchFaces folder carries a `settings.h` where you set WiFi/weather/NTP values ([WiFi & NTP](../components/wifi-ntp.md)).
3. **Enter bootloader mode.** Plug in USB, then press and hold the top two buttons (Back & Up) for more than 4 seconds and release **Back first**, before releasing Up. An ESP32S3 device should enumerate a serial port (COM / `/dev/cu.*`). If no port appears, confirm you are using a data (not charge-only) USB cable and try other ports.
4. **Select the board and upload.** Set Tools > Board > ESP32 Arduino > **ESP32S3 Dev Module**, Flash Size > **8MB (64Mb)**, and Partition Scheme > **8M with spiffs...**; leave the rest default. Choose an example and upload. The Arduino build produces a merged firmware image (the `*.bin` / `*.ino.esp32.bin` files in the example folders are prebuilt results of this step).
5. **Reset to run the new firmware.** After upload, reset Watchy. To reset: hold the top two buttons (Back & Up) for more than 4 seconds and release **Up first**, before releasing Back; wait a few seconds for it to boot and refresh the screen.

**Troubleshooting a failed flash.** A `Timed out waiting for packet header` from `esptool` means the flash never reached the ROM bootloader — this is the most-commented Watchy issue and is environmental, not a firmware bug (see the [serial flash timeout incident](../incidents/2022-02-19-serial-flash-timeout.md)). Re-enter bootloader mode, switch to a known-good data cable, and re-select the serial port.

A distinctly different failure is esptool rejecting the flash on a *chip* mismatch: `A fatal error occurred: This chip is ESP32, not ESP32-S3. Wrong chip argument?`. This happens when the firmware was compiled for the wrong CPU — in practice, when the S3 FQBN from the docs is used against a classic-ESP32 (V1/V2) Watchy. Rebuild for the actual board (`esp32:esp32:esp32`) and re-upload; see the [wrong-chip upload rejection incident](../incidents/2026-08-14-wrong-chip-upload-reject.md). The rule: confirm the connected chip before compiling, because the arch, linker script, and flash layout differ between the classic ESP32 and the ESP32-S3, so the S3 `.bin` cannot simply be repointed at the device.

**CLI flashing (tested on a classic-ESP32 Watchy).** The same flow runs entirely from `arduino-cli`:

```sh
arduino-cli compile --fqbn "esp32:esp32:esp32" ./examples/WatchFaces/7_SEG
arduino-cli upload -p /dev/ttyUSB0 --fqbn "esp32:esp32:esp32" ./examples/WatchFaces/7_SEG
```

The classic ESP32 uses the default 4 MB partition; only the newer V3 (ESP32-S3) board needs the `FlashSize=8M,PartitionScheme=default_8MB` options from step 4. A first (cold) compile of a face is a long build that spends most of its time in the linker step (an `ld` process running near 100% CPU) — this is normal, not a hang. Upload writes and verifies four images (bootloader at `0x1000`, partitions at `0x8000`, `boot_app0` at `0xe000`, app at `0x10000`) before hard-resetting via the RTS pin. A trailing `[stderr] /bin/sh: 1: Bad substitution` line from arduino-cli is harmless tooling noise and does not indicate a failed upload. The 7_SEG face builds to ~1,298,052 bytes, about 99% of the 1,310,720-byte program region, so the classic-ESP32 build leaves almost no flash headroom for later edits that grow the sketch; the prebuilt S3 image in the example folder was built for the V3 and should not be flashed to the classic board.

After first flash, time/weather can be provisioned through the menu — see the [getting-started source](../../sources/2026-08-14-getting-started.md) and the watch's own [WiFi provisioning](../components/wifi-ntp.md). For firmware drift across the library, the version macros reported by `config.h` (`WATCHY_LIB_VER`) are what you validate against.

The provided scripts in `scripts/` do not flash a board themselves (they are wiki tooling); flashing always goes through the Arduino toolchain. Use this route whenever a serial upload is needed and reference [BLE OTA Update](../runbooks/ble-ota-update.md) for the wireless alternative.
