#pragma once
#include <stdio.h>
#include <math.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"

typedef enum
{
    ANALOG_OFF,
    ANALOG_ON,
    ANALOG_FADE_IN,
    ANALOG_FADE_OUT,
    ANALOG_SIN_WAVE,
} pwm_state_t;

typedef struct
{
    int pin;
    uint32_t freq_hertz;
    ledc_mode_t speed_mode;
    ledc_timer_t timer_num;
    ledc_channel_t channel;
    uint32_t duty_range;
    pwm_state_t state;
    TickType_t stateChangeTime;
    uint32_t fade_duration;
    pwm_state_t next_state;
    pwm_state_t pre_state;
    int duty;
    float angle;
    int period;
} pwm_component_t;

pwm_component_t *pwm_init(int pin, uint32_t freq_hertz, ledc_mode_t speed_mode, ledc_timer_t timer_num, ledc_channel_t channel, uint32_t duty_range, uint32_t fade_duration);
void pwm_update(pwm_component_t *pwm);
void pwm_set(pwm_component_t *pwm, int value);
void pwm_sine(pwm_component_t *pwm, int period);
void pwm_destroy(pwm_component_t *pwm);