#pragma once
#include <stdio.h>
#include <ctype.h>
#include "ui/theme.h"
#include "ui/core/datastore.h"

// Battery value chip for the home top-right slot. Fills buf with "85%", "85%+" (charging),
// "USB" (on USB, no battery) or "--" (unknown), and returns the slot color: accent while
// charging, down when low (<=20%), else ink_dim. caps=false lowercases "USB" for the
// lowercase theme voices (analog, calm); digits/% are unaffected.
static inline lv_color_t batt_chip(char* buf, size_t n, bool caps, const beacon_theme_t* t) {
  finance_rec_t f = ds_get_finance(0); // Slot 0 is NET which ui_bridge hijacked for battery pct
  int pct = (int)f.value;
  bool chg = false; // dummy
  if (pct >= 0) snprintf(buf, n, "%d%%%s", pct, chg ? "+" : "");
  else          snprintf(buf, n, "%s", chg ? "USB" : "--");
  if (!caps) for (char* p = buf; *p; ++p) *p = (char)tolower((unsigned char)*p);
  return chg ? t->accent : (pct >= 0 && pct <= 20 ? t->down : t->ink_dim);
}
