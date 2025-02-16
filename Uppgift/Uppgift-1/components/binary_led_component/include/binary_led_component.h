#pragma once
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"

typedef struct
{

} binary_led_component;

void binary_led_init();

void binary_led_update();

void binary_led_setLed(uint32_t value);

void binary_led_blink();

// xxx init ( pin, xxx )

// xxx update (xxx)

// xxx setLed ( value, xxx )

// xxx blink(miliseocnds_on, milliseconds_off, xxx)
