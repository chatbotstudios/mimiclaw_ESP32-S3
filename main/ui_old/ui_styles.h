#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// Beacon-inspired "Dot-Matrix" / "Editorial Index" Theme Tokens

// Colors
#define BEACON_COLOR_BG         lv_color_hex(0x000000)
#define BEACON_COLOR_TEXT_PRM   lv_color_hex(0xFFFFFF)
#define BEACON_COLOR_TEXT_SEC   lv_color_hex(0x888888)
#define BEACON_COLOR_ACCENT     lv_color_hex(0x4285F4) // Gemini Blue
#define BEACON_COLOR_WARN       lv_color_hex(0xF4B400) // Amber
#define BEACON_COLOR_SUCCESS    lv_color_hex(0x0F9D58) // Green
#define BEACON_COLOR_PANEL_BG   lv_color_hex(0x111111)
#define BEACON_COLOR_PANEL_BORDER lv_color_hex(0x333333)

// Padding & Spacing
#define BEACON_PAD_GLOBAL       16
#define BEACON_PAD_PANEL        12
#define BEACON_GAP_LARGE        16
#define BEACON_GAP_SMALL        8

// Common Style Initializers
static inline void beacon_style_panel_init(lv_style_t *style) {
    lv_style_init(style);
    lv_style_set_bg_color(style, BEACON_COLOR_PANEL_BG);
    lv_style_set_bg_opa(style, LV_OPA_COVER);
    lv_style_set_border_color(style, BEACON_COLOR_PANEL_BORDER);
    lv_style_set_border_width(style, 1);
    lv_style_set_radius(style, 12);
    lv_style_set_pad_all(style, BEACON_PAD_PANEL);
}

static inline void beacon_style_text_primary_init(lv_style_t *style) {
    lv_style_init(style);
    lv_style_set_text_color(style, BEACON_COLOR_TEXT_PRM);
}

static inline void beacon_style_text_secondary_init(lv_style_t *style) {
    lv_style_init(style);
    lv_style_set_text_color(style, BEACON_COLOR_TEXT_SEC);
}

#ifdef __cplusplus
}
#endif
