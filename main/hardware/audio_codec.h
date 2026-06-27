#ifndef MIMI_AUDIO_CODEC_H
#define MIMI_AUDIO_CODEC_H

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * Initialize the ES8311 Audio Codec via I2C.
 * This sets up the internal registers for playback.
 */
esp_err_t audio_codec_init(void);

/**
 * Set the speaker volume.
 * @param volume 0-100
 */
esp_err_t audio_codec_set_volume(uint8_t volume);

/**
 * Mute or unmute the audio output.
 */
esp_err_t audio_codec_set_mute(bool mute);

/**
 * Put the codec into low-power standby mode.
 */
esp_err_t audio_codec_standby(bool standby);

#endif // MIMI_AUDIO_CODEC_H
