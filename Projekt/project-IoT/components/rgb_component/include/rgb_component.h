#define CONFIG_BLINK_LED_STRIP 1
#include <stdio.h>
#include "led_strip.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#define BUFFER_SIZE 10
#define CHECK_INTERVAL_MS 3000

typedef void (*warning_value_callback_t)(void *buzzer);
typedef void (*normal_value_callback)(void *buzzer, int value);

typedef struct
{
    uint8_t led_state;
    led_strip_handle_t led_strip;
    gpio_num_t led_pin;
    int buffer[BUFFER_SIZE];
    int buffer_index;
    int buffer_sum;

} onboard_rgb_led_t;

onboard_rgb_led_t *rgb_led_init(uint8_t led_state, led_strip_handle_t led_strip, gpio_num_t led_pin);
void rgb_led_configure(onboard_rgb_led_t *led);
void rgb_led_set_state(onboard_rgb_led_t *led);
void rgb_led_update_buffer(onboard_rgb_led_t *led, int value);
int rgb_led_buffer_average(onboard_rgb_led_t *led);
