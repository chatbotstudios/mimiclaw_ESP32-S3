#include "hardware/audio_service.h"
#include "esp_log.h"
#include "esp_codec_dev.h"
#include "bsp/esp-bsp.h"

static const char *TAG = "audio_service";

static esp_codec_dev_handle_t s_codec_dev = NULL;
static esp_codec_dev_handle_t s_mic_dev = NULL;

esp_err_t audio_service_init(void) {
    ESP_LOGI(TAG, "Initializing Audio Service via BSP");

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000), // Matched to boot.raw
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = BSP_I2S_MCLK,
            .bclk = BSP_I2S_SCLK,
            .ws = BSP_I2S_LCLK,
            .dout = BSP_I2S_DOUT,
            .din = BSP_I2S_DSIN,
        },
    };

    esp_err_t err = bsp_audio_init(&std_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize BSP Audio");
        return err;
    }

    s_codec_dev = bsp_audio_codec_speaker_init();
    if (!s_codec_dev) {
        ESP_LOGE(TAG, "Failed to initialize BSP Speaker Codec");
        return ESP_FAIL;
    }
    
    s_mic_dev = bsp_audio_codec_microphone_init();
    if (!s_mic_dev) {
        ESP_LOGW(TAG, "Failed to initialize BSP Microphone Codec");
    }

    /* Set default volume */
    esp_codec_dev_set_out_vol(s_codec_dev, 70.0f);

    ESP_LOGI(TAG, "Audio Service initialized (16kHz Mono)");
    return ESP_OK;
}

esp_err_t audio_service_play_buffer(const uint8_t *data, size_t len) {
    if (!s_codec_dev) return ESP_ERR_INVALID_STATE;
    
    esp_codec_dev_sample_info_t fs = {
        .sample_rate = 16000,
        .channel = 1,
        .bits_per_sample = 16,
    };
    
    esp_codec_dev_open(s_codec_dev, &fs);
    esp_codec_dev_write(s_codec_dev, (void*)data, len);
    // Note: in a real streaming scenario we'd open once and write repeatedly.
    // For now we close it here to flush.
    esp_codec_dev_close(s_codec_dev);
    
    return ESP_OK;
}

esp_err_t audio_service_play_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return ESP_FAIL;

    if (!s_codec_dev) {
        fclose(f);
        return ESP_ERR_INVALID_STATE;
    }

    esp_codec_dev_sample_info_t fs = {
        .sample_rate = 16000,
        .channel = 1,
        .bits_per_sample = 16,
    };
    esp_codec_dev_open(s_codec_dev, &fs);

    uint8_t buf[1024];
    while (1) {
        size_t read = fread(buf, 1, sizeof(buf), f);
        if (read == 0) break;
        esp_codec_dev_write(s_codec_dev, buf, read);
    }
    
    esp_codec_dev_close(s_codec_dev);
    fclose(f);
    return ESP_OK;
}

void audio_service_set_volume(int volume) {
    if (s_codec_dev) {
        esp_codec_dev_set_out_vol(s_codec_dev, (float)volume);
    }
}
