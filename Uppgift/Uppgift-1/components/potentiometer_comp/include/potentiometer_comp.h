#pragma once
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#define PIN_GPIO int
#define THRESHOLD int

typedef struct adc_oneshot_unit_ctx_t *adc_oneshot_unit_handle_t;
typedef struct
{
    PIN_GPIO pin;
    int lastValue;
    void (*onThresholdCallback)(int value);
    int adc_raw;
    int threshold;
    int risingEdge;
    adc_oneshot_unit_handle_t handle;
} potentiometer;

// xxx init(pin / adc, xxx)
potentiometer *pot_init(PIN_GPIO pin, THRESHOLD threshold);
void pot_update(potentiometer *pot);
int pot_getValue(potentiometer *pot);

// xxx setOnThreshold ( int threshold, bool risingEdge, xxx (*onThreshold)(int pin/adc, value, xxx), xxx )
void pot_setOnThreshold(potentiometer *pot, void (*onThreshold)());
void pot_destroy(potentiometer *pot);