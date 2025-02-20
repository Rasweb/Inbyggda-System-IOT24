#include "analog_led_component.h"
#define MAX_DUTY 4095
int duty = 0;

analog_led_component *analog_led_init(int pin, uint32_t freq_hertz, ledc_mode_t speed_mode, ledc_timer_t timer_num, ledc_channel_t channel, uint32_t duty_range, uint32_t fade_duration)
{
    // LEDC timer configuration
    ledc_timer_config_t ledcTimerConfig = {
        .speed_mode = speed_mode,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .timer_num = timer_num,
        .freq_hz = freq_hertz,
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false,
    };
    ledc_timer_config(&ledcTimerConfig);

    // LEDC channel configuration
    ledc_channel_config_t ledcChannelConfig = {
        .gpio_num = pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
        .flags.output_invert = 0,
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
    new_binary_led->angle = 0.0;
    new_binary_led->period = 100;
    return new_binary_led;
};
void analog_led_update(analog_led_component *led)
{
    TickType_t current_time = xTaskGetTickCount();
    int fade = 50;
    // printf("state: %d", led->state);
    ESP_LOGI("MAIN", "Current State: %d, Next State: %d", led->state, led->next_state);
    switch (led->state)
    {
    case ANALOG_OFF:
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(led->speed_mode, led->channel, 0));
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(led->speed_mode, led->channel));
        break;
    case ANALOG_ON:
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(led->speed_mode, led->channel, MAX_DUTY));
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(led->speed_mode, led->channel));
        break;
    case ANALOG_FADE_IN:
        if ((current_time - led->stateChangeTime) >= pdMS_TO_TICKS(led->fade_duration))
        {
            duty += fade;
            // printf("fade in %d", duty);
            if (duty >= MAX_DUTY)
            {
                duty = MAX_DUTY;
                led->next_state = ANALOG_FADE_OUT;
            }
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(led->speed_mode, led->channel, duty));
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(led->speed_mode, led->channel));
            led->stateChangeTime = current_time;
        }
        else
            led->next_state = ANALOG_FADE_IN;
        break;

    case ANALOG_FADE_OUT:
        if (current_time - led->stateChangeTime > pdMS_TO_TICKS(led->fade_duration))
        {
            duty -= fade;
            // printf("fade out %d", duty);
            if (duty <= 0)
            {
                duty = 0;
                led->next_state = ANALOG_FADE_IN;
            }
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(led->speed_mode, led->channel, duty));
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(led->speed_mode, led->channel));
            led->stateChangeTime = current_time;
        }
        else
            led->next_state = ANALOG_FADE_OUT;
        break;
    case ANALOG_SIN_WAVE:
        printf("Duty: %d", duty);
        // Updatera angle för sin uträkningen
        led->angle += ((2 * M_PI) / led->period);
        // Titta om angle har fulföljt ett helt varv
        if (led->angle >= 2 * M_PI)
        {
            // Startar ett nytt varv
            led->angle = 0.0;
        }
        // Räknar ut duty för led:en
        // Använder sin för att variera styrkan
        // Cykeln är mellan 0 och MAX_DUTY
        duty = (MAX_DUTY / 2) + (MAX_DUTY / 2) * sin(led->angle);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(led->speed_mode, led->channel, duty));
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(led->speed_mode, led->channel));
        led->stateChangeTime = current_time;
        break;

    default:
        ESP_LOGI("MAIN", "Unknown State: %d", led->state);
        break;
    }
    led->pre_state = led->state;
    led->state = led->next_state;
}
void analog_setLed(analog_led_component *led, int value)
{
    // led->state = value;
    if (value == ANALOG_ON)
        led->state = ANALOG_ON;
    else if (value == ANALOG_OFF)
        led->state = ANALOG_OFF;
    else if (value == ANALOG_FADE_IN)
        led->state = ANALOG_FADE_IN;
    else if (value == ANALOG_FADE_OUT)
        led->state = ANALOG_FADE_OUT;
    else if (value == ANALOG_SIN_WAVE)
        led->state = ANALOG_SIN_WAVE;
}
void analog_sin(analog_led_component *led, int period)
{
    printf("State in sin: %d", led->state);
    led->state = ANALOG_SIN_WAVE;
    led->period = period;
    led->stateChangeTime = xTaskGetTickCount();
}
