#ifndef MIMI_AUDIO_CODEC_H
#define MIMI_AUDIO_CODEC_H

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>
#include "esp_codec_dev.h"
#include "driver/i2s_std.h"

/**
 * Initialize the ES8311 Audio Codec using esp_codec_dev.
 */
esp_codec_dev_handle_t audio_codec_init(i2s_chan_handle_t tx_handle);

#endif // MIMI_AUDIO_CODEC_H
