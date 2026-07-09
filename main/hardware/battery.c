#include "hardware/battery.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include <math.h>

#include "driver/gpio.h"

static const char *TAG = "battery";

/* Hardware Settings */
#define BATT_ADC_UNIT           ADC_UNIT_1
#define BATT_ADC_CHAN           ADC_CHANNEL_3 // GPIO 4 on ESP32-S3
#define BATT_ADC_ATTEN          ADC_ATTEN_DB_12 // Full scale 0-3.1V (S3)
#define BATT_DIVIDER_RATIO      2.0f            // 100k/100k divider
#define BATT_ADC_RES            4095.0f         // 12-bit
#define BATT_CTRL_PIN           17

/* Voltage Mapping (Standard LiPo) */
#define V_FULL  4.20f
#define V_EMPTY 3.20f

static adc_oneshot_unit_handle_t s_adc_handle = NULL;

esp_err_t battery_init(void) {
    ESP_LOGI(TAG, "Initializing battery sensing on GPIO 4 with Control on GPIO 17");

    /* Initialize Battery Control Pin */
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BATT_CTRL_PIN),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);
    gpio_set_level(BATT_CTRL_PIN, 1); // Enable battery divider

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = BATT_ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &s_adc_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = BATT_ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(s_adc_handle, BATT_ADC_CHAN, &config));

    return ESP_OK;
}

float battery_get_voltage(void) {
    if (!s_adc_handle) return 0.0f;

    int raw = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(s_adc_handle, BATT_ADC_CHAN, &raw));

    /* Convert to voltage: raw * (Vref / resolution) * divider */
    /* On ESP32-S3 with 12dB atten, Vref is approx 3100mV */
    float voltage = (float)raw * (3.1f / BATT_ADC_RES) * BATT_DIVIDER_RATIO;
    
    /* Apply a small calibration offset if needed (typically -0.05 to +0.05) */
    voltage += 0.02f; 

    return voltage;
}

int battery_get_percentage(void) {
    float v = battery_get_voltage();
    
    if (v >= V_FULL) return 100;
    if (v <= V_EMPTY) return 0;

    /* Simple linear mapping (can be improved with a curve lookup) */
    int percentage = (int)((v - V_EMPTY) / (V_FULL - V_EMPTY) * 100.0f);
    
    return percentage;
}

/* Backwards compatibility wrappers */
uint16_t mimi_power_get_battery_voltage_mv(void) {
#ifdef CONFIG_BOARD_AMOLED_175
    // PMIC I2C battery reading can be implemented here later using BSP
    // Mocking for now to avoid linker errors
    return 3700;
#else
    return (uint16_t)(battery_get_voltage() * 1000.0f);
#endif
}

uint8_t mimi_power_get_battery_percent(void) {
#ifdef CONFIG_BOARD_AMOLED_175
    // PMIC I2C battery reading can be implemented here later using BSP
    // Mocking for now
    return 50;
#else
    return (uint8_t)battery_get_percentage();
#endif
}

bool mimi_power_is_charging(void) {
    return false; // Mock
}
