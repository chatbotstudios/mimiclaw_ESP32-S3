#pragma once
#include <string.h>

static inline void settings_power_open_dim(void) {}
static inline void settings_power_open_sleep(void) {}
static inline void settings_power_dim_label(char* buf, size_t cap) { if(cap>0) strncpy(buf, "dim", cap-1); }
static inline void settings_power_sleep_label(char* buf, size_t cap) { if(cap>0) strncpy(buf, "sleep", cap-1); }
