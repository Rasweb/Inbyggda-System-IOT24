#include "binary_led_component.h"

binary_led_component *binary_led_init(gpio_mode_t gpio_mode, int pin, int pull_down, int pull_up)
{
    gpio_config_t ledc_config = {
        .mode = gpio_mode,
        .pin_bit_mask = ((uint64_t)1) << pin,
        .pull_down_en = pull_down,
        .pull_up_en = pull_up,
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&ledc_config));

    binary_led_component *new_binary_led = pvPortMalloc(sizeof(binary_led_component));

    new_binary_led->gpio_mode = gpio_mode;
    new_binary_led->pin = pin;
    new_binary_led->pull_down = pull_down;
    new_binary_led->pull_up = pull_up;
    return new_binary_led;
};

void binary_led_update(int pin, int value)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_set_level(pin, value));
};

void binary_led_setLed(int pin, int value)
{
    binary_led_update(pin, value);
}

void binary_led_blink(blink_states binary_states, int secOn, int secOff)
{
    //    xxx blink(miliseocnds_on, milliseconds_off, xxx)

    TickType_t on_time = xTaskGetTickCount();
    TickType_t off_time = xTaskGetTickCount();

    switch (binary_states)
    {
    case BLINK_ON:

    case BLINK_OFF:

    default:
        break;
    }
};

void binary_destroy(binary_led_component *binary_led)
{
    if (binary_led != NULL)
    {
        vPortFree(binary_led);
        binary_led = NULL;
    }
}
