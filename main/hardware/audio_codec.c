#include "hardware/audio_codec.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "audio_codec";

#define ES8311_ADDR 0x18

/* Register Map (Partial) */
#define REG_RESET       0x00
#define REG_CLK_MGR     0x01
#define REG_SDP_CONFIG  0x03
#define REG_SYSTEM      0x0B
#define REG_VOLUME      0x14
#define REG_ADC         0x10
#define REG_DAC         0x17
#define REG_GPIO        0x4C

extern i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t s_codec_handle = NULL;

static esp_err_t write_reg(uint8_t reg, uint8_t val) {
    uint8_t data[2] = {reg, val};
    return i2c_master_transmit(s_codec_handle, data, 2, 100);
}

esp_err_t audio_codec_init(void) {
    if (!bus_handle) return ESP_ERR_INVALID_STATE;

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ES8311_ADDR,
        .scl_speed_hz = 100000,
    };
    
    esp_err_t err = i2c_master_bus_add_device(bus_handle, &dev_cfg, &s_codec_handle);
    if (err != ESP_OK) return err;

    /* ES8311 Deep Reset & Wakeup */
    write_reg(0x00, 0x80); // Reset
    vTaskDelay(pdMS_TO_TICKS(50));
    write_reg(0x00, 0x00); 
    
    write_reg(0x45, 0x00); // Power Up
    write_reg(0x01, 0x30); // MCLK/BCLK auto
    write_reg(0x02, 0x10); // Digital Power
    
    /* I2S Standard Philips, 16-bit */
    write_reg(0x03, 0x10); 
    write_reg(0x13, 0x10); 
    
    /* Volume & Gain (MAX) */
    write_reg(0x14, 0xFF); // Digital Volume
    write_reg(0x16, 0x24); // Analog Gain +12dB
    write_reg(0x17, 0xFF); // DAC Volume
    
    /* Unmute & Enable Speaker */
    write_reg(0x31, 0x00); // Unmute
    write_reg(0x44, 0x08); // Enable DAC to Output
    
    ESP_LOGI(TAG, "ES8311 Codec Deep Initialized (44.1kHz Ready)");
    return ESP_OK;
}

esp_err_t audio_codec_set_volume(uint8_t volume) {
    /* Map 0-100 to 0-255 register value */
    uint8_t val = (volume * 255) / 100;
    return write_reg(REG_VOLUME, val);
}

esp_err_t audio_codec_set_mute(bool mute) {
    return write_reg(0x31, mute ? 0x60 : 0x00);
}

esp_err_t audio_codec_standby(bool standby) {
    return write_reg(0x02, standby ? 0x00 : 0x10);
}
