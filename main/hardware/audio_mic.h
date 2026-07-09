#pragma once

#include "esp_err.h"
#include <stddef.h>

// Assuming standard I2S pins for ES7210
#define MIC_I2S_BCLK 41
#define MIC_I2S_WS   42
#define MIC_I2S_DIN  45
#define MIC_I2S_MCLK 46

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the I2S microphone interface
 */
esp_err_t audio_mic_init(void);

/**
 * @brief Read audio data from the microphones
 * 
 * @param buffer Pointer to the destination buffer
 * @param len Number of bytes to read
 * @param bytes_read Number of bytes actually read
 * @param timeout_ms Timeout in milliseconds
 * @return esp_err_t 
 */
esp_err_t audio_mic_read(void *buffer, size_t len, size_t *bytes_read, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif
