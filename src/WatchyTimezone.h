#ifndef WATCHY_TIMEZONE_H
#define WATCHY_TIMEZONE_H

#include <stddef.h>
#include <stdint.h>

// Whole-hour timezone picker data. The picker shows TZ_SLOTS effective UTC
// offsets (TZ_SLOT_MIN..TZ_SLOT_MIN+TZ_SLOTS-1); which city is shown in each
// slot depends on the season (tzWindow), so a label is always true on the day.
#define TZ_SLOTS    24
#define TZ_SLOT_MIN -11
#define TZ_WINDOWS  4

enum TzDstRule : uint8_t { TZ_DST_NONE, TZ_DST_US, TZ_DST_EU, TZ_DST_AU };

typedef struct tzCity {
  const char *name;
  int8_t baseHours; // standard-time UTC offset
  uint8_t rule;     // TzDstRule; +1 h while the rule is active
} tzCity;

bool tzDstActive(uint8_t rule, int year, int month, int day);
// 0..3 = tables A..D (see wiki/syntheses/set-time-ntp-integration-plan.md)
uint8_t tzWindow(int year, int month, int day);
const tzCity &tzCityInfo(uint8_t city);
// city index shown in `slot` (0..TZ_SLOTS-1) during `window`
uint8_t tzTableCity(uint8_t window, uint8_t slot);
int8_t tzEffectiveHours(uint8_t city, bool dstOn);
int32_t tzOffsetSecs(uint8_t city, bool dstOn);
// "+01 Amsterdam"
void tzLabel(char *buf, size_t len, int hours, uint8_t city);

#endif
