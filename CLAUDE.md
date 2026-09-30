# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Watchy is an open-source ESP32-based e-paper watch. This repo is the **Arduino library** (also published for PlatformIO) that provides the firmware framework: RTC, e-paper display driver, accelerometer, BLE OTA, WiFi/NTP, and the menu system that watch faces are built on top of. Distributed via the Arduino Library Manager and PlatformIO registry — `library.json` / `library.properties` define the package metadata and version, and must be bumped together on release.

There is also a second, unrelated subsystem in this repo: an LLM-maintained engineering wiki under `wiki/` (with raw inputs in `sources/` and tooling in `scripts/`). See `wiki/CLAUDE.md` for its own operating contract — it documents *this* codebase's behavior/incidents, it is not part of the firmware.

## Architecture

- `src/Watchy.h` / `src/Watchy.cpp` — the core `Watchy` class. Owns the RTC, display, settings, and the button-driven menu state machine (`guiState`, stored in `RTC_DATA_ATTR` so it survives deep sleep). `drawWatchFace()` and `handleButtonPress()` are `virtual` — this is the extension point for custom watch faces.
- `src/config.h` — all hardware config: GPIO pin maps, button bit-masks, and per-revision `#ifdef` blocks. Board revision is selected via Arduino board-manager defines: `ARDUINO_WATCHY_V10` / `V15` / `V20` (ESP32, DS3231 or PCF8563 RTC) or `ARDUINO_ESP32S3_DEV` (V3 hardware, ESP32-S3, different pinout and its own 32kHz RTC path). Code branches heavily on these defines throughout `src/`.
- `src/Display.h` / `src/Display.cpp` — `WatchyDisplay`, a `GxEPD2_EPD` subclass for the 200x200 SSD1681 e-paper panel (partial-refresh support).
- `src/WatchyRTC.*` / `src/Watchy32KRTC.*` — RTC abstraction; V1/V1.5/V2 use `WatchyRTC` (DS3231 or PCF8563 depending on revision), V3 uses the ESP32-S3's internal 32kHz RTC (`Watchy32KRTC`).
- `src/bma.*`, `src/bma423.*`, `src/bma4.*` — Bosch BMA423 accelerometer driver (step counting, tap/wrist-tilt interrupts).
- `src/BLE.*` — BLE service used for over-the-air firmware updates from the companion app.
- Watch faces are firmware sketches, not part of the library build: each lives in `examples/WatchFaces/<Name>/` as an `.ino` + a `settings.h` (weather/NTP/API-key config, passed into `watchySettings`) + a subclass of `Watchy` overriding `drawWatchFace()`. `settings.h` holds a real (rate-limited) OpenWeatherMap demo API key — don't treat it as a secret to scrub, it's intentionally public in this template.

## Build / CI

There is no local build/test harness in this repo — firmware is compiled via the Arduino IDE/CLI or PlatformIO against real hardware targets, and CI (GitHub Actions, `arduino-test-compile`) is the source of truth for "does it compile":

- `.github/workflows/dev.actions.yml` — on push/PR to `dev`: compiles every example in `examples/WatchFaces/` against board `esp32:esp32:watchy:Revision=v20` (arduino-esp32 core 2.0.17), using the dependency list from `library.json`/`library.properties`.
- `.github/workflows/main.actions.yml` — on `v*.*.*` tag push: builds all example watch faces across `v10`/`v15`/`v20` board revisions and attaches renamed `.bin`s to a GitHub Release.
- `.github/workflows/v3.actions.yml` — on `v*.*.*` tag push: same release flow for the V3 (`esp32s3`) board revision.

When adding an external Arduino library dependency, update it in **three** places that must stay in sync: `library.json`, `library.properties`, and the `required-libraries` list in each workflow file.

To flash default WiFi credentials, copy `src/wifi_secrets.h.example` to `src/wifi_secrets.h` (gitignored). `Watchy.cpp` picks it up via `__has_include`, and it overrides portal-saved credentials; without it, `connectWiFi()` uses upstream `WiFi.begin()`. Locally built `.bin`s then embed the credentials — don't commit them.

`src/format.sh` runs `clang-format` (LLVM style) over `src/` in place — the file's shebang/args are malformed (wrapped oddly), so run it as `bash -c 'clang-format --style="{BasedOnStyle: llvm, AlignConsecutiveMacros: true, AlignConsecutiveAssignments: true}" -i src/*'` rather than executing it directly, or fix the script first.

## Adding a new watch face

Copy an existing folder under `examples/WatchFaces/` as a starting point (e.g. `Basic` for a minimal one). Each needs: `<Name>.ino` (instantiates `Watchy` with a `watchySettings` struct and calls `.init()`), `settings.h`, and typically a `Watchy<Name>.h`/`.cpp` pair subclassing `Watchy` and overriding `drawWatchFace()`. Committed `.bin` files alongside sources are prebuilt release artifacts, not build output to regenerate by hand. `extras/WatchFaces/index.json` plus the matching PNG catalogs the face for the docs site — update it when adding a face there.

## Branching

Contributions target the `dev` branch, not `master`/`main` (see `CONTRIBUTING.md`); `dev` is what CI builds on every push/PR.
