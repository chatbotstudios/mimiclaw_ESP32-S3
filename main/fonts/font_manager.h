#pragma once

#include "lvgl.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the font manager.
 */
void font_manager_init(void);

/**
 * Cleanup any dynamically loaded Tiny TTF fonts.
 */
void font_manager_cleanup(void);

/**
 * Gets a highly reliable core font compiled into flash.
 * Size 24 or 48.
 */
const lv_font_t * font_manager_get_core_font_24(void);
const lv_font_t * font_manager_get_core_font_48(void);

/**
 * Gets an emoji/decorative font loaded dynamically from the SD card.
 * If the SD card is missing or the file fails to load, gracefully falls back to the core flash font.
 * Sizes 16, 24, 32, 48, 64 supported for TTF.
 */
const lv_font_t * font_manager_get_emoji_font(int size);

#ifdef __cplusplus
}
#endif
