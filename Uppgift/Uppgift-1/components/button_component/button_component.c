#include <stdio.h>
#include "button_component.h"
#include "freertos/FreeRTOS.h"

static void (*onPressedCallback)(int pin) = NULL;

button_component button_init(PIN_TYPE pin, gpio_pull_down_mode pull_down, gpio_pull_up_mode pull_up, gpio_int_type_mode intr)
{
    gpio_config_t button_config = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = ((uint64_t)1) << pin,
        .pull_down_en = pull_down,
        .pull_up_en = pull_up,
        .intr_type = intr,
    };
    button_component new_btn;

    if (pull_down == 1)
    {
        new_btn.btn_pull_down = 1;
    }
    else
    {
        new_btn.btn_pull_down = 0;
    }

    new_btn.btn_pressed = false;
    new_btn.btn_latch = false;
    new_btn.btn_pin = pin;
    new_btn.last_press_time = 0;
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&button_config));
    return new_btn;
};

// Implementera debounce
void button_update(button_component *btn)
{
    int lvl = gpio_get_level(btn->btn_pin);
    // current time
    TickType_t current_time = xTaskGetTickCount();

    if (lvl == 1 && btn->btn_latch == 1)
    {
        btn->btn_latch = false;
        btn->btn_pressed = false;
    }
    else if (lvl == 0 && btn->btn_latch == false)
    {
        // 10 ms debounce
        if ((current_time - btn->last_press_time) >= pdMS_TO_TICKS(10))
        {
            btn->btn_latch = true;
            btn->btn_pressed = true;
            btn->last_press_time = current_time;

            if (onPressedCallback != NULL)
            {
                onPressedCallback(btn->btn_pin);
            }
        }
    }
};

int button_isPressed(button_component *btn)
{
    return btn->btn_pressed;
};

void setOnPressed(void (*onPressed)(int pin))
{
    onPressedCallback = onPressed;
};
