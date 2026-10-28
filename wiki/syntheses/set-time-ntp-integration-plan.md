# Set Time + NTP Integration Plan

Design and implementation record for the Set Time redesign: a "Use NTP" checkbox that locks manual editing, a Sync button that unchecks the box on failure, and a timezone picker whose city labels stay true through DST. Designed with the user 2026-09-30; implemented the same day on branch `chads_watch` against `Watchy`@`fa00d06` (uncommitted). Compile-checked on `esp32:esp32:watchy:Revision=v20` (7_SEG face, +3 KB flash vs. base); **not yet run on hardware**.

## What the watch needs

A face only reads the [RTC](../components/rtc.md) and draws it, so the RTC must hold *local* wall-clock time. The only timezone fact the firmware needs is one integer: the UTC offset in effect now, `gmtOffset` in seconds, which `syncNTP(gmt)` adds to NTP's UTC. Cities, DST rules and tables exist to keep that integer correct through the year; see [WiFi & NTP](../components/wifi-ntp.md) for the sync path.

## Interaction model

Set Time (`setTime()`, `src/Watchy.cpp:535`) is a list view on the same conventions as the main menu in the [GUI state machine](../concepts/gui-state-machine.md): Up/Down move the highlight, Menu activates, Back leaves. Rows are the `SETTIME_*` constants in `src/config.h`. Disabled rows are struck through and skipped by Up/Down.

| Row | Menu button does | Disabled when |
|---|---|---|
| `[x] Use NTP` | Toggles `useNTP`; turning it on syncs immediately | never |
| `Sync` | Runs a sync | `useNTP` off |
| `-05 New York` | Opens the timezone picker | never |
| `Time 14:32` | Opens the hour/minute editor | `useNTP` on |
| `Date 2026/09/30` | Opens the year/month/day editor | `useNTP` on |

A sync draws "Syncing..." first (`connectWiFi()` blocks up to 10 s), then calls `_ntpSyncNow()` (connect, `syncNTP()`, radios off). On failure `useNTP` is cleared, the highlight moves back to the checkbox, and the status line reads "Sync failed"; on success it reads "NTP synced". `showSyncNTP()` remains as public API but is no longer in the main menu, which dropped to `MENU_LENGTH` 5. The list returns to the menu, and the picker to the list, after 60 s without a button; the editors have no idle timeout.

The editors (`_editDateTime(bool editDate)`) reuse the old blinking-digit loop: Up/Down change the field, Menu advances and commits after the last field, Back steps back and cancels from the first field. Time edits write `Second = 0`; date edits keep the current time. The picker (`_pickTimezone()`) shows 7 of 24 rows, starts on the current zone, wraps at both ends, selects with Menu and cancels with Back.

## Timezone model

`src/WatchyTimezone.{h,cpp}` holds a master city list (name, standard-offset hours, DST rule) and four 24-slot tables. The 24 slots are whole-hour effective offsets -11 to +12, so `+00` is the 12th row; -12 is skipped because it has no real city. A slot's label is always true on the day because the table is chosen by date (`tzWindow()`), and a picked city keeps its rule, so its slot moves when DST starts. The persisted state is `tzCitySel` (master-list index, -1 until a zone is picked), `tzDstOn` (whether the rule is currently applied to the RTC) and `useNTP`, all [RTC_DATA_ATTR](../concepts/rtc-data-attr.md) and reset in the `init()` reset branch like `gmtOffset`. Labels are built as `"%+03d %s"`, at most 16 characters against the 18 that fit the 200 px font.

Three rules cover the five regions asked for: **US** (US and Canada; 2nd Sunday March to 1st Sunday November), **EU** (EU, UK; last Sunday March to last Sunday October), **AU** (NSW, Vic, ACT, Tas, SA; 1st Sunday October to 1st Sunday April), plus **none** (Japan, Queensland, Perth, Arizona, Hawaii and others). The EU period always sits inside the US period, which gives only four distinct combinations across the year:

| Table | Applies | US / EU / AU |
|---|---|---|
| A | Jan to 2nd Sun Mar; 1st Sun Nov to Dec | off / off / on |
| B | 2nd to last Sun Mar; last Sun Oct to 1st Sun Nov | on / off / on |
| C | last Sun Mar to 1st Sun Apr; 1st to last Sun Oct | on / on / on |
| D | 1st Sun Apr to 1st Sun Oct | on / on / off |

| Slot | A | B | C | D |
|---|---|---|---|---|
| -11 | Pago Pago | Pago Pago | Pago Pago | Pago Pago |
| -10 | Honolulu | Honolulu | Honolulu | Honolulu |
| -09 | Anchorage | Adak | Adak | Adak |
| -08 | Los Angeles | Anchorage | Anchorage | Anchorage |
| -07 | Denver | Los Angeles | Los Angeles | Los Angeles |
| -06 | Chicago | Denver | Denver | Denver |
| -05 | New York | Chicago | Chicago | Chicago |
| -04 | Caracas | New York | New York | New York |
| -03 | Sao Paulo | Sao Paulo | Sao Paulo | Sao Paulo |
| -02 | Noronha | Noronha | Noronha | Noronha |
| -01 | Cape Verde | Cape Verde | Cape Verde | Cape Verde |
| +00 | UTC/Zulu | UTC/Zulu | UTC/Zulu | UTC/Zulu |
| +01 | Amsterdam | Amsterdam | London | London |
| +02 | Johannesburg | Johannesburg | Amsterdam | Amsterdam |
| +03 | Moscow | Moscow | Moscow | Moscow |
| +04 | Dubai | Dubai | Dubai | Dubai |
| +05 | Karachi | Karachi | Karachi | Karachi |
| +06 | Dhaka | Dhaka | Dhaka | Dhaka |
| +07 | Bangkok | Bangkok | Bangkok | Bangkok |
| +08 | Singapore | Singapore | Singapore | Singapore |
| +09 | Tokyo | Tokyo | Tokyo | Tokyo |
| +10 | Brisbane | Brisbane | Brisbane | Sydney |
| +11 | Sydney | Sydney | Sydney | Noumea |
| +12 | Fiji | Fiji | Fiji | Fiji |

Filler choices: Chicago completes the US chain, London takes `+01` while the UK is on summer time, Brisbane is Sydney's DST-free counterpart, and Adak is the only US zone at `-09` in summer (an obscure name, kept because it is true). Auckland was drafted for `+12` in table D but replaced with Fiji, since New Zealand needs a fifth rule that starts a week before Australia's and would be an hour wrong for that week. Cities displaced from a slot (Phoenix, Mexico City, Lagos, Halifax, and Johannesburg in C and D) cannot be picked on those dates; this is the cost of one city per slot. The rule dates were written from memory and checked against the IANA tz database through 2040.

## DST handling

The RTC stores local time, so both a zone change and a DST flip move the RTC by a whole hour (`_shiftRTC()`). `_applyTimezone()` shifts by `newOffset - gmtOffset` when a city is picked, and needs no WiFi. On every minute tick `_dstTick()` (called from the timer/RTC-alarm wake branch in `init()`, `src/Watchy.cpp:55`) compares the city's rule for today's date with `tzDstOn` and flips when they differ. It does nothing before 03:00 local: flips happen at 03:00 on the transition day, and the guard stops the repeated 02:xx hour after a fall-back from flipping twice. This is day-granular, so the switch is up to a few hours later than the legal instant. The weather fetch respects a picked zone: with `tzCitySel >= 0` it no longer overwrites `gmtOffset` from the API, and syncs NTP only if `useNTP` is on (`src/Watchy.cpp:1016`).

## Verification and known limits

The date logic was checked on a host compiler: every table entry's effective offset equals its slot on every day 2020-2040, and 23,013 DST day-flags matched Python's `zoneinfo`. The firmware compiled on `watchy` v20; nothing has run on a watch. Not tested: button feel (Up/Down hold-to-repeat depends on partial-refresh time), the 03:00 flip on a real transition night, and sync against a real network.

Known limits: a user whose DST-free city loses a contested slot (for example Johannesburg in summer) must pick the DST city at the same offset and then sees an unwanted twice-yearly flip; half-hour zones and New Zealand are out of scope by design; `tzCitySel`, `tzDstOn` and `useNTP` survive deep sleep but not power loss (NVS would be the fix, see [Power Budget Analysis](power-budget.md) for why WiFi is kept manual); the main-menu "Sync NTP" item is gone, so scripts that called `showSyncNTP()` still work but users reach sync only through Set Time.
