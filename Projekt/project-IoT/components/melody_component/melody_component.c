#include "melody_component.h"

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

melody_component_t *melody_init()
{
    melody_component_t *new_melody = pvPortMalloc(sizeof(melody_component_t));
    new_melody->state = MELODY_OFF;
    new_melody->miliseconds_between_notes = 0;
    new_melody->miliseconds_note_break = 0;
    new_melody->duration = 0;
    new_melody->last_tick = 0;
    new_melody->note_playing = false;
    new_melody->noteIndex = 0;

    return new_melody;
}

// void playNote(pwm_component_t *buzzer, int frequency, int duration)
// {
//     ledc_set_freq(buzzer->speed_mode, buzzer->timer_num, frequency);
//     ledc_set_duty(buzzer->speed_mode, buzzer->channel, 2048); // 50% duty
//     ledc_update_duty(buzzer->speed_mode, buzzer->channel);
//     vTaskDelay(pdMS_TO_TICKS(duration));

//     // Stäng av buzzer
//     ledc_set_duty(buzzer->speed_mode, buzzer->channel, 0);
//     ledc_update_duty(buzzer->speed_mode, buzzer->channel);

//     // Paus mellan noter
//     vTaskDelay(pdMS_TO_TICKS(100));
// }

void set_melody_state(melody_component_t *melody, melodyState state, int duration)
{
    melody->duration = duration;
    melody->state = state;
}

void update_melody(melody_component_t *melody, pwm_component_t *buzzer)
{

    // TickType_t last_tick = 0;

    // ESP_LOGI("RGB", "feq: %d", melody->frequency);

    TickType_t current_time = xTaskGetTickCount();
    switch (melody->state)
    {

    case MELODY_OFF:
        ledc_set_duty(buzzer->speed_mode, buzzer->channel, 0);
        ledc_update_duty(buzzer->speed_mode, buzzer->channel);
        melody->note_playing = false;
        melody->noteIndex = 0;
        melody->next_state = MELODY_OFF;
        break;
    case MELODY_PLAYING:
        if (!melody->note_playing)
        {
            ESP_LOGI("MELODY", "Playing Index in pausing if: %d, Frequency: %d", melody->noteIndex, warning_melody[melody->noteIndex]);
            ledc_set_freq(buzzer->speed_mode, buzzer->timer_num, warning_melody[melody->noteIndex]);
            ledc_set_duty(buzzer->speed_mode, buzzer->channel, 2048); // 50% duty cycle (4096/2)
            ledc_update_duty(buzzer->speed_mode, buzzer->channel);

            melody->last_tick = current_time;
            melody->note_playing = true;
        }
        if ((current_time - melody->last_tick) >= pdMS_TO_TICKS(warning_note_durations[melody->noteIndex]))
        {
            ESP_LOGI("MELODY", "IN second if of playing");
            melody->next_state = MELODY_PLAYING_DELAY;
            melody->note_playing = true;
        }
        else
        {
            ESP_LOGI("MELODY", "IN BOTTOM of playin");
            melody->next_state = MELODY_PLAYING;
        }
        break;
    case MELODY_PLAYING_DELAY:
        if ((current_time - melody->last_tick) >= pdMS_TO_TICKS(warning_note_durations[melody->noteIndex]))
        {
            ESP_LOGI("MELODY", "delay Index: %d", melody->noteIndex);
            ledc_set_duty(buzzer->speed_mode, buzzer->channel, 0);
            ledc_update_duty(buzzer->speed_mode, buzzer->channel);
            melody->noteIndex++;
            melody->note_playing = false;
            melody->next_state = MELODY_PLAYING;
        }
        melody->next_state = MELODY_PLAYING_DELAY;
        break;
    default:
        break;
    }
    melody->pre_state = melody->state;
    melody->state = melody->next_state;
}
// Spela en not

//         void play_melody(pwm_component_t *buzzer, onboard_rgb_led_t *rgb_led)
// {

//     int alarm_melody[45];
//     int note_durations[45];
//     if (rgb_led->led_state == 1)
//     {
//         int warning_melody[] = {392, 392, 392, 311, 466, 392, 311, 466, 392,          // G4, G4, G4, D#4, A#4, G4, D#4, A#4, G4
//                                 587, 587, 587, 622, 466, 370, 311, 466, 392,          // D5, D5, D5, D#5, A#4, G4, D#4, A#4, G4
//                                 784, 392, 392, 784, 739, 698, 659, 622, 659,          // G5, G4, G4, G5, F#5, F5, E5, D#5, E5
//                                 466, 622, 587, 466, 622, 587, 466, 622, 587,          // A#4, D#5, D5, A#4, D#5, D5, A#4, D#5, D5
//                                 466, 392, 311, 466, 392, 311, 466, 392, 311};         // A#4, G4, D#4, A#4, G4, D#4, A#4, G4, D#4};   // G4, G4, G4, F4, G4, C5, C5, C5, C5, G4, G4, G4, F4, G4, C5, C5, C5, G4
//         int warning_note_durations[] = {500, 500, 500, 350, 150, 500, 350, 150, 1000, // Durations for the first part
//                                         500, 500, 500, 350, 150, 500, 350, 150, 1000, // Durations for the second part
//                                         500, 350, 150, 500, 350, 150, 500, 350, 150,  // Durations for the third part
//                                         1000, 500, 350, 150, 500, 350, 150, 500, 350, // Durations for the fourth part
//                                         150, 500, 350, 150, 500, 350, 150, 500, 350}; // Durations for the fifth part}; // Each note lasts 500 ms

//         // Star Wars - "Imperial March"

//         // Use memcpy to copy the contents of warning_melody to alarmMelody
//         memcpy(alarm_melody, warning_melody, sizeof(warning_melody));
//         memcpy(note_durations, warning_note_durations, sizeof(warning_note_durations));
//     }

//     for (int i = 0; i < sizeof(alarm_melody) / sizeof(alarm_melody[0]); i++)
//     {
//         ledc_set_freq(LEDC_MODE, LEDC_TIMER, alarm_melody[i]);
//         ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 2048); // 50% duty cycle (4096/2)
//         ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

//         vTaskDelay(pdMS_TO_TICKS(note_durations[i]));

//         // Turn off the buzzer between notes
//         ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
//         ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

//         vTaskDelay(pdMS_TO_TICKS(100)); // Short pause between notes
//     }
// }