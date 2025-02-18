#pragma once
#include <stdio.h>
#include <math.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"

typedef enum
{
    ANALOG_FADE_IN,
    ANALOG_FADE_OUT,
    ANALOG_SIN_WAVE,
    ANALOG_ON,
    ANALOG_OFF,
} analog_led_state_t;

typedef struct
{
    int pin;
    uint32_t freq_hertz;
    ledc_mode_t speed_mode;
    ledc_timer_t timer_num;
    ledc_channel_t channel;
    uint32_t duty_range;
    analog_led_state_t state;
    TickType_t stateChangeTime;
    uint32_t fade_duration;
} analog_led_component;

analog_led_component *analog_led_init(int pin, uint32_t freq_hertz, ledc_mode_t speed_mode, ledc_timer_t timer_num, ledc_channel_t channel, uint32_t duty_range, uint32_t fade_duration);
void analog_led_update(analog_led_component *analog_led);
void analog_setLed(analog_led_component *analog_led, int value);
void analog_sin();
