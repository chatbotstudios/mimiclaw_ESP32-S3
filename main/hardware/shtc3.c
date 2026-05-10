#include "hardware/shtc3.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

static const char *TAG = "shtc3";

#define I2C_MASTER_SCL_IO 48
#define I2C_MASTER_SDA_IO 47
#define I2C_MASTER_NUM 0
#define I2C_MASTER_FREQ_HZ 100000

#define SHTC3_ADDR 0x70
#define SHTC3_CMD_WAKE 0x3517
#define SHTC3_CMD_SLEEP 0xB098
#define SHTC3_CMD_MEASURE 0x7866

i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t dev_handle;
static SemaphoreHandle_t s_shtc3_mutex = NULL;

esp_err_t shtc3_init(void) {
  s_shtc3_mutex = xSemaphoreCreateMutex();
  if (!s_shtc3_mutex) {
    return ESP_ERR_NO_MEM;
  }

  i2c_master_bus_config_t bus_config = {
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .i2c_port = I2C_MASTER_NUM,
      .scl_io_num = I2C_MASTER_SCL_IO,
      .sda_io_num = I2C_MASTER_SDA_IO,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = true,
  };

  esp_err_t err = i2c_new_master_bus(&bus_config, &bus_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to create I2C bus: %s", esp_err_to_name(err));
    return err;
  }

  i2c_device_config_t dev_config = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = SHTC3_ADDR,
      .scl_speed_hz = I2C_MASTER_FREQ_HZ,
  };

  err = i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to add SHTC3 device: %s", esp_err_to_name(err));
    return err;
  }

  /* Wake up */
  uint8_t cmd[2] = {(uint8_t)(SHTC3_CMD_WAKE >> 8),
                    (uint8_t)(SHTC3_CMD_WAKE & 0xFF)};
  i2c_master_transmit(dev_handle, cmd, 2, pdMS_TO_TICKS(100));
  vTaskDelay(pdMS_TO_TICKS(1));

  /* Test Read */
  shtc3_data_t test_data;
  if (shtc3_read(&test_data) == ESP_OK) {
    ESP_LOGI(TAG, "SHTC3 initialized. Test Read: Temp=%.2fC, Hum=%.2f%%",
             test_data.temperature, test_data.humidity);
  } else {
    ESP_LOGW(TAG, "SHTC3 initialized but test read failed.");
  }

  return ESP_OK;
}

esp_err_t shtc3_read(shtc3_data_t *data) {
  if (!data || !s_shtc3_mutex)
    return ESP_ERR_INVALID_ARG;

  if (xSemaphoreTake(s_shtc3_mutex, pdMS_TO_TICKS(500)) != pdTRUE) {
    return ESP_ERR_TIMEOUT;
  }

  /* Wake up */
  uint8_t cmd_wake[2] = {(uint8_t)(SHTC3_CMD_WAKE >> 8),
                         (uint8_t)(SHTC3_CMD_WAKE & 0xFF)};
  i2c_master_transmit(dev_handle, cmd_wake, 2, pdMS_TO_TICKS(100));
  vTaskDelay(pdMS_TO_TICKS(1));

  /* Measure */
  uint8_t cmd_meas[2] = {(uint8_t)(SHTC3_CMD_MEASURE >> 8),
                         (uint8_t)(SHTC3_CMD_MEASURE & 0xFF)};
  esp_err_t err =
      i2c_master_transmit(dev_handle, cmd_meas, 2, pdMS_TO_TICKS(100));
  if (err != ESP_OK) {
    xSemaphoreGive(s_shtc3_mutex);
    return err;
  }

  vTaskDelay(pdMS_TO_TICKS(20));

  /* Read result */
  uint8_t res[6];
  err = i2c_master_receive(dev_handle, res, 6, pdMS_TO_TICKS(200));
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Read failed: %s", esp_err_to_name(err));
    xSemaphoreGive(s_shtc3_mutex);
    return err;
  }

  uint16_t t_raw = (res[0] << 8) | res[1];
  data->temperature = -45 + 175 * ((float)t_raw / 65536.0);
  uint16_t h_raw = (res[3] << 8) | res[4];
  data->humidity = 100 * ((float)h_raw / 65536.0);

  ESP_LOGD(TAG, "Read SHTC3: T=%.2f C, H=%.2f %%", data->temperature,
           data->humidity);

  xSemaphoreGive(s_shtc3_mutex);
  return ESP_OK;
}

esp_err_t shtc3_read_raw(shtc3_raw_data_t *data) {
  if (!data || !s_shtc3_mutex)
    return ESP_ERR_INVALID_ARG;

  if (xSemaphoreTake(s_shtc3_mutex, pdMS_TO_TICKS(500)) != pdTRUE) {
    return ESP_ERR_TIMEOUT;
  }

  /* Wake up */
  uint8_t cmd_wake[2] = {(uint8_t)(SHTC3_CMD_WAKE >> 8),
                         (uint8_t)(SHTC3_CMD_WAKE & 0xFF)};
  i2c_master_transmit(dev_handle, cmd_wake, 2, pdMS_TO_TICKS(100));
  vTaskDelay(pdMS_TO_TICKS(1));

  /* Measure */
  uint8_t cmd_meas[2] = {(uint8_t)(SHTC3_CMD_MEASURE >> 8),
                         (uint8_t)(SHTC3_CMD_MEASURE & 0xFF)};
  esp_err_t err =
      i2c_master_transmit(dev_handle, cmd_meas, 2, pdMS_TO_TICKS(100));
  if (err != ESP_OK) {
    xSemaphoreGive(s_shtc3_mutex);
    return err;
  }

  vTaskDelay(pdMS_TO_TICKS(20));

  /* Read result */
  uint8_t res[6];
  err = i2c_master_receive(dev_handle, res, 6, pdMS_TO_TICKS(200));
  if (err != ESP_OK) {
    xSemaphoreGive(s_shtc3_mutex);
    return err;
  }

  data->temp_raw = (res[0] << 8) | res[1];
  data->hum_raw = (res[3] << 8) | res[4];

  xSemaphoreGive(s_shtc3_mutex);
  return ESP_OK;
}
