#pragma once
#include <stdint.h>
#include <stdbool.h>

static inline void nvs_begin(void) {}
static inline bool nvs_get_str(const char* key, char* out, size_t cap) { if(cap>0) out[0]='\0'; return false; }
static inline void nvs_set_str(const char* key, const char* val) {}
static inline bool nvs_get_u8(const char* key, uint8_t* out) { *out = 0; return false; }
static inline void nvs_set_u8(const char* key, uint8_t val) {}
static inline void nvs_commit(void) {}
static inline void nvs_clear(void) {}
static inline void nvs_set_theme(uint8_t idx) {}
static inline uint8_t nvs_get_theme(void) { return 0; }
static inline void nvs_set_brightness(uint8_t b) {}
static inline uint8_t nvs_get_brightness(uint8_t def) { return def; }
