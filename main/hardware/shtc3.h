#pragma once

#include "driver/i2c_master.h"
#include "esp_err.h"

extern i2c_master_bus_handle_t bus_handle;

typedef struct {
  float temperature;
  float humidity;
} shtc3_data_t;

typedef struct {
  uint16_t temp_raw;
  uint16_t hum_raw;
} shtc3_raw_data_t;

/**
 * Initialize the SHTC3 sensor on the I2C bus.
 * SDA=47, SCL=48 (Board Specific)
 */
esp_err_t shtc3_init(void);

/**
 * Read temperature and humidity.
 */
esp_err_t shtc3_read(shtc3_data_t *data);

/**
 * Read raw sensor values.
 */
esp_err_t shtc3_read_raw(shtc3_raw_data_t *data);
