#include "analog_led_component.h"
#define MAX_DUTY 4095
int duty = 0;

pwm_component_t *pwm_init(int pin, uint32_t freq_hertz, ledc_mode_t speed_mode, ledc_timer_t timer_num, ledc_channel_t channel, uint32_t duty_range, uint32_t fade_duration)
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

    pwm_component_t *new_pwm = pvPortMalloc(sizeof(pwm_component_t));
    new_pwm->pin = pin;
    new_pwm->freq_hertz = freq_hertz;
    new_pwm->speed_mode = speed_mode;
    new_pwm->timer_num = timer_num;
    new_pwm->channel = channel;
    new_pwm->duty_range = duty_range;
    new_pwm->state = ANALOG_OFF;
    new_pwm->stateChangeTime = 0;
    new_pwm->fade_duration = fade_duration;
    new_pwm->angle = 0.0;
    new_pwm->period = 100;
    new_pwm->duty = 0;
    return new_pwm;
};
void pwm_update(pwm_component_t *pwm)
{
    TickType_t current_time = xTaskGetTickCount();
    int fade = 50;
    // printf("state: %d", led->state);
    // ESP_LOGI("MAIN", "Current State: %d, Next State: %d", led->state, led->next_state);
    switch (pwm->state)
    {
    case ANALOG_OFF:
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(pwm->speed_mode, pwm->channel, 0));
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(pwm->speed_mode, pwm->channel));
        pwm->next_state = ANALOG_OFF;
        break;
    case ANALOG_ON:
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(pwm->speed_mode, pwm->channel, pwm->duty));
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(pwm->speed_mode, pwm->channel));
        pwm->next_state = ANALOG_ON;
        break;
    case ANALOG_FADE_IN:
        if ((current_time - pwm->stateChangeTime) >= pdMS_TO_TICKS(pwm->fade_duration))
        {
            pwm->duty += fade;
            if (pwm->duty >= MAX_DUTY)
            {
                pwm->duty = MAX_DUTY;
                pwm->next_state = ANALOG_FADE_OUT;
            }
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(pwm->speed_mode, pwm->channel, pwm->duty));
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(pwm->speed_mode, pwm->channel));
            pwm->stateChangeTime = current_time;
        }
        else
            pwm->next_state = ANALOG_FADE_IN;
        break;

    case ANALOG_FADE_OUT:
        if (current_time - pwm->stateChangeTime > pdMS_TO_TICKS(pwm->fade_duration))
        {
            pwm->duty -= fade;
            if (pwm->duty <= 0)
            {
                pwm->duty = 0;
                pwm->next_state = ANALOG_FADE_IN;
            }
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(pwm->speed_mode, pwm->channel, pwm->duty));
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(pwm->speed_mode, pwm->channel));
            pwm->stateChangeTime = current_time;
        }
        else
            pwm->next_state = ANALOG_FADE_OUT;
        break;
    case ANALOG_SIN_WAVE:
        // Updatera vinkel för sin uträkningen
        // led->period är bestämer vinkel tiden
        pwm->angle += ((2 * M_PI) / pwm->period);
        // Titta om angle har fulföljt ett helt varv
        if (pwm->angle >= 2 * M_PI)
        {
            // Startar ett nytt varv
            pwm->angle = 0.0;
        }
        // Räknar ut duty för led:en
        // Använder sin för att variera styrkan
        // Cykeln är mellan 0 och MAX_DUTY
        duty = (MAX_DUTY / 2) + (MAX_DUTY / 2) * sin(pwm->angle);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(pwm->speed_mode, pwm->channel, duty));
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(pwm->speed_mode, pwm->channel));
        pwm->stateChangeTime = current_time;
        pwm->next_state = ANALOG_SIN_WAVE;
        break;

    default:
        ESP_LOGI("MAIN", "Unknown State: %d", pwm->state);
        break;
    }
    pwm->pre_state = pwm->state;
    pwm->state = pwm->next_state;
}
void pwm_set(pwm_component_t *pwm, int value)
{
    pwm->state = ANALOG_ON;
    pwm->duty = value;
}
void pwm_sine(pwm_component_t *pwm, int period)
{
    pwm->state = ANALOG_SIN_WAVE;
    pwm->period = period;
    pwm->stateChangeTime = xTaskGetTickCount();
}

void pwm_destroy(pwm_component_t *pwm)
{
    if (pwm != NULL)
    {
        vPortFree(pwm);
        pwm = NULL;
    }
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