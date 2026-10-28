#include "WatchyTimezone.h"
#include <stdio.h>

// Master city list. Indices are referenced by TZ_TABLES and persisted in
// Watchy.cpp (tzCitySel), so append only.
static const tzCity TZ_CITIES[] = {
    {"Pago Pago", -11, TZ_DST_NONE},   // 0
    {"Honolulu", -10, TZ_DST_NONE},    // 1
    {"Adak", -10, TZ_DST_US},          // 2
    {"Anchorage", -9, TZ_DST_US},      // 3
    {"Los Angeles", -8, TZ_DST_US},    // 4
    {"Denver", -7, TZ_DST_US},         // 5
    {"Chicago", -6, TZ_DST_US},        // 6
    {"New York", -5, TZ_DST_US},       // 7
    {"Caracas", -4, TZ_DST_NONE},      // 8
    {"Sao Paulo", -3, TZ_DST_NONE},    // 9
    {"Noronha", -2, TZ_DST_NONE},      // 10
    {"Cape Verde", -1, TZ_DST_NONE},   // 11
    {"UTC/Zulu", 0, TZ_DST_NONE},      // 12
    {"Amsterdam", 1, TZ_DST_EU},       // 13
    {"London", 0, TZ_DST_EU},          // 14
    {"Johannesburg", 2, TZ_DST_NONE},  // 15
    {"Moscow", 3, TZ_DST_NONE},        // 16
    {"Dubai", 4, TZ_DST_NONE},         // 17
    {"Karachi", 5, TZ_DST_NONE},       // 18
    {"Dhaka", 6, TZ_DST_NONE},         // 19
    {"Bangkok", 7, TZ_DST_NONE},       // 20
    {"Singapore", 8, TZ_DST_NONE},     // 21
    {"Tokyo", 9, TZ_DST_NONE},         // 22
    {"Brisbane", 10, TZ_DST_NONE},     // 23
    {"Sydney", 10, TZ_DST_AU},         // 24
    {"Noumea", 11, TZ_DST_NONE},       // 25
    {"Fiji", 12, TZ_DST_NONE},         // 26
};

// Tables A..D, slot 0 = -11 ... slot 23 = +12. Every entry's effective offset
// in its window equals its slot.
static const uint8_t TZ_TABLES[TZ_WINDOWS][TZ_SLOTS] = {
    // A: US off, EU off, AU on
    {0, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 18, 19, 20, 21, 22,
     23, 24, 26},
    // B: US on, EU off, AU on
    {0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 15, 16, 17, 18, 19, 20, 21, 22,
     23, 24, 26},
    // C: US on, EU on, AU on
    {0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 14, 13, 16, 17, 18, 19, 20, 21, 22,
     23, 24, 26},
    // D: US on, EU on, AU off
    {0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 14, 13, 16, 17, 18, 19, 20, 21, 22,
     24, 25, 26},
};

// 0 = Sunday
static int dayOfWeek(int y, int m, int d) {
  static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  if (m < 3) {
    y -= 1;
  }
  return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

static int nthSunday(int y, int m, int n) {
  int first = 1 + (7 - dayOfWeek(y, m, 1)) % 7;
  return first + 7 * (n - 1);
}

// only called for 31-day months (March, October)
static int lastSunday31(int y, int m) { return 31 - dayOfWeek(y, m, 31); }

bool tzDstActive(uint8_t rule, int year, int month, int day) {
  int md = month * 100 + day;
  switch (rule) {
  case TZ_DST_US: // 2nd Sun Mar .. 1st Sun Nov
    return md >= 300 + nthSunday(year, 3, 2) &&
           md < 1100 + nthSunday(year, 11, 1);
  case TZ_DST_EU: // last Sun Mar .. last Sun Oct
    return md >= 300 + lastSunday31(year, 3) &&
           md < 1000 + lastSunday31(year, 10);
  case TZ_DST_AU: // 1st Sun Oct .. 1st Sun Apr (southern summer, wraps the year)
    return md >= 1000 + nthSunday(year, 10, 1) ||
           md < 400 + nthSunday(year, 4, 1);
  default:
    return false;
  }
}

uint8_t tzWindow(int year, int month, int day) {
  // The EU period always lies inside the US period, so three checks suffice.
  if (!tzDstActive(TZ_DST_US, year, month, day)) {
    return 0; // A
  }
  if (!tzDstActive(TZ_DST_EU, year, month, day)) {
    return 1; // B
  }
  return tzDstActive(TZ_DST_AU, year, month, day) ? 2 : 3; // C : D
}

const tzCity &tzCityInfo(uint8_t city) { return TZ_CITIES[city]; }

uint8_t tzTableCity(uint8_t window, uint8_t slot) {
  return TZ_TABLES[window][slot];
}

int8_t tzEffectiveHours(uint8_t city, bool dstOn) {
  return TZ_CITIES[city].baseHours + (dstOn ? 1 : 0);
}

int32_t tzOffsetSecs(uint8_t city, bool dstOn) {
  return (int32_t)tzEffectiveHours(city, dstOn) * 3600;
}

void tzLabel(char *buf, size_t len, int hours, uint8_t city) {
  snprintf(buf, len, "%+03d %s", hours, TZ_CITIES[city].name);
}
