#pragma once
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#define PIN_GPIO int

typedef struct
{
    PIN_GPIO pot_pin;
    void (*onThresholdCallback)(int pin);
} potentiometer;

void pot_init(PIN_GPIO pin);
void update();
int getValue();
void setOnThreshold(potentiometer *pot, int threshold, int risingEdge, void (*onThreshold)());
// xxx init ( pin/adc, xxx )

// xxx update (xxx)

// int getValue( xxx )

// void setOnThreshold ( int threshold, bool risingEdge, void (*onThreshold)(int pin/adc, value) )
