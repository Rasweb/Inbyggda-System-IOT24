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
    new_binary_led->duty = 0;
    return new_binary_led;
};
void analog_led_update(analog_led_component *led)
{
    TickType_t current_time = xTaskGetTickCount();
    int fade = 50;
    // printf("state: %d", led->state);
    // ESP_LOGI("MAIN", "Current State: %d, Next State: %d", led->state, led->next_state);
    switch (led->state)
    {
    case ANALOG_OFF:
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(led->speed_mode, led->channel, 0));
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(led->speed_mode, led->channel));
        led->next_state = ANALOG_OFF;
        break;
    case ANALOG_ON:
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(led->speed_mode, led->channel, led->duty));
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(led->speed_mode, led->channel));
        ESP_LOGI("MAIN", "analog on duty: %d", led->duty);
        led->next_state = ANALOG_ON;
        break;
    case ANALOG_FADE_IN:
        if ((current_time - led->stateChangeTime) >= pdMS_TO_TICKS(led->fade_duration))
        {
            ESP_LOGI("MAIN", "fade in duty: %d", led->duty);
            led->duty += fade;
            if (led->duty >= MAX_DUTY)
            {
                led->duty = MAX_DUTY;
                led->next_state = ANALOG_FADE_OUT;
            }
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(led->speed_mode, led->channel, led->duty));
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(led->speed_mode, led->channel));
            led->stateChangeTime = current_time;
        }
        else
            led->next_state = ANALOG_FADE_IN;
        break;

    case ANALOG_FADE_OUT:
        if (current_time - led->stateChangeTime > pdMS_TO_TICKS(led->fade_duration))
        {
            ESP_LOGI("MAIN", "fade out duty: %d", led->duty);
            led->duty -= fade;
            if (led->duty <= 0)
            {
                led->duty = 0;
                led->next_state = ANALOG_FADE_IN;
            }
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(led->speed_mode, led->channel, led->duty));
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(led->speed_mode, led->channel));
            led->stateChangeTime = current_time;
        }
        else
            led->next_state = ANALOG_FADE_OUT;
        break;
    case ANALOG_SIN_WAVE:
        ESP_LOGI("MAIN", "Sin wave duty: %d", duty);
        // Updatera vinkel för sin uträkningen
        // led->period är bestämer vinkel tiden
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
        led->next_state = ANALOG_SIN_WAVE;
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
    led->state = ANALOG_ON;
    led->duty = value;
}
void analog_sin(analog_led_component *led, int period)
{
    led->state = ANALOG_SIN_WAVE;
    led->period = period;
    led->stateChangeTime = xTaskGetTickCount();
}

// För framtida kod, om man vill sätta specifika värden för olika state
// void analog_setLed(analog_led_component *led, analog_led_state_t state, int value)
// {
//     if (state == ANALOG_OFF)
//     {
//         led->state = ANALOG_OFF;
//         led->duty = 0;
//     }
//     else if (state == ANALOG_ON)

//     {
//         led->state = ANALOG_ON;
//         led->duty = value;
//     }
//     else if (state == ANALOG_FADE_IN)
//     {
//         led->state = ANALOG_FADE_IN;
//         led->duty = value;
//     }
//     else if (state == ANALOG_FADE_OUT)
//     {
//         led->state = ANALOG_FADE_OUT;
//         led->duty = value;
//     }
//     else if (state == ANALOG_SIN_WAVE)
//     {
//         led->state = ANALOG_SIN_WAVE;
//         led->angle = value;
//         analog_sin(led, 100);
//     }
// }