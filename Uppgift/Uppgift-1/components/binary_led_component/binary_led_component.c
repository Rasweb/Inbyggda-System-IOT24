#include "binary_led_component.h"

binary_led_component *binary_led_init(gpio_mode_t gpio_mode, int pin, int pull_down, int pull_up)
{
    gpio_config_t ledc_config = {
        .pin_bit_mask = ((uint64_t)1) << pin,
        .mode = gpio_mode,
        .pull_down_en = pull_down,
        .pull_up_en = pull_up,
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&ledc_config));

    binary_led_component *new_binary_led = pvPortMalloc(sizeof(binary_led_component));

    new_binary_led->gpio_mode = gpio_mode;
    new_binary_led->pin = pin;
    new_binary_led->pull_down = pull_down;
    new_binary_led->pull_up = pull_up;
    new_binary_led->stateChangeTime = 0;
    new_binary_led->state = BINARY_OFF;
    new_binary_led->miliseconds_on = 500;
    new_binary_led->miliseconds_off = 500;
    return new_binary_led;
};

void binary_led_update(binary_led_component *led)
{
    TickType_t current_time = xTaskGetTickCount();
    switch (led->state)
    {
    case BINARY_ON:
        binary_led_setLed(led->pin, 1);
        break;
    case BINARY_OFF:
        binary_led_setLed(led->pin, 0);
        break;
    case BINARY_BLINK_ON:
        if ((current_time - led->stateChangeTime) >= pdMS_TO_TICKS(led->miliseconds_on))
        {
            binary_led_setLed(led->pin, 0);
            led->state = BINARY_BLINK_OFF;
            led->stateChangeTime = current_time;
        }
        break;
    case BINARY_BLINK_OFF:
        if ((current_time - led->stateChangeTime) >= pdMS_TO_TICKS(led->miliseconds_off))
        {
            binary_led_setLed(led->pin, 1);
            led->state = BINARY_BLINK_ON;
            led->stateChangeTime = current_time;
        }
        break;
    default:
        ESP_LOGE("LED", "Unknown state");
        led->state = BINARY_OFF;
        break;
    }
};

void binary_led_setLed(int pin, int value)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_set_level(pin, value));
}

void binary_led_blink(binary_led_component *led, uint32_t miliseconds_on, uint32_t miliseconds_off)
{
    led->miliseconds_on = miliseconds_on;
    led->miliseconds_off = miliseconds_off;
    led->state = BINARY_BLINK_ON;
    led->stateChangeTime = xTaskGetTickCount();
    binary_led_setLed(led->pin, 1);
};

void binary_destroy(binary_led_component *binary_led)
{
    if (binary_led != NULL)
    {
        vPortFree(binary_led);
        binary_led = NULL;
    }
}
