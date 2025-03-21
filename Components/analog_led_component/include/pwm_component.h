#pragma once
#include <stdio.h>
#include <math.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "macro.h"
typedef enum
{
    PWM_OFF,
    PWM_ON,
    PWM_MELODY_PLAYING,
    PWM_MELODY_PLAYING_DELAY,
    PWM_FADE_IN,
    PWM_FADE_OUT,
    PWM_SIN_WAVE,
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
    bool note_playing;
    int noteIndex;
    int melody_choice;
    int *warning_melody_arr;
    int *warning_note_durations_arr;
    int warning_melody_arr_size;
} pwm_component_t;

pwm_component_t *pwm_init(int pin, uint32_t freq_hertz, ledc_mode_t speed_mode, ledc_timer_t timer_num, ledc_channel_t channel, uint32_t duty_range, uint32_t fade_duration);
void choose_melody(pwm_component_t *pwm, int choice);
void pwm_update(pwm_component_t *pwm);
void pwm_set_melody(pwm_component_t *pwm, int duty, pwm_state_t state);
void pwm_set(pwm_component_t *pwm, int value);
void pwm_sine(pwm_component_t *pwm, int period);
void pwm_free(pwm_component_t *pwm);
