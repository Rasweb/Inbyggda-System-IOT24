#include "analog_led_component.h"

analog_led_component *analog_led_init(int pin, uint32_t freq_hertz, ledc_mode_t speed_mode, ledc_timer_t timer_num, ledc_channel_t channel, uint32_t duty_range, uint32_t fade_duration)
{
    // LEDC timer configuration
    ledc_timer_config_t ledcTimerConfig = {
        .clk_cfg = LEDC_AUTO_CLK,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .freq_hz = freq_hertz,
        .speed_mode = speed_mode,
        .timer_num = timer_num,
        .deconfigure = false,
    };
    ledc_timer_config(&ledcTimerConfig);

    // LEDC channel configuration
    ledc_channel_config_t ledcChannelConfig = {
        .channel = channel,
        .duty = 0,
        .gpio_num = pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0,
        .flags.output_invert = 0,
        .intr_type = LEDC_INTR_DISABLE,
    };
    ledc_channel_config(&ledcChannelConfig);

    analog_led_component *new_binary_led = pvPortMalloc(sizeof(analog_led_component));
    new_binary_led->pin = pin;
    new_binary_led->freq_hertz = freq_hertz;
    new_binary_led->speed_mode = speed_mode;
    new_binary_led->timer_num = timer_num;
    new_binary_led->channel = channel;
    new_binary_led->duty_range = duty_range;
    new_binary_led->state = ANALOG_OFF;
    new_binary_led->stateChangeTime = 0;
    new_binary_led->fade_duration = fade_duration;

    return new_binary_led;
};

void analog_led_update(analog_led_component *analog_led)
{
    ledc_fade_func_install(analog_led->speed_mode);
    TickType_t current_time = xTaskGetTickCount();
    switch (analog_led->state)
    {
    case ANALOG_ON:
        analog_setLed(analog_led, 4095);
        break;
    case ANALOG_OFF:
        analog_setLed(analog_led, 0);
        break;
        // For fade in/out - https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32c6/api-reference/peripherals/ledc.html#change-pwm-duty-cycle-using-hardware
        // For fade in/out - https://github.com/espressif/esp-idf/blob/v5.4/examples/peripherals/ledc/ledc_fade/main/ledc_fade_example_main.c
    case ANALOG_FADE_IN:
        ledc_set_fade_with_time(analog_led->speed_mode, analog_led->channel, 4095, analog_led->fade_duration);
        ledc_fade_start(analog_led->speed_mode, analog_led->channel, LEDC_FADE_NO_WAIT);
        analog_led->state = ANALOG_FADE_OUT;
        break;
    case ANALOG_FADE_OUT:
        ledc_set_fade_with_time(analog_led->speed_mode, analog_led->channel, 0, analog_led->fade_duration);
        ledc_fade_start(analog_led->speed_mode, analog_led->channel, LEDC_FADE_NO_WAIT);
        analog_led->state = ANALOG_FADE_IN;
        break;

    case ANALOG_SIN_WAVE:
        break;
    default:
        break;
    }
}
void analog_setLed(analog_led_component *analog_led, int value)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(analog_led->speed_mode, analog_led->channel, value));
    ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(analog_led->speed_mode, analog_led->channel));
}
void analog_sin() {}

// ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, current_duty, 0);
// current_duty = ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
// ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, value, 0); // update

// ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, value));
// Update duty to apply the new value
// ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
