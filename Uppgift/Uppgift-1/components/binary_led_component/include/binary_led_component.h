#pragma once
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"

/*
Manual state:
* Sub states:
- On
- Off

Blink States
* Sub states:
- On
- Off

Manual -> Blink: db = 1
ManualOn -> ManualOff: d = 0
ManualOff -> ManualOn: d = 1

Blink -> Manual: db = 0
BlinkOn -> BlinkOff: time
BlinkOff -> BlinkOn: time
*/
typedef struct
{
    gpio_mode_t gpio_mode;
    int pin;
    int pull_down;
    int pull_up;
} binary_led_component;

typedef enum
{
    BINARY_ON,
    BINARY_OFF
} manualState;

typedef enum
{
    BLINK_ON,
    BLINK_OFF,
} blink_states;

binary_led_component *binary_led_init(gpio_mode_t gpio_mode, int pin, int pull_down, int pull_up);

// Lägg state machinen här:
// Som sedan kallas hela tiden
void binary_led_update(int pin, int value);

void binary_led_setLed(int pin, int value);

void binary_led_blink(blink_states binary_states, int secOn, int secOff);

void binary_destroy(binary_led_component *binary_led);
