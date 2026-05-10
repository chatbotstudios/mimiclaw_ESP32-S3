#include "hardware/led.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hardware/pm_system.h"
#include "led_strip.h"
#include "mimi_config.h"

static const char *TAG = "led";

#define LED_PIN_RED 3
#define LED_PIN_GREEN 1

static led_strip_handle_t s_led_strip = NULL;
static bool s_red_state = false;
static bool s_green_state = false;
static uint32_t s_current_color = 0;

esp_err_t led_init(void) {
  /* 1. Init Discrete LEDs (Red/Green) */
  gpio_config_t io_conf = {
      .pin_bit_mask = (1ULL << LED_PIN_RED) | (1ULL << LED_PIN_GREEN),
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = 0,
      .pull_down_en = 0,
      .intr_type = GPIO_INTR_DISABLE,
  };
  gpio_config(&io_conf);
  gpio_set_level(LED_PIN_RED, 0);
  gpio_set_level(LED_PIN_GREEN, 0);

  /* 2. Init RGB LED (NeoPixel) - DISABLED to avoid pin conflicts with I2S/I2C
   */
  /*
  led_strip_config_t strip_config = {
      .strip_gpio_num = MIMI_RGB_LED_PIN,
      .max_leds = MIMI_RGB_LED_COUNT,
      .led_model = LED_MODEL_WS2812,
      .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
      .flags.invert_out = false,
  };
  led_strip_rmt_config_t rmt_config = {
      .clk_src = RMT_CLK_SRC_DEFAULT,
      .resolution_hz = 10 * 1000 * 1000, // 10MHz
      .flags.with_dma = false,
  };

  esp_err_t err = led_strip_new_rmt_device(&strip_config, &rmt_config,
  &s_led_strip); if (err != ESP_OK) { ESP_LOGE(TAG, "Failed to init RGB LED
  Strip: %s", esp_err_to_name(err)); } else { led_strip_clear(s_led_strip);
    ESP_LOGI(TAG, "RGB LED initialized on GPIO %d", MIMI_RGB_LED_PIN);
  }
  */

  return ESP_OK;
}

void led_set_rgb(uint8_t r, uint8_t g, uint8_t b) {
  if (s_led_strip) {
    led_strip_set_pixel(s_led_strip, 0, r, g, b);
    led_strip_refresh(s_led_strip);
    s_current_color = (r << 16) | (g << 8) | b;
  }
}

void led_set_color(uint32_t color_hex) {
  uint8_t r = (color_hex >> 16) & 0xFF;
  uint8_t g = (color_hex >> 8) & 0xFF;
  uint8_t b = color_hex & 0xFF;
  led_set_rgb(r, g, b);
}

void led_set_state_color(uint32_t color_hex) {
  /* Map hex states to physical Red/Green LEDs */
  if (color_hex == MIMI_COLOR_ONLINE) {
    led_set_level(MIMI_LED_RED, 0);
    led_set_level(MIMI_LED_GREEN, 1);
  } else if (color_hex == MIMI_COLOR_CONNECTING) {
    led_set_level(MIMI_LED_RED, 1); // Red + Green = Yellow
    led_set_level(MIMI_LED_GREEN, 1);
  } else if (color_hex == 0) { // Off
    led_set_level(MIMI_LED_RED, 0);
    led_set_level(MIMI_LED_GREEN, 0);
  } else {
    /* Thinking, Executing, Error, Offline -> Red */
    led_set_level(MIMI_LED_RED, 1);
    led_set_level(MIMI_LED_GREEN, 0);
  }
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
  led_set_state_color(MIMI_COLOR_THINKING); // Purple
}

void led_stop_processing(void) {
  led_set_state_color(MIMI_COLOR_ONLINE); // Back to Green
}

void led_blink(mimi_led_color_t color, int ms) {
  int pin = (color == MIMI_LED_RED) ? LED_PIN_RED : LED_PIN_GREEN;
  gpio_set_level(pin, 1);
  vTaskDelay(pdMS_TO_TICKS(ms));
  gpio_set_level(pin, 0);
}
