
#include "rgb_component.h"

onboard_rgb_led_t *rgb_led_init(uint8_t led_state, led_strip_handle_t led_strip, gpio_num_t led_pin)
{
    onboard_rgb_led_t *new_led = pvPortMalloc(sizeof(onboard_rgb_led_t));
    new_led->led_state = led_state;
    new_led->led_strip = led_strip;
    new_led->led_pin = led_pin;
    return new_led;
}

void rgb_led_configure(onboard_rgb_led_t *led)
{
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = led->led_pin,
        .max_leds = 1, // at least one LED on board
    };
#if CONFIG_BLINK_LED_STRIP_BACKEND_RMT
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led->led_strip));
#endif
    /* Set all LED off to clear all pixels */
    led_strip_clear(led->led_strip);
}

static void rgb_led_color(onboard_rgb_led_t *led)
{
    /* If the addressable LED is enabled */
    if (led->led_state == 1)
    {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        led_strip_set_pixel(led->led_strip, 0, 100, 0, 0);
        /* Refresh the strip to send data */
        led_strip_refresh(led->led_strip);
    }
    else if (led->led_state == 2)
    {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        led_strip_set_pixel(led->led_strip, 0, 230, 29, 0);
        /* Refresh the strip to send data */
        led_strip_refresh(led->led_strip);
    }
    else
    {
        /* Set all LED off to clear all pixels */
        led_strip_clear(led->led_strip);
    }
}

void rgb_led_set_state(onboard_rgb_led_t *led)
{
    rgb_led_color(led);
}

void rgb_led_free(onboard_rgb_led_t *led)
{
    vPortFree(led);
    led = NULL;
}