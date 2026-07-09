#include "audio_mic.h"
#include "driver/i2s_std.h"
#include "esp_log.h"

static const char *TAG = "MIC";
static i2s_chan_handle_t rx_chan = NULL;

esp_err_t audio_mic_init(void) {
    ESP_LOGI(TAG, "Initializing Microphone I2S Interface");

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_MASTER);
    esp_err_t err = i2s_new_channel(&chan_cfg, NULL, &rx_chan);
    if (err != ESP_OK) return err;

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000), // 16kHz
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = MIC_I2S_MCLK,
            .bclk = MIC_I2S_BCLK,
            .ws = MIC_I2S_WS,
            .dout = I2S_GPIO_UNUSED,
            .din = MIC_I2S_DIN,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    err = i2s_channel_init_std_mode(rx_chan, &std_cfg);
    if (err != ESP_OK) return err;

    err = i2s_channel_enable(rx_chan);
    if (err != ESP_OK) return err;

    ESP_LOGI(TAG, "Microphone initialized successfully");
    return ESP_OK;
}

esp_err_t audio_mic_read(void *buffer, size_t len, size_t *bytes_read, uint32_t timeout_ms) {
    if (!rx_chan) return ESP_ERR_INVALID_STATE;
    return i2s_channel_read(rx_chan, buffer, len, bytes_read, timeout_ms);
}
