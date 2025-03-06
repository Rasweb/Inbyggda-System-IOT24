#include "binary_led_component.h"

binary_led_component *binary_led_init(gpio_mode_t gpio_mode, int pin, int pull_down, int pull_up)
{
    gpio_config_t ledc_config = {
        .pin_bit_mask = 1ULL << pin,
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
    printf("state: %d\t", led->state);

    switch (led->state)
    {
    case BINARY_ON:
        ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_set_level(led->pin, led->state));
        break;
    case BINARY_OFF:
        ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_set_level(led->pin, led->state));
        break;
    case BINARY_BLINK_ON:
        if ((current_time - led->stateChangeTime) >= pdMS_TO_TICKS(led->miliseconds_on))
        {
            ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_set_level(led->pin, BINARY_ON));
            led->stateChangeTime = current_time;
            led->next_state = BINARY_BLINK_OFF;
            printf("\n\n");
        }
        else
            led->next_state = BINARY_BLINK_ON;
        break;
    case BINARY_BLINK_OFF:
        if ((current_time - led->stateChangeTime) >= pdMS_TO_TICKS(led->miliseconds_off))
        {
            ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_set_level(led->pin, BINARY_OFF));
            led->stateChangeTime = current_time;
            led->next_state = BINARY_BLINK_ON;
            printf("\n\n");
        }
        else
            led->next_state = BINARY_BLINK_OFF;
        break;
    default:
        ESP_LOGE("LED", "Unknown state");
        led->state = BINARY_OFF;
        break;
    }
    led->pre_state = led->state;
    led->state = led->next_state;
};

void binary_led_setLed(binary_led_component *led, int value)
{
    led->state = value;
}

void binary_led_blink(binary_led_component *led, uint32_t miliseconds_on, uint32_t miliseconds_off)
{
    led->miliseconds_on = miliseconds_on;
    led->miliseconds_off = miliseconds_off;
    led->state = BINARY_BLINK_ON;
};

void binary_destroy(binary_led_component *binary_led)
{
    if (binary_led != NULL)
    {
        vPortFree(binary_led);
        binary_led = NULL;
    }
}
