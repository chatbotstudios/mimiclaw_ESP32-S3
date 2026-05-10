#include "hardware/led.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hardware/pm_system.h"

static const char *TAG = "led";

#define LED_PIN_RED 3
#define LED_PIN_GREEN 1

static TaskHandle_t s_led_task_handle = NULL;
static bool s_is_processing = false;
static bool s_red_state = false;
static bool s_green_state = false;

static void led_task(void *arg) {
  while (1) {
    if (s_is_processing) {
      s_red_state = !s_red_state;
      gpio_set_level(LED_PIN_RED, s_red_state ? 1 : 0);

      /* Determine blink rate based on power mode */
      int delay_ms = 600; // Balanced default
      mimi_pwr_mode_t mode = pm_system_get_mode();
      if (mode == MIMI_PWR_PERFORMANCE)
        delay_ms = 300; // Performance

      vTaskDelay(pdMS_TO_TICKS(delay_ms));
    } else {
      s_red_state = false;
      gpio_set_level(LED_PIN_RED, 0);
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
  }
}

esp_err_t led_init(void) {
  gpio_config_t io_conf = {
      .pin_bit_mask = (1ULL << LED_PIN_RED) | (1ULL << LED_PIN_GREEN),
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = 0,
      .pull_down_en = 0,
      .intr_type = GPIO_INTR_DISABLE,
  };
  esp_err_t err = gpio_config(&io_conf);
  if (err != ESP_OK)
    return err;

  gpio_set_level(LED_PIN_RED, 0);
  gpio_set_level(LED_PIN_GREEN, 0); // Turn off Green by default

  xTaskCreate(led_task, "led_task", 2048, NULL, 5, &s_led_task_handle);
  ESP_LOGI(TAG, "LEDs initialized: Red=GPIO %d, Green=GPIO %d", LED_PIN_RED, LED_PIN_GREEN);
  return ESP_OK;
}

void led_set_level(mimi_led_color_t color, int level) {
  if (color == MIMI_LED_RED) {
    s_red_state = level ? true : false;
    gpio_set_level(LED_PIN_RED, s_red_state ? 1 : 0);
  } else {
    s_green_state = level ? true : false;
    gpio_set_level(LED_PIN_GREEN, s_green_state ? 1 : 0);
  }
}

bool led_get_state(mimi_led_color_t color) { 
  return (color == MIMI_LED_RED) ? s_red_state : s_green_state; 
}

void led_start_processing(void) {
  s_is_processing = true;
  if (s_led_task_handle) {
    xTaskNotifyGive(s_led_task_handle);
  }
}

void led_stop_processing(void) {
  s_is_processing = false;
  gpio_set_level(LED_PIN_RED, 0);
}

void led_blink(mimi_led_color_t color, int ms) {
  int pin = (color == MIMI_LED_RED) ? LED_PIN_RED : LED_PIN_GREEN;
  gpio_set_level(pin, 1);
  vTaskDelay(pdMS_TO_TICKS(ms));
  gpio_set_level(pin, 0);
}
