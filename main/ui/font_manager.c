#include "font_manager.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include <sys/stat.h>

static const char *TAG = "font_manager";

// Declare compiled C fonts
LV_FONT_DECLARE(inter_24);
LV_FONT_DECLARE(inter_48);

// Cache for dynamically loaded Tiny TTF fonts
static lv_font_t *emoji_font_16 = NULL;
static lv_font_t *emoji_font_24 = NULL;
static lv_font_t *emoji_font_32 = NULL;
static lv_font_t *emoji_font_48 = NULL;
static lv_font_t *emoji_font_64 = NULL;

void font_manager_init(void) {
    ESP_LOGI(TAG, "Font manager initialized. Core fonts available.");
}

void font_manager_cleanup(void) {
#if LV_USE_TINY_TTF
    if (emoji_font_16) { lv_tiny_ttf_destroy(emoji_font_16); emoji_font_16 = NULL; }
    if (emoji_font_24) { lv_tiny_ttf_destroy(emoji_font_24); emoji_font_24 = NULL; }
    if (emoji_font_32) { lv_tiny_ttf_destroy(emoji_font_32); emoji_font_32 = NULL; }
    if (emoji_font_48) { lv_tiny_ttf_destroy(emoji_font_48); emoji_font_48 = NULL; }
    if (emoji_font_64) { lv_tiny_ttf_destroy(emoji_font_64); emoji_font_64 = NULL; }
    ESP_LOGI(TAG, "Tiny TTF fonts cleaned up.");
#endif
}

const lv_font_t * font_manager_get_core_font_24(void) {
    return &inter_24;
}

const lv_font_t * font_manager_get_core_font_48(void) {
    return &inter_48;
}

const lv_font_t * font_manager_get_emoji_font(int size) {
#if LV_USE_TINY_TTF && LV_TINY_TTF_FILE_SUPPORT
    lv_font_t **cached_font = NULL;
    
    switch (size) {
        case 16: cached_font = &emoji_font_16; break;
        case 24: cached_font = &emoji_font_24; break;
        case 32: cached_font = &emoji_font_32; break;
        case 48: cached_font = &emoji_font_48; break;
        case 64: cached_font = &emoji_font_64; break;
        default: 
            ESP_LOGW(TAG, "Unsupported emoji font size %d, falling back to 24", size);
            cached_font = &emoji_font_24;
            size = 24;
            break;
    }

    if (*cached_font != NULL) {
        return *cached_font; // Return cached
    }

    // Attempt to load from SD card
    const char *ttf_path = "/sdcard/fonts/NotoEmoji-Regular.ttf";
    
    struct stat st;
    if (stat(ttf_path, &st) == 0) {
        // File exists, attempt to load
        lv_font_t *new_font = lv_tiny_ttf_create_file(ttf_path, size);
        if (new_font) {
            ESP_LOGI(TAG, "Successfully loaded Tiny TTF from %s at size %d", ttf_path, size);
            *cached_font = new_font;
            return new_font;
        } else {
            ESP_LOGE(TAG, "Failed to create Tiny TTF font from %s (out of memory?)", ttf_path);
        }
    } else {
        ESP_LOGW(TAG, "TTF file %s not found on SD card.", ttf_path);
    }
#endif

    // Fallbacks
    ESP_LOGW(TAG, "Falling back to core font for size %d", size);
    if (size >= 48) return &inter_48;
    return &inter_24;
}
