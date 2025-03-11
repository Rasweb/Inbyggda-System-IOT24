#define CONFIG_BLINK_LED_STRIP 1
#include <stdio.h>
#include "led_strip.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
typedef struct
{
    uint8_t led_state;
    led_strip_handle_t led_strip;
    gpio_num_t led_pin;
} onboard_rgb_led_t;

onboard_rgb_led_t *rgb_led_init(uint8_t led_state, led_strip_handle_t led_strip, gpio_num_t led_pin);
void rgb_led_configure(onboard_rgb_led_t *led);
void rgb_led_blink(onboard_rgb_led_t *led);
void rgb_led_update(onboard_rgb_led_t *led);