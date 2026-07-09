#pragma once

#include "esp_err.h"

// Define QSPI Pins for Waveshare AMOLED 1.75
#define AMOLED_PIN_CS   12
#define AMOLED_PIN_SCK  38
#define AMOLED_PIN_MOSI 4
#define AMOLED_PIN_MISO 5
#define AMOLED_PIN_D2   6
#define AMOLED_PIN_D3   7
#define AMOLED_PIN_RST  39
#define AMOLED_PIN_TE   16

#define AMOLED_WIDTH  466
#define AMOLED_HEIGHT 466

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t amoled_co5300_init(void);

#ifdef __cplusplus
}
#endif
