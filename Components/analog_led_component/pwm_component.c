#include "pwm_component.h"
#define MAX_DUTY 4095
int duty = 0;

// Noter
const int NOTE_G4 = 392;
const int NOTE_Ds4 = 311;
const int NOTE_A4s = 466;
const int NOTE_D5 = 587;
const int NOTE_Ds5 = 622;
const int NOTE_G5 = 784;
const int NOTE_Fs5 = 739;
const int NOTE_F5 = 698;
const int NOTE_E5 = 659;

// Melodin
const int warning_melody[] = {
    NOTE_G4, NOTE_G4, NOTE_G4, NOTE_Ds4, NOTE_A4s, NOTE_G4, NOTE_Ds4, NOTE_A4s, NOTE_G4,
    NOTE_D5, NOTE_D5, NOTE_D5, NOTE_Ds5, NOTE_A4s, NOTE_G4, NOTE_Ds4, NOTE_A4s, NOTE_G4,
    NOTE_G5, NOTE_G4, NOTE_G4, NOTE_G5, NOTE_Fs5, NOTE_F5, NOTE_E5, NOTE_Ds5, NOTE_E5,
    NOTE_A4s, NOTE_Ds5, NOTE_D5, NOTE_A4s, NOTE_Ds5, NOTE_D5, NOTE_A4s, NOTE_Ds5, NOTE_D5,
    NOTE_A4s, NOTE_G4, NOTE_Ds4, NOTE_A4s, NOTE_G4, NOTE_Ds4, NOTE_A4s, NOTE_G4, NOTE_Ds4};

// Melodi längd
const int warning_note_durations[] = {
    500, 500, 500, 350, 150, 500, 350, 150, 1000,
    500, 500, 500, 350, 150, 500, 350, 150, 1000,
    500, 350, 150, 500, 350, 150, 500, 350, 150,
    1000, 500, 350, 150, 500, 350, 150, 500, 350,
    150, 500, 350, 150, 500, 350, 150, 500, 350};

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
    new_pwm->state = PWM_OFF;
    new_pwm->stateChangeTime = 0;
    new_pwm->fade_duration = fade_duration;
    new_pwm->angle = 0.0;
    new_pwm->period = 100;
    new_pwm->duty = 0;
    new_pwm->note_playing = false;
    new_pwm->noteIndex = 0;
    return new_pwm;
};
void pwm_update(pwm_component_t *pwm)
{
    TickType_t current_time = xTaskGetTickCount();
    int fade = 50;
    switch (pwm->state)
    {
    case PWM_OFF:
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(pwm->speed_mode, pwm->channel, 0));
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(pwm->speed_mode, pwm->channel));
        pwm->note_playing = false;
        pwm->noteIndex = 0;
        pwm->next_state = PWM_OFF;
        break;
    case PWM_ON:
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(pwm->speed_mode, pwm->channel, pwm->duty));
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(pwm->speed_mode, pwm->channel));
        pwm->next_state = PWM_ON;
        break;
    case PWM_MELODY_PLAYING:
        if (!pwm->note_playing)
        {
            if (pwm->noteIndex >= sizeof(warning_melody) / sizeof(warning_melody[0]))
            {
                pwm->next_state = PWM_OFF;
                break;
            }
            ledc_set_freq(pwm->speed_mode, pwm->timer_num, warning_melody[pwm->noteIndex]);
            ledc_set_duty(pwm->speed_mode, pwm->channel, 2048); // 50% duty cycle (4096/2)
            ledc_update_duty(pwm->speed_mode, pwm->channel);

            pwm->note_playing = true;
        }
        if ((current_time - pwm->stateChangeTime) >= pdMS_TO_TICKS(warning_note_durations[pwm->noteIndex]))
        {
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(pwm->speed_mode, pwm->channel, 0));
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(pwm->speed_mode, pwm->channel));
            pwm->stateChangeTime = current_time;
            pwm->next_state = PWM_MELODY_PLAYING_DELAY;
        }
        break;
    case PWM_MELODY_PLAYING_DELAY:
        if ((current_time - pwm->stateChangeTime) >= pdMS_TO_TICKS(warning_note_durations[pwm->noteIndex]))
        {
            ledc_set_duty(pwm->speed_mode, pwm->channel, 0);
            ledc_update_duty(pwm->speed_mode, pwm->channel);
            pwm->noteIndex++;
            pwm->note_playing = false;
            pwm->stateChangeTime = current_time;
            pwm->next_state = PWM_MELODY_PLAYING;
        }
        break;
    case PWM_FADE_IN:
        if ((current_time - pwm->stateChangeTime) >= pdMS_TO_TICKS(pwm->fade_duration))
        {
            pwm->duty += fade;
            if (pwm->duty >= MAX_DUTY)
            {
                pwm->duty = MAX_DUTY;
                pwm->next_state = PWM_FADE_OUT;
            }
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(pwm->speed_mode, pwm->channel, pwm->duty));
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(pwm->speed_mode, pwm->channel));
            pwm->stateChangeTime = current_time;
        }
        else
            pwm->next_state = PWM_FADE_IN;
        break;
    case PWM_FADE_OUT:
        if (current_time - pwm->stateChangeTime > pdMS_TO_TICKS(pwm->fade_duration))
        {
            pwm->duty -= fade;
            if (pwm->duty <= 0)
            {
                pwm->duty = 0;
                pwm->next_state = PWM_FADE_IN;
            }
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(pwm->speed_mode, pwm->channel, pwm->duty));
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(pwm->speed_mode, pwm->channel));
            pwm->stateChangeTime = current_time;
        }
        else
            pwm->next_state = PWM_FADE_OUT;
        break;
    case PWM_SIN_WAVE:
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
        pwm->next_state = PWM_SIN_WAVE;
        break;

    default:
        ESP_LOGI("MAIN", "Unknown State: %d", pwm->state);
        break;
    }
    pwm->pre_state = pwm->state;
    pwm->state = pwm->next_state;
}
void pwm_set_melody(pwm_component_t *pwm, int duty, pwm_state_t state)
{
    pwm->state = state;
    pwm->duty = duty;
    // pwm->noteIndex = 0; // Reset note index
}
void pwm_set(pwm_component_t *pwm, int value)
{
    pwm->state = PWM_ON;
    pwm->duty = value;
}
void pwm_sine(pwm_component_t *pwm, int period)
{
    pwm->state = PWM_SIN_WAVE;
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

void pwm_free(pwm_component_t *pwm)
{
    vPortFree(pwm);
}