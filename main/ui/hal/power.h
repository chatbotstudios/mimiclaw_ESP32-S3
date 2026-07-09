#pragma once
static inline void power_deep_sleep(void) {}
static inline void power_off(void) {}
static inline int power_battery_pct(void) { return 100; }
static inline bool power_charging(void) { return false; }
