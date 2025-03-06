#pragma once
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#define PIN_GPIO int
#define THRESHOLD int
#define CHANNEL_TYPE adc_channel_t

typedef struct adc_oneshot_unit_ctx_t *adc_oneshot_unit_handle_t;

typedef struct
{
    PIN_GPIO pin;
    int lastValue;
    void (*onThresholdCallback)(int value);
    void (*beforeThresholdCallback)(int value);
    int adc_raw;
    int threshold;
    bool risingEdge;     // Kallar funktionen när den överstiger gränsen.
    bool fallingEdge;    // Kallar funktionen när den är under gränsen.
    bool thresholdState; // Gör så att funktionen kallas en gång.
    adc_oneshot_unit_handle_t handle;
    adc_channel_t adc_channel;
    bool beforeThreshold;
    bool afterThreshold;
} potentiometer;

potentiometer *pot_init(PIN_GPIO pin, CHANNEL_TYPE channel);
void pot_update(potentiometer *pot);
int pot_getValue(potentiometer *pot);
void pot_setOnThreshold(potentiometer *pot, THRESHOLD threshold, bool after, bool before, void (*onThreshold)(), void (*beforeThreshold)());
void pot_destroy(potentiometer *pot);