#pragma once
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"

typedef enum
{
    BINARY_OFF,
    BINARY_ON,
    BINARY_BLINK_OFF,
    BINARY_BLINK_ON,
} manualState;

typedef struct
{
    gpio_mode_t gpio_mode;
    int pin;
    int pull_down;
    int pull_up;
    TickType_t stateChangeTime;
    manualState state;
    manualState next_state;
    manualState pre_state;

    uint32_t miliseconds_on;
    uint32_t miliseconds_off;
} binary_led_component;

binary_led_component *binary_led_init(gpio_mode_t gpio_mode, int pin, int pull_down, int pull_up);

void binary_led_update(binary_led_component *led);

void binary_led_setLed(binary_led_component *led, int value);

void binary_led_blink(binary_led_component *led, uint32_t miliseconds_on, uint32_t miliseconds_off);

void binary_destroy(binary_led_component *binary_led);
