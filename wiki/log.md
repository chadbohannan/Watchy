# Wiki Log

Chronological, append-only record of wiki operations. One line per operation:

```
[YYYY-MM-DD] operation | Subject — brief description of what happened
```

Operations: `ingest`, `query`, `lint`, `enrich`, `sync`, `update`.

[2026-08-14] update | Wiki bootstrap — seeded index, operating contract, and initial pages for the Watchy firmware
[2026-08-14] enrich | Display/RTC/BLE/BMA423/WiFi-NTP/board-config components + GUI, deep-sleep, rtc-data-attr, watchfaces concepts — code-grounded verification & densification against `d1d233c`
[2026-08-14] enrich | Watchfaces concept — documented shared drawing patterns and per-face catalog from `examples/WatchFaces/`
[2026-08-14] ingest | [getting-started](../sources/2026-08-14-getting-started.md), [battery-life](../sources/2026-08-14-battery-life.md) + issues #147 #153 #174 #215 #239 — created 5 incident pages, enriched serial-flash runbook & power-budget synthesis, updated index
[2026-08-14] update | lint-wiki.py — fixed `../sources/`, `../scripts/` link resolution to check the repo checkout outside wiki/; rebuilt absolute-path link check
[2026-08-14] ingest | First board flash — S3 FQBN rejected by esptool (`ESP32 is not ESP32-S3`); rebuilt for `esp32:esp32:esp32`, uploaded 7_SEG Light. Created wrong-chip incident, enriched flashing-serial runbook w/ CLI workflow & classic-ESP32 notes, updated index
[2026-08-14] enrich | board-config — stated CPU-family split explicitly: V3 = ESP32-S3, V1.0/1.5/2.0 = classic ESP32 (PICO-D4), linked wrong-chip incident

[2026-09-30] ingest | Set Time + NTP integration plan — new synthesis page (list-view UI, sync helper, tz subview, open decisions); proposed, not implemented; verified against `fa00d06`; index updated
[2026-09-30] update | Set Time + NTP implemented on chads_watch — plan page now records design, 4 tz tables, DST tick; updated gui-state-machine, wifi-ntp, board-config, rtc-data-attr, index; compile-checked, not hardware-tested
