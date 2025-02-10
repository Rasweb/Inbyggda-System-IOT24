#include <stdio.h>
#include "button_component.h"
#include "freertos/FreeRTOS.h"

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
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&button_config));
    return new_btn;
};

// Implementera debounce
void button_update(button_component *btn)
{
    int lvl = gpio_get_level(btn->btn_pin);
    if (lvl == 1 && btn->btn_latch == 1)
    {
        printf("Pin: %d, %d\n", btn->btn_pin, lvl);
        btn->btn_latch = false;
        btn->btn_pressed = false;
    }
    else if (lvl == 0 && btn->btn_latch == 0)
    {
        printf("Pin: %d, %d\n", btn->btn_pin, lvl);
        btn->btn_latch = true;
        btn->btn_pressed = true;
    }
};

int button_isPressed(button_component *btn)
{
    // if (btn->btn_pull_down)
    // {
    //     printf("1");
    // }
    // else
    // {
    //     printf("0");
    // }
    // printf("btn pressed: %d", btn->btn_pressed);
    return btn->btn_pressed;
};

void setOnPressed(void (*onPressed)(int pin)) {
    // btn->isPressed = true;
    // btn->isPressed = false;
};
