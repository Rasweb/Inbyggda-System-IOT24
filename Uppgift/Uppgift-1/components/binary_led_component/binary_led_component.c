#include "binary_led_component.h"

void binary_led_init(int pin)
{

    // LEDC timer configuration
    ledc_timer_config_t ledcTimerConfig = {
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .freq_hz = 1000,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
    };
    ledc_timer_config(&ledcTimerConfig);

    ledc_channel_config_t ledcChannelConfig = {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,
        .flags.output_invert = 0,
        .gpio_num = pin,
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0,
    };
    ledc_channel_config(&ledcChannelConfig);
};

void binary_led_update()
{

    uint32_t current_duty;
    // ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, current_duty, 0);
    current_duty = ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
};

void binary_led_setLed(uint32_t value)
{
    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, value, 0);
};

void binary_led_blink() {
};