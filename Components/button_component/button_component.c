#include <stdio.h>
#include "button_component.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#define TAG "MAIN"
#define DEBOUNCE_DELAY 10

button_component *btn_init(PIN_TYPE pin, gpio_pull_down_mode pull_down, gpio_pull_up_mode pull_up, gpio_int_type_mode intr)
{
    gpio_config_t button_config = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = ((uint64_t)1) << pin,
        .pull_down_en = pull_down,
        .pull_up_en = pull_up,
        .intr_type = intr,
    };

    button_component *new_btn = pvPortMalloc(sizeof(button_component));
    new_btn->btn_pin = pin;
    new_btn->state = STATE_RELEASED;
    new_btn->stateChangeTime = 0;
    new_btn->onReleasedCallback = NULL;
    new_btn->onPressedCallback = NULL;
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&button_config));
    return new_btn;
};

void btn_update(button_component *btn)
{
    int level = gpio_get_level(btn->btn_pin);
    TickType_t current_time = xTaskGetTickCount();
    switch (btn->state)
    {
    case STATE_RELEASED:
        if (level == 1)
        {
            btn->state = STATE_PRESS_DEBOUNCE;
            btn->stateChangeTime = current_time;
            ESP_LOGI(TAG, "Entering STATE_PRESS_DEBOUNCE");
        }
        break;
    case STATE_PRESS_DEBOUNCE:
        if ((current_time - btn->stateChangeTime) >= pdMS_TO_TICKS(DEBOUNCE_DELAY))
        {
            if (gpio_get_level(btn->btn_pin) == 1)
            {
                btn->state = STATE_PRESSED;
                btn->stateChangeTime = current_time;
                ESP_LOGI(TAG, "Button Pressed, entering STATE_PRESSED");
                if (btn->onPressedCallback != NULL)
                    btn->onPressedCallback(btn->btn_pin);
            }
            else
            {
                btn->state = STATE_RELEASED;
                ESP_LOGI(TAG, "Bounce detected during press; returning to STATE_RELEASED");
            }
        }
        break;

    case STATE_PRESSED:
        if (level == 0)
        {
            btn->state = STATE_RELEASE_DEBOUNCE;
            btn->stateChangeTime = current_time;
            ESP_LOGI(TAG, "Entering STATE_RELEASE_DEBOUNCE");
        }
        break;
    case STATE_RELEASE_DEBOUNCE:
        if ((current_time - btn->stateChangeTime) >= pdMS_TO_TICKS(DEBOUNCE_DELAY))
        {
            if (gpio_get_level(btn->btn_pin) == 0)
            {
                btn->state = STATE_RELEASED;
                btn->stateChangeTime = current_time;
                ESP_LOGI(TAG, "Button Released, returning to STATE_RELEASED");
                // if(btn->onReleasedCallback != NULL)
                //     btn->onReleasedCallback(btn->btn_pin);
            }
            else
            {
                btn->state = STATE_PRESSED;
                ESP_LOGI(TAG, "Bounce detected during release; returning to STATE_PRESSED");
            }
        }
        break;
    default:
        ESP_LOGE(TAG, "Unknown state");
        btn->state = STATE_RELEASED;
        break;
    }
};

int btn_isPressed(button_component *btn)
{
    return (btn->state == STATE_PRESSED) ? 1 : 0;
};

void btn_setOnPressed(button_component *btn, void (*onPressed)(int pin))
{
    btn->onPressedCallback = onPressed;
};

void btn_destroy(button_component *btn)
{
    vPortFree(btn);
    btn = NULL;
}