#pragma once
#include "driver/gpio.h"
#define PIN_TYPE int

typedef enum
{
    PULL_DOWN_DISABLE = 0x0, /*!< Disable GPIO pull-down resistor */
    PULL_DOWN_ENABLE = 0x1,  /*!< Enable GPIO pull-down resistor  */
} gpio_pull_down_mode;

typedef enum
{
    PULL_UP_DISABLE = 0x0, /*!< Disable GPIO pull-up resistor */
    PULL_UP_ENABLE = 0x1,  /*!< Enable GPIO pull-up resistor */
} gpio_pull_up_mode;

typedef enum
{
    INT_DISABLE = 0,     /*!< Disable GPIO interrupt                             */
    INTR_POSEDGE = 1,    /*!< GPIO interrupt type : rising edge                  */
    INTR_NEGEDGE = 2,    /*!< GPIO interrupt type : falling edge                 */
    INTR_ANYEDGE = 3,    /*!< GPIO interrupt type : both rising and falling edge */
    INTR_LOW_LEVEL = 4,  /*!< GPIO interrupt type : input low level trigger      */
    INTR_HIGH_LEVEL = 5, /*!< GPIO interrupt type : input high level trigger     */
    INTR_MAX,
} gpio_int_type_mode;

typedef struct
{
    bool btn_pressed;
    bool btn_latch;
    PIN_TYPE btn_pin;
    int btn_pull_down;
    int last_press_time;
} button_component;

button_component button_init(PIN_TYPE pin, gpio_pull_down_mode pull_down, gpio_pull_up_mode pull_up, gpio_int_type_mode intr);

// För interrupt
// onPressed kallas i button update
void button_update(button_component *btn);

int button_isPressed(button_component *btn);

// Får inte kallas från interrupt
// Sätter onPressed
void setOnPressed(void (*onPressed)(int pin));