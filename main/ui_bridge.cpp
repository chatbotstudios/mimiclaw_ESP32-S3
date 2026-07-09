#include "ui_bridge.h"
#include <string.h>
#include <stdio.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "ui/carousel.h"
#include "ui/theme.h"
#include "ui/styles.h"
#include "ui/core/datastore.h"

#ifdef CONFIG_BOARD_AMOLED_175
#include "bsp/esp-bsp.h"
#endif

static const char *TAG = "UI_BRIDGE";

// Provide the time functions expected by screen.h
uint32_t now_s(void) { return esp_timer_get_time() / 1000000; }
uint32_t uptime_s(void) { return esp_timer_get_time() / 1000000; }

void ui_bridge_init(void) {
    ESP_LOGI(TAG, "Initializing C++ UI Bridge");

#ifdef CONFIG_BOARD_AMOLED_175
    bsp_display_lock(0);
#endif

    datastore_init();
    styles_init();
    carousel_init();

#ifdef CONFIG_BOARD_AMOLED_175
    bsp_display_unlock();
#endif
}

void ui_bridge_update(const char *wifi_ssid, const char *ip_addr,
                      float battery_v, int battery_pct, float temp,
                      float humidity, bool is_bluetooth_on,
                      int pm_mode, const char *uptime_str,
                      bool thinking) {

    uint32_t now = now_s();
    
    // NET / Battery
    finance_rec_t f0 = {0};
    f0.hdr.state = ST_LIVE;
    f0.hdr.last_updated = now;
    strncpy(f0.id, "NET", 4);
    f0.value = battery_pct;
    ds_set_finance(0, &f0);

    // Weather / System info
    weather_rec_t w = {0};
    w.hdr.state = ST_LIVE;
    w.hdr.last_updated = now;
    w.temp_c = temp;
    w.humidity_pct = humidity;
    w.wmo_code = 0;
    ds_set_weather(&w);

    // Buddy / AI State
    buddy_rec_t b = {0};
    b.hdr.state = ST_LIVE;
    b.hdr.last_updated = now;
    b.running = thinking ? 1 : 0;
    if (thinking) {
      b.session_count = 1;
      b.sessions[0].state = 1; // BST_WORKING
      strncpy(b.sessions[0].id, "claude", 7);
      strncpy(b.sessions[0].label, "mimiclaw", 9);
      b.sessions[0].ts = now;
    }
    ds_set_buddy(&b);
}

void ui_bridge_next_page(void) {
#ifdef CONFIG_BOARD_AMOLED_175
    bsp_display_lock(0);
#endif
    
    carousel_scroll_next();

#ifdef CONFIG_BOARD_AMOLED_175
    bsp_display_unlock();
#endif
}
