#include "ui_manager.h"
#include "lvgl.h"
#include "esp_log.h"
#ifdef CONFIG_BOARD_AMOLED_175
#include "bsp/esp-bsp.h"
#else
#include "esp_lvgl_port.h"
#endif

static const char *TAG = "ui_manager";

static ui_page_t current_page = PAGE_SPLASH;
lv_obj_t *scr_welcome = NULL;
lv_obj_t *scr_thinking = NULL;
lv_obj_t *scr_dashboard = NULL;

static lv_obj_t* get_screen_obj(ui_page_t page) {
    switch (page) {
        case PAGE_WELCOME: return scr_welcome;
        case PAGE_THINKING: return scr_thinking;
        case PAGE_DASHBOARD: return scr_dashboard;
        default: return NULL;
    }
}

static void screen_gesture_event_cb(lv_event_t * e) {
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if (dir == LV_DIR_LEFT) {
        ui_manager_next_page();
    } else if (dir == LV_DIR_RIGHT) {
        ui_manager_prev_page();
    }
}

static void screen_tap_event_cb(lv_event_t * e) {
    lv_point_t p;
    lv_indev_get_point(lv_indev_get_act(), &p);
    
    // Tap on right side -> next, left side -> prev
    if (p.x > 300) {
        ui_manager_next_page();
    } else if (p.x < 166) {
        ui_manager_prev_page();
    }
}

void ui_manager_init(void) {
    ESP_LOGI(TAG, "Initializing Multi-Page UI");
    
#ifdef CONFIG_BOARD_AMOLED_175
    bsp_display_lock(0);
#endif

    // Create all screens
    ui_welcome_create();
    ui_thinking_create();
    ui_dashboard_create();

    // Attach Swipe and Tap event handlers to all interactive screens
    lv_obj_t *screens[] = {scr_welcome, scr_thinking, scr_dashboard};
    for (int i = 0; i < 3; i++) {
        if (screens[i]) {
            lv_obj_add_flag(screens[i], LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(screens[i], screen_gesture_event_cb, LV_EVENT_GESTURE, NULL);
            lv_obj_add_event_cb(screens[i], screen_tap_event_cb, LV_EVENT_CLICKED, NULL);
        }
    }

#ifdef CONFIG_BOARD_AMOLED_175
    bsp_display_unlock();
#endif

    ui_splash_show();
}

void ui_manager_switch_page(ui_page_t page) {
    if (page >= PAGE_COUNT || page == PAGE_SPLASH) return; // Can't switch back to splash
    
#ifdef CONFIG_BOARD_AMOLED_175
    bsp_display_lock(0);
#endif

    lv_obj_t *target_scr = get_screen_obj(page);
    if (target_scr) {
        lv_scr_load(target_scr);
        current_page = page;
    }

#ifdef CONFIG_BOARD_AMOLED_175
    bsp_display_unlock();
#endif
}

void ui_manager_next_page(void) {
    ui_page_t next = current_page + 1;
    if (next >= PAGE_COUNT) next = PAGE_WELCOME;
    ui_manager_switch_page(next);
}

void ui_manager_prev_page(void) {
    ui_page_t prev = current_page - 1;
    if (prev <= PAGE_SPLASH) prev = PAGE_DASHBOARD; // Loop back to end
    ui_manager_switch_page(prev);
}
