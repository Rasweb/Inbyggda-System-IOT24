#pragma once
#include <stdio.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "string.h"
#include "led_strip.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "analog_led_component.h"

typedef enum
{
    MELODY_OFF,
    MELODY_PLAYING,
    MELODY_PLAYING_DELAY,
} melodyState;

// manualState state;
// manualState next_state;
// manualState pre_state;
// uint32_t miliseconds_on;
// uint32_t miliseconds_off;

typedef struct
{
    melodyState state;
    melodyState next_state;
    melodyState pre_state;
    uint32_t miliseconds_between_notes;
    uint32_t miliseconds_note_break;
    int duration;
    TickType_t last_tick;
    bool note_playing;
    int noteIndex;

} melody_component_t;

melody_component_t *melody_init();
void update_melody(melody_component_t *melody, pwm_component_t *buzzer);
void set_melody_state(melody_component_t *melody, melodyState state, int duration);
void play_melody(pwm_component_t *buzzer);
