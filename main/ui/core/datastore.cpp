#include "core/datastore.h"
#include <string.h>

static weather_rec_t    s_weather = {0};
static finance_rec_t    s_finance[10] = {0};
static uint8_t          s_finance_count = 0;
static usage_rec_t      s_usage = {0};
static buddy_rec_t      s_buddy = {0};

void datastore_init(void) {
    memset(&s_weather, 0, sizeof(s_weather));
    memset(&s_finance, 0, sizeof(s_finance));
    memset(&s_usage, 0, sizeof(s_usage));
    memset(&s_buddy, 0, sizeof(s_buddy));
    s_finance_count = 2; // Setup 2 dummy finance slots
    strncpy(s_finance[0].id, "NET", 4);
    strncpy(s_finance[1].id, "SYS", 4);
}

void ds_set_weather(const weather_rec_t* r) { s_weather = *r; }
void ds_set_finance(uint8_t idx, const finance_rec_t* r) { if (idx < 10) s_finance[idx] = *r; }
void ds_set_finance_if(uint8_t idx, const char* expect_id, const finance_rec_t* r) { ds_set_finance(idx, r); }
void ds_set_usage(const usage_rec_t* r) { s_usage = *r; }
void ds_set_buddy(const buddy_rec_t* r) { s_buddy = *r; }
void ds_apply_sessions(const buddy_session_t* s, uint8_t count, uint32_t now) {}

void ds_set_state_weather(screen_state_t s, data_err_t e) {}
void ds_set_state_finance(uint8_t idx, screen_state_t s, data_err_t e) {}
void ds_reseed_finance(const char ids[][FIN_ID_LEN], int count) {}
void ds_set_hub_offline(void) {}

weather_rec_t ds_get_weather(void) { return s_weather; }
finance_rec_t ds_get_finance(uint8_t idx) { return s_finance[idx]; }
uint8_t ds_get_finance_count(void) { return s_finance_count; }
usage_rec_t ds_get_usage(void) { return s_usage; }
buddy_rec_t ds_get_buddy(void) { return s_buddy; }

void ds_tick_staleness(uint32_t now) {}
void ds_set_open_pending(const char* id, uint32_t now) {}
void ds_apply_open_ack(const char* id, bool ok, uint32_t now) {}
void ds_tick_open(uint32_t now) {}
void ds_tick_buddy_prompt(uint32_t now) {}
