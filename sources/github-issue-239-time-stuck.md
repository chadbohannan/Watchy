# GitHub Issue #239 — "Time stuck to 45:85 2165/25/45"

Source: https://github.com/sqfmi/Watchy/issues/239 (open, 8 comments, created 2023-10-01).

## Summary

Watchy is stuck showing `45:85 2165/25/45` — an impossible time (minute 85, month 25, year 2165) that persists even after NTP sync, firmware reset/reflash, or manually setting the time. User asks whether the RTC is at fault and what else to try.

## Notes

- The displayed values are byte-garbage for a time-of-day: minutes > 59 and months > 12 are impossible, suggesting the RTC registers hold out-of-range/binary-coded-decade values or the time wasn't written cleanly.
- The user linked an external screenshot/log and was directed by a vendor to come to GitHub first.
- The issue remained **open** as of capture; no confirmed root cause or fix is recorded in the issue body.

## Relevance

This is a candidate incident pattern for the RTC component: a stuck/garbled time that survives NTP sync and manual set points at the RTC (external chip or time keeping), not the display or the network path. When investigating, compare against the RTC wiki page for the DS3231 vs PCF8563 vs V3 `Watchy32KRTC` write/read paths and weekday normalization.
