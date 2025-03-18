#pragma once
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#define PIN_GPIO int
#define THRESHOLD int
#define CHANNEL_TYPE adc_channel_t
#define STANDARD_BUFFER_SIZE 10
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
    int *buffer;
    int buffer_index;
    int buffer_sum;
    int buffer_size;
} adc_t;

adc_t *adc_init(PIN_GPIO pin, CHANNEL_TYPE channel, adc_unit_t adc_unit, int buffer);
void adc_update(adc_t *pot);
int adc_getValue(adc_t *pot);
void adc_setOnThreshold(adc_t *pot, THRESHOLD threshold, bool after, bool before, void (*onThreshold)(), void (*beforeThreshold)());
void adc_update_buffer(adc_t *pot, int value);
int adc_buffer_average(adc_t *pot);
void adc_destroy(adc_t *pot);