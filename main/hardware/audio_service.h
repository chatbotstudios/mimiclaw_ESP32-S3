#ifndef MIMI_AUDIO_SERVICE_H
#define MIMI_AUDIO_SERVICE_H

#include "esp_err.h"
#include <stddef.h>

/**
 * Initialize the I2S peripheral and the Audio Codec.
 */
esp_err_t audio_service_init(void);

/**
 * Play a raw PCM buffer (16-bit, Mono, 16kHz default).
 */
esp_err_t audio_service_play_buffer(const uint8_t *data, size_t len);

/**
 * Play a sound file from SPIFFS (e.g., "/spiffs/audio/boot.raw").
 */
esp_err_t audio_service_play_file(const char *path);

/**
 * Set global system volume (0-100).
 */
void audio_service_set_volume(int volume);

#endif // MIMI_AUDIO_SERVICE_H
