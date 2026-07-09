#pragma once
#include <stdint.h>
#include <stdbool.h>

static inline void net_begin(void) {}
static inline bool net_is_connected(void) { return false; }
static inline const char* net_ip(void) { return "0.0.0.0"; }
static inline const char* net_ssid(void) { return ""; }
static inline const char* net_mac(void) { return "00:00:00:00:00:00"; }
static inline void net_status_str(char* buf, size_t cap) { if(cap>0) strncpy(buf, "offline", cap-1); }
