#include "pwm_component.h"
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
    new_pwm->state = PWM_OFF;
    new_pwm->stateChangeTime = 0;
    new_pwm->fade_duration = fade_duration;
    new_pwm->angle = 0.0;
    new_pwm->period = 100;
    new_pwm->duty = 0;
    new_pwm->note_playing = false;
    new_pwm->noteIndex = 0;
    new_pwm->warning_melody_arr = NULL;
    new_pwm->warning_note_durations_arr = NULL;
    new_pwm->melody_choice = 1;
    new_pwm->warning_melody_arr_size = 0;
    return new_pwm;
};
void set_melody(pwm_component_t *pwm, int *melody, int *durations, int size, int choice)
{
    pwm->warning_melody_arr = (int *)malloc(pwm->warning_melody_arr_size * sizeof(int));
    pwm->warning_note_durations_arr = (int *)malloc(pwm->warning_melody_arr_size * sizeof(int));

    for (int i = 0; i < pwm->warning_melody_arr_size; i++)
    {
        pwm->warning_melody_arr[i] = melody[i];
        pwm->warning_note_durations_arr[i] = durations[i];
    }
    pwm->melody_choice = choice;
}

void choose_melody(pwm_component_t *pwm, int choice)
{
    if (pwm->warning_melody_arr != NULL)
    {
        free(pwm->warning_melody_arr);
        pwm->warning_melody_arr = NULL;
    }
    if (pwm->warning_note_durations_arr != NULL)
    {
        free(pwm->warning_note_durations_arr);
        pwm->warning_note_durations_arr = NULL;
    }

    ESP_LOGI("ss", "here choice: %d", choice);
    // Imperial march
    if (choice == 1)
    {
        // Melodin
        int imperial_melody[] = {
            NOTE_G4, NOTE_G4, NOTE_G4, NOTE_DS4, NOTE_AS4, NOTE_G4, NOTE_DS4, NOTE_AS4, NOTE_G4,
            NOTE_D5, NOTE_D5, NOTE_D5, NOTE_DS5, NOTE_AS4, NOTE_G4, NOTE_DS4, NOTE_AS4, NOTE_G4,
            NOTE_G5, NOTE_G4, NOTE_G4, NOTE_G5, NOTE_FS5, NOTE_F5, NOTE_E5, NOTE_DS5, NOTE_E5,
            NOTE_AS4, NOTE_DS5, NOTE_D5, NOTE_AS4, NOTE_DS5, NOTE_D5, NOTE_AS4, NOTE_DS5, NOTE_D5,
            NOTE_AS4, NOTE_G4, NOTE_DS4, NOTE_AS4, NOTE_G4, NOTE_DS4, NOTE_AS4, NOTE_G4, NOTE_DS4};

        // Melodi längd
        int imperial_note_durations[] = {
            500, 500, 500, 350, 150, 500, 350, 150, 1000,
            500, 500, 500, 350, 150, 500, 350, 150, 1000,
            500, 350, 150, 500, 350, 150, 500, 350, 150,
            1000, 500, 350, 150, 500, 350, 150, 500, 350,
            150, 500, 350, 150, 500, 350, 150, 500, 350};

        pwm->warning_melody_arr_size = sizeof(imperial_melody) / sizeof(imperial_melody[0]);
        set_melody(pwm, imperial_melody, imperial_note_durations, pwm->warning_melody_arr_size, choice);
    }
    // Pirates of the caribbean
    else if (choice == 2)
    {
        ESP_LOGI("TSS", "It'sa here");
        int pirates_melody[] = {
            NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4,
            NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5,
            NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4,
            NOTE_A4, NOTE_G4, NOTE_A4,

            NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4,
            NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5,
            NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4,
            NOTE_A4, NOTE_G4, NOTE_A4,

            NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4,
            NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5,
            NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5,
            NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4,

            NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5,
            NOTE_D5, NOTE_E5, NOTE_A4,
            NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4,
            NOTE_C5, NOTE_A4, NOTE_B4,

            NOTE_A4, NOTE_A4,
            // Repeat of first part
            NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5,
            NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4,
            NOTE_A4, NOTE_G4, NOTE_A4,

            NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4,
            NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5,
            NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4,
            NOTE_A4, NOTE_G4, NOTE_A4,

            NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4,
            NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5,
            NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5,
            NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4,

            NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5,
            NOTE_D5, NOTE_E5, NOTE_A4,
            NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4,
            NOTE_C5, NOTE_A4, NOTE_B4,
            // End of Repeat

            NOTE_E5, NOTE_F5,
            NOTE_E5, NOTE_E5, NOTE_G5, NOTE_E5, NOTE_D5,
            NOTE_D5, NOTE_C5,
            NOTE_B4, NOTE_C5, NOTE_B4, NOTE_A4,

            NOTE_E5, NOTE_F5,
            NOTE_E5, NOTE_E5, NOTE_G5, NOTE_E5, NOTE_D5,
            NOTE_D5, NOTE_C5,
            NOTE_B4, NOTE_C5, NOTE_B4, NOTE_A4};

        int pirates_note_durations[] = {
            125.00, 125.00, 250.00, 125.00, 125.00,
            125.00, 125.00, 250.00, 125.00, 125.00,
            125.00, 125.00, 250.00, 125.00, 125.00,
            125.00, 125.00, 250.00, 125.00,

            125.00, 125.00, 250.00, 125.00, 125.00,
            125.00, 125.00, 250.00, 125.00, 125.00,
            125.00, 125.00, 250.00, 125.00, 125.00,
            125.00, 125.00, 250.00, 125.00,

            125.00, 125.00, 250.00, 125.00, 125.00,
            125.00, 125.00, 250.00, 125.00, 125.00,
            125.00, 125.00, 125.00, 250.00, 125.00,

            125.00, 125.00, 250.00, 125.00, 125.00,
            250.00, 125.00, 250.00, 125.00,
            125.00, 125.00, 250.00, 125.00, 125.00,
            125.00, 125.00, 250.00, 250.00,

            250.00, 125.00,
            // Repeat of First Part
            125.00, 125.00, 250.00, 125.00, 125.00,
            125.00, 125.00, 250.00, 125.00, 125.00,
            125.00, 125.00, 250.00, 125.00,

            125.00, 125.00, 250.00, 125.00, 125.00,
            125.00, 125.00, 250.00, 125.00, 125.00,
            125.00, 125.00, 250.00, 125.00,
            125.00, 125.00, 250.00, 125.00,

            125.00, 125.00, 250.00, 125.00, 125.00,
            250.00, 125.00, 250.00, 125.00,
            125.00, 125.00, 250.00, 125.00, 125.00,
            125.00, 125.00, 250.00, 250.00,
            // End of Repeat

            250.00, 125.00, 250.00, 250.00, 125.00, 250.00,
            125.00, 125.00, 125.00, 125.00, 125.00, 125.00, 125.00, 125.00, 250.00,
            250.00, 125.00, 250.00, 250.00, 125.00, 250.00,
            125.00, 125.00, 125.00, 125.00, 500.00,

            250.00, 125.00, 250.00, 250.00, 125.00, 250.00,
            125.00, 125.00, 125.00, 125.00, 125.00, 125.00, 125.00, 125.00, 250.00,
            250.00, 125.00, 250.00, 250.00, 125.00, 250.00,
            125.00, 125.00, 125.00, 125.00, 500.00};

        pwm->warning_melody_arr_size = sizeof(pirates_melody) / sizeof(pirates_melody[0]);
        set_melody(pwm, pirates_melody, pirates_note_durations, pwm->warning_melody_arr_size, choice);
    }
    // // Game of thrones
    else if (choice == 3)
    {
        int thrones_melody[] = {
            NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4,
            NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4,
            NOTE_G4, NOTE_C4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_E4, NOTE_F4,
            NOTE_G4, NOTE_C4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_E4, NOTE_F4,
            NOTE_G4, NOTE_C4,

            NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4,
            NOTE_D4,
            NOTE_F4, NOTE_AS3,
            NOTE_DS4, NOTE_D4, NOTE_F4, NOTE_AS3,
            NOTE_DS4, NOTE_D4, NOTE_C4,

            NOTE_G4, NOTE_C4,

            NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4,
            NOTE_D4,
            NOTE_F4, NOTE_AS3,
            NOTE_DS4, NOTE_D4, NOTE_F4, NOTE_AS3,
            NOTE_DS4, NOTE_D4, NOTE_C4,
            NOTE_G4, NOTE_C4,
            NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4,

            NOTE_D4,
            NOTE_F4, NOTE_AS3,
            NOTE_D4, NOTE_DS4, NOTE_D4, NOTE_AS3,
            NOTE_C4,
            NOTE_C5,
            NOTE_AS4,
            NOTE_C4,
            NOTE_G4,
            NOTE_DS4,
            NOTE_DS4, NOTE_F4,
            NOTE_G4,

            NOTE_C5,
            NOTE_AS4,
            NOTE_C4,
            NOTE_G4,
            NOTE_DS4,
            NOTE_DS4, NOTE_D4,
            NOTE_C5, NOTE_G4, NOTE_GS4, NOTE_AS4, NOTE_C5, NOTE_G4, NOTE_GS4, NOTE_AS4,
            NOTE_C5, NOTE_G4, NOTE_GS4, NOTE_AS4, NOTE_C5, NOTE_G4, NOTE_GS4, NOTE_AS4,

            NOTE_GS5, NOTE_AS5, NOTE_C6, NOTE_G5, NOTE_GS5, NOTE_AS5,
            NOTE_C6, NOTE_G5, NOTE_GS5, NOTE_AS5, NOTE_C6, NOTE_G5, NOTE_GS5, NOTE_AS5};
        int thrones_note_durations[] = {
            125.00, 125.00, 62.50, 62.50, 125.00, 125.00, 62.50, 62.50,
            125.00, 125.00, 62.50, 62.50, 125.00, 125.00, 62.50, 62.50,
            125.00, 125.00, 62.50, 62.50, 125.00, 125.00, 62.50, 62.50,
            125.00, 125.00, 62.50, 62.50, 125.00, 125.00, 62.50, 62.50,
            250.00, 250.00,

            62.50, 62.50, 250.00, 250.00, 62.50, 62.50,
            1000.00,
            250.00, 250.00,
            62.50, 62.50, 250.00, 250.00,
            62.50, 62.50, 1000.00,

            250.00, 250.00,

            62.50, 62.50, 250.00, 250.00, 62.50, 62.50,
            1000.00,
            250.00, 250.00,
            62.50, 62.50, 250.00, 250.00,
            62.50, 62.50,
            250.00, 250.00,

            500.00,
            250.00, 250.00,
            125.00, 125.00, 125.00, 125.00,
            1000.00,
            500.00,
            500.00,
            500.00,
            500.00,
            500.00,
            250.00, 250.00,
            1000.00,

            500.00,
            500.00,
            500.00,
            500.00,
            500.00,
            250.00, 250.00,
            125.00, 125.00, 62.50, 62.50, 125.00, 125.00,
            125.00, 125.00, 62.50, 62.50,
            125.00, 125.00, 62.50, 62.50,

            250.00, 62.50, 62.50, 125.00, 62.50, 125.00, 62.50, 125.00,
            62.50, 62.50, 62.50, 62.50};

        pwm->warning_melody_arr_size = sizeof(thrones_melody) / sizeof(thrones_melody[0]);
        set_melody(pwm, thrones_melody, thrones_note_durations, pwm->warning_melody_arr_size, choice);
    }
    // Star wars main theme
    else if (choice == 4)
    {

        int star_wars_melody[] = {
            NOTE_AS4, NOTE_AS4, NOTE_AS4,
            NOTE_F5, NOTE_C6,
            NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
            NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
            NOTE_AS5, NOTE_A5, NOTE_AS5, NOTE_G5, NOTE_C5, NOTE_C5, NOTE_C5,
            NOTE_F5, NOTE_C6,
            NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,

            NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
            NOTE_AS5, NOTE_A5, NOTE_AS5, NOTE_G5, NOTE_C5, NOTE_C5,
            NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,
            NOTE_F5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_C5,
            NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,

            NOTE_C6, NOTE_G5, NOTE_G5, NOTE_C5,
            NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,
            NOTE_F5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_D5, NOTE_E5, NOTE_C6, NOTE_C6,
            NOTE_F6, NOTE_DS6, NOTE_CS6, NOTE_C6, NOTE_AS5, NOTE_GS5, NOTE_G5, NOTE_F5,
            NOTE_C6};
        int star_wars_note_durations[] = {
            125.00, 125.00, 125.00,
            500.00, 500.00,
            125.00, 125.00, 125.00, 500.00, 250.00,
            125.00, 125.00, 125.00, 500.00, 250.00,
            125.00, 125.00, 125.00, 500.00, 125.00, 125.00, 125.00,
            500.00, 500.00,
            125.00, 125.00, 125.00, 500.00, 250.00,

            125.00, 125.00, 125.00, 500.00, 250.00,
            125.00, 125.00, 125.00, 500.00, 62.50,
            250.00, 125.00, 125.00, 125.00, 125.00,
            125.00, 125.00, 125.00, 250.00, 125.00, 250.00, 125.00, 62.50,
            250.00, 125.00, 125.00, 125.00, 125.00,

            125.00, 62.50, 500.00, 125.00, 125.00,
            250.00, 125.00, 125.00, 125.00, 125.00,
            125.00, 125.00, 125.00, 250.00, 125.00, 250.00, 125.00, 62.50,
            250.00, 125.00, 250.00, 125.00, 250.00, 125.00, 250.00, 125.00,
            1000.00};

        pwm->warning_melody_arr_size = sizeof(star_wars_melody) / sizeof(star_wars_melody[0]);
        set_melody(pwm, star_wars_melody, star_wars_note_durations, pwm->warning_melody_arr_size, choice);
    }
    // Super Mario Bros
    else if (choice == 5)
    {
        int super_melody[] = {
            NOTE_E5, NOTE_E5, NOTE_E5, NOTE_C5, NOTE_E5,
            NOTE_G5, NOTE_G4,
            NOTE_C5, NOTE_G4, NOTE_E4,
            NOTE_A4, NOTE_B4, NOTE_AS4, NOTE_A4,
            NOTE_G4, NOTE_E5, NOTE_G5, NOTE_A5, NOTE_F5, NOTE_G5,
            NOTE_E5, NOTE_C5, NOTE_D5, NOTE_B4,
            NOTE_C5, NOTE_G4, NOTE_E4,
            NOTE_A4, NOTE_B4, NOTE_AS4, NOTE_A4,
            NOTE_G4, NOTE_E5, NOTE_G5, NOTE_A5, NOTE_F5, NOTE_G5,
            NOTE_E5, NOTE_C5, NOTE_D5, NOTE_B4,

            NOTE_G5, NOTE_FS5, NOTE_F5, NOTE_DS5, NOTE_E5,
            NOTE_GS4, NOTE_A4, NOTE_C4, NOTE_A4, NOTE_C5, NOTE_D5,
            NOTE_DS5, NOTE_D5,
            NOTE_C5,

            NOTE_G5, NOTE_FS5, NOTE_F5, NOTE_DS5, NOTE_E5,
            NOTE_GS4, NOTE_A4, NOTE_C4, NOTE_A4, NOTE_C5, NOTE_D5,
            NOTE_DS5, NOTE_D5,
            NOTE_C5,

            NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_D5,
            NOTE_E5, NOTE_C5, NOTE_A4, NOTE_G4,

            NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_D5, NOTE_E5,

            NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_D5,
            NOTE_E5, NOTE_C5, NOTE_A4, NOTE_G4,
            NOTE_E5, NOTE_E5, NOTE_E5, NOTE_C5, NOTE_E5,
            NOTE_G5, NOTE_G4,
            NOTE_C5, NOTE_G4, NOTE_E4,

            NOTE_A4, NOTE_B4, NOTE_AS4, NOTE_A4,
            NOTE_G4, NOTE_E5, NOTE_G5, NOTE_A5, NOTE_F5, NOTE_G5,
            NOTE_E5, NOTE_C5, NOTE_D5, NOTE_B4,

            NOTE_C5, NOTE_G4, NOTE_E4,
            NOTE_A4, NOTE_B4, NOTE_AS4, NOTE_A4,
            NOTE_G4, NOTE_E5, NOTE_G5, NOTE_A5, NOTE_F5, NOTE_G5,
            NOTE_E5, NOTE_C5, NOTE_D5, NOTE_B4,

            NOTE_E5, NOTE_C5, NOTE_G4, NOTE_GS4,
            NOTE_A4, NOTE_F5, NOTE_F5, NOTE_A4,
            NOTE_D5, NOTE_A5, NOTE_A5, NOTE_A5, NOTE_G5, NOTE_F5,

            NOTE_E5, NOTE_C5, NOTE_A4, NOTE_G4,
            NOTE_E5, NOTE_C5, NOTE_G4, NOTE_GS4,
            NOTE_A4, NOTE_F5, NOTE_F5, NOTE_A4,
            NOTE_B4, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_D5,
            NOTE_C5, NOTE_E4, NOTE_E4, NOTE_C4,

            NOTE_E5, NOTE_C5, NOTE_G4, NOTE_GS4,
            NOTE_A4, NOTE_F5, NOTE_F5, NOTE_A4,
            NOTE_D5, NOTE_A5, NOTE_A5, NOTE_A5, NOTE_G5, NOTE_F5,

            NOTE_E5, NOTE_C5, NOTE_A4, NOTE_G4,
            NOTE_E5, NOTE_C5, NOTE_G4, NOTE_GS4,
            NOTE_A4, NOTE_F5, NOTE_F5, NOTE_A4,
            NOTE_B4, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_D5,
            NOTE_C5, NOTE_E4, NOTE_E4, NOTE_C4,
            NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_D5, NOTE_E5,

            NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_D5,
            NOTE_E5, NOTE_C5, NOTE_A4, NOTE_G4,
            NOTE_E5, NOTE_E5, NOTE_E5, NOTE_C5, NOTE_E5,
            NOTE_G5, NOTE_G4,
            NOTE_E5, NOTE_C5, NOTE_G4, NOTE_GS4,
            NOTE_A4, NOTE_F5, NOTE_F5, NOTE_A4,
            NOTE_D5, NOTE_A5, NOTE_A5, NOTE_A5, NOTE_G5, NOTE_F5,

            NOTE_E5, NOTE_C5, NOTE_A4, NOTE_G4,
            NOTE_E5, NOTE_C5, NOTE_G4, NOTE_GS4,
            NOTE_A4, NOTE_F5, NOTE_F5, NOTE_A4,
            NOTE_B4, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_D5,
            NOTE_C5, NOTE_E4, NOTE_E4, NOTE_C4,

            // Game over sound
            NOTE_C5, NOTE_G4, NOTE_E4,
            NOTE_A4, NOTE_B4, NOTE_A4, NOTE_GS4, NOTE_AS4, NOTE_GS4,
            NOTE_G4, NOTE_D4, NOTE_E4};
        int super_note_durations[] = {
            125, 125, 125, 125, 125, 125, 125,
            250, 250, 125, 250,
            250, 125, 250, 250,
            250, 250, 125, 250,
            125, 125, 125, 250, 125, 125,
            125, 250, 125, 125, 250,
            250, 125, 250, 250,
            250, 250, 125, 250,
            125, 125, 125, 250, 125, 125,
            125, 250, 125, 125,

            250, 125, 125, 125, 250, 125,
            125, 125, 125, 125, 125, 125, 125, 125,
            250, 250, 125, 250,
            500, 500,

            250, 125, 125, 125, 250, 125,
            125, 125, 125, 125, 125, 125, 125, 125,
            250, 250, 125, 250,
            500, 500,

            125, 250, 125, 125, 125, 250,
            125, 250, 125, 500,

            125, 250, 125, 125, 125, 125, 125,
            1000,
            125, 250, 125, 125, 125, 250,
            125, 250, 125, 500,
            125, 125, 125, 125, 125, 125, 250,
            250, 250, 250, 250,
            250, 125, 250, 250,

            250, 250, 125, 250,
            125, 125, 125, 250, 125, 125,
            125, 250, 125, 250,

            250, 125, 250, 250,
            250, 250, 125, 250,
            125, 125, 125, 250, 125, 125,
            125, 250, 125, 250,

            125, 250, 125, 125,
            125, 250, 125, 500,
            125, 125, 125, 125, 125, 125,

            125, 250, 125, 500,
            125, 250, 125, 250,
            125, 250, 125, 125, 125, 125,
            125, 250, 125, 500,

            250, 250, 250,
            125, 125, 125, 125, 125, 125,
            125, 125, 500};

        pwm->warning_melody_arr_size = sizeof(super_melody) / sizeof(super_melody[0]);
        set_melody(pwm, super_melody, super_note_durations, pwm->warning_melody_arr_size, choice);
    }
}
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
            if (pwm->noteIndex >= pwm->warning_melody_arr_size)
            // if (pwm->noteIndex >= sizeof(pwm->warning_melody_arr) / sizeof(pwm->warning_melody_arr[0]))
            {
                pwm->next_state = PWM_OFF;
                break;
            }
            ledc_set_freq(pwm->speed_mode, pwm->timer_num, pwm->warning_melody_arr[pwm->noteIndex]);
            ledc_set_duty(pwm->speed_mode, pwm->channel, 2048); // 50% duty cycle (4096/2)
            ledc_update_duty(pwm->speed_mode, pwm->channel);

            pwm->note_playing = true;
        }
        if ((current_time - pwm->stateChangeTime) >= pdMS_TO_TICKS(pwm->warning_note_durations_arr[pwm->noteIndex]))
        {
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(pwm->speed_mode, pwm->channel, 0));
            ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(pwm->speed_mode, pwm->channel));
            pwm->stateChangeTime = current_time;
            pwm->next_state = PWM_MELODY_PLAYING_DELAY;
        }
        break;
    case PWM_MELODY_PLAYING_DELAY:
        if ((current_time - pwm->stateChangeTime) >= pdMS_TO_TICKS(pwm->warning_note_durations_arr[pwm->noteIndex]))
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
void pwm_free(pwm_component_t *pwm)
{
    if (pwm != NULL)
    {
        vPortFree(pwm);
        pwm = NULL;
    }
}