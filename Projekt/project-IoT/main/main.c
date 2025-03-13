#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "passive_buzzer_component.h"
#include "photoresistor_component.h"
#include "potentiometer_comp.h"
#include "display_component.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "rgb_component.h"
#include "analog_led_component.h"

#define TAG "MAIN"
#define POT_THRESHOLD 3000
#define POT_CHANNEL ADC_CHANNEL_2
#define POT_PIN GPIO_NUM_2
#define POT_ADC_UNIT 0

#define PHOTORESISTOR_CHANNEL ADC_CHANNEL_4
#define PHOTORESISTOR_PIN GPIO_NUM_4
#define PHOTORESISTOR_ADC_UNIT 1

#define BUZZER_PIN GPIO_NUM_18
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY 5000
#define BUZZER_HERTZ 1000

#define RGB_LED_STATE 0
#define RGB_LED_PIN GPIO_NUM_8
#define CHECK_INTERVAL_MS 2000

void photoresistorCallback(int value)
{
    // ESP_LOGI(TAG, "Photoresistor callback %d", value);
}

// TODO - Ge användaren ett val av olika melodier
// TODO Gör så att det går att stoppa melodin när state ändras.
void play_melody(pwm_component_t *buzzer, onboard_rgb_led_t *rgb_led)
{
    // ESP_LOGI(TAG, "STATE : %d", rgb_led->led_state);
    int alarm_melody[45];
    int note_durations[45];
    if (rgb_led->led_state == 1)
    {
        int warning_melody[] = {392, 392, 392, 311, 466, 392, 311, 466, 392,          // G4, G4, G4, D#4, A#4, G4, D#4, A#4, G4
                                587, 587, 587, 622, 466, 370, 311, 466, 392,          // D5, D5, D5, D#5, A#4, G4, D#4, A#4, G4
                                784, 392, 392, 784, 739, 698, 659, 622, 659,          // G5, G4, G4, G5, F#5, F5, E5, D#5, E5
                                466, 622, 587, 466, 622, 587, 466, 622, 587,          // A#4, D#5, D5, A#4, D#5, D5, A#4, D#5, D5
                                466, 392, 311, 466, 392, 311, 466, 392, 311};         // A#4, G4, D#4, A#4, G4, D#4, A#4, G4, D#4};   // G4, G4, G4, F4, G4, C5, C5, C5, C5, G4, G4, G4, F4, G4, C5, C5, C5, G4
        int warning_note_durations[] = {500, 500, 500, 350, 150, 500, 350, 150, 1000, // Durations for the first part
                                        500, 500, 500, 350, 150, 500, 350, 150, 1000, // Durations for the second part
                                        500, 350, 150, 500, 350, 150, 500, 350, 150,  // Durations for the third part
                                        1000, 500, 350, 150, 500, 350, 150, 500, 350, // Durations for the fourth part
                                        150, 500, 350, 150, 500, 350, 150, 500, 350}; // Durations for the fifth part}; // Each note lasts 500 ms

        // Star Wars - "Imperial March"

        // Use memcpy to copy the contents of warning_melody to alarmMelody
        memcpy(alarm_melody, warning_melody, sizeof(warning_melody));
        memcpy(note_durations, warning_note_durations, sizeof(warning_note_durations));
    }
    else
    {
        int close_alarm_melody[] = {392, 392, 392, 311, 466, 392, 311, 466, 392,        // G4, G4, G4, D#4, A#4, G4, D#4, A#4, G4
                                    587, 587, 587, 622, 466, 370, 311, 466, 392,        // D5, D5, D5, D#5, A#4, G4, D#4, A#4, G4
                                    784, 392, 392, 784, 739, 698, 659, 622, 659,        // G5, G4, G4, G5, F#5, F5, E5, D#5, E5
                                    466, 622, 587, 466, 622, 587, 466, 622, 587,        // A#4, D#5, D5, A#4, D#5, D5, A#4, D#5, D5
                                    466, 392, 311, 466, 392, 311, 466, 392, 311};       // A#4, G4, D#4, A#4, G4, D#4, A#4, G4, D#4};   // G4, G4, G4, F4, G4, C5, C5, C5, C5, G4, G4, G4, F4, G4, C5, C5, C5, G4
        int close_note_durations[] = {1000, 1000, 1000, 700, 300, 1000, 700, 300, 2000, // Durations for the first part
                                      1000, 1000, 1000, 700, 300, 1000, 700, 300, 2000, // Durations for the second part
                                      1000, 700, 300, 1000, 700, 300, 1000, 700, 300,   // Durations for the third part
                                      2000, 1000, 700, 300, 1000, 700, 300, 1000, 700,  // Durations for the fourth part
                                      300, 1000, 700, 300, 1000, 700, 300, 1000, 700};  // Durations for the fifth part

        // Use memcpy to copy the contents of warning_melody to alarmMelody
        memcpy(alarm_melody, close_alarm_melody, sizeof(close_alarm_melody));
        memcpy(note_durations, close_note_durations, sizeof(close_note_durations));
    }

    for (int i = 0; i < sizeof(alarm_melody) / sizeof(alarm_melody[0]); i++)
    {
        // Manually set frequency and duty cycle
        // if (rgb_led->led_state == 0)
        // {
        //     break;
        // }
        ledc_set_freq(LEDC_MODE, LEDC_TIMER, alarm_melody[i]);
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 2048); // 50% duty cycle (4096/2)
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

        vTaskDelay(pdMS_TO_TICKS(note_durations[i]));

        // Turn off the buzzer between notes
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

        vTaskDelay(pdMS_TO_TICKS(100)); // Short pause between notes
    }
}

void app_main(void)
{

    gpio_set_level(GPIO_NUM_8, 0);

    adc_t *pot = adc_init(POT_PIN, POT_CHANNEL, POT_ADC_UNIT);
    if (pot == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize potentiometer ADC");
        return;
    }
    adc_t *photoresistor = adc_init(PHOTORESISTOR_PIN, PHOTORESISTOR_CHANNEL, PHOTORESISTOR_ADC_UNIT);
    if (photoresistor == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize photoresistor ADC");
        return;
    }
    pwm_component_t *buzzer = pwm_init(BUZZER_PIN, BUZZER_HERTZ, LEDC_MODE, LEDC_TIMER, LEDC_CHANNEL, 4096, 1000);

    adc_setOnThreshold(pot, POT_THRESHOLD, true, false, NULL, NULL);

    int period = 10;

    // TODO - Fixa rgb led c och h filen
    onboard_rgb_led_t *rgb_led = rgb_led_init(RGB_LED_STATE, NULL, RGB_LED_PIN);
    rgb_led_configure(rgb_led);
    display_component_t *new_display = display_init();

    TickType_t last_check_time = xTaskGetTickCount();

    while (1)
    {
        adc_update(photoresistor);
        adc_update(pot);
        int pot_value = adc_getValue(pot);
        // TODO - Anpassa koden till callbacksen
        adc_setOnThreshold(photoresistor, pot_value, true, false, photoresistorCallback, NULL);

        int current_sensor_value = adc_getValue(photoresistor);

        pwm_update(buzzer);

        rgb_led_update_buffer(rgb_led, current_sensor_value);

        if (xTaskGetTickCount() - last_check_time >= pdMS_TO_TICKS(CHECK_INTERVAL_MS))
        {
            last_check_time = xTaskGetTickCount();
            int average_value = rgb_led_buffer_average(rgb_led);
            int test = pot_value - 400;
            if (average_value >= pot_value)
            {
                rgb_led->led_state = 1;
                ESP_LOGI("MAIN", "LED State set to 1 (Red)");
                display_ui(new_display, 1, average_value, pot_value);
                rgb_led_set_state(rgb_led);
                play_melody(buzzer, rgb_led);
            }
            else if (average_value >= test)
            {
                rgb_led->led_state = 2;
                ESP_LOGI("MAIN", "LED State set to 2 (Yellow)");
                display_ui(new_display, 2, average_value, pot_value);
                rgb_led_set_state(rgb_led);
                play_melody(buzzer, rgb_led);
            }
            else
            {
                display_ui(new_display, 0, average_value, pot_value);
                rgb_led->led_state = 0;
                pwm_set(buzzer, 0);
                ESP_LOGI("MAIN", "LED State set to 0 (Off)");
                rgb_led_set_state(rgb_led);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    display_free(new_display);
}
