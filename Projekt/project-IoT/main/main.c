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

// lv_obj_clean(src);
void potThresholdCallback(int value)
{
    ESP_LOGI(TAG, "Rising Edge %d", value);
}

void photoresistorCallback(int value)
{
    // ESP_LOGI(TAG, "Photoresistor callback %d", value);
}

void play_melody(pwm_component_t *buzzer)
{

    int alarmMelody[] = {392, 392, 392, 311, 466, 392, 311, 466, 392,    // G4, G4, G4, D#4, A#4, G4, D#4, A#4, G4
                         587, 587, 587, 622, 466, 370, 311, 466, 392,    // D5, D5, D5, D#5, A#4, G4, D#4, A#4, G4
                         784, 392, 392, 784, 739, 698, 659, 622, 659,    // G5, G4, G4, G5, F#5, F5, E5, D#5, E5
                         466, 622, 587, 466, 622, 587, 466, 622, 587,    // A#4, D#5, D5, A#4, D#5, D5, A#4, D#5, D5
                         466, 392, 311, 466, 392, 311, 466, 392, 311};   // A#4, G4, D#4, A#4, G4, D#4, A#4, G4, D#4};   // G4, G4, G4, F4, G4, C5, C5, C5, C5, G4, G4, G4, F4, G4, C5, C5, C5, G4
    int noteDurations[] = {500, 500, 500, 350, 150, 500, 350, 150, 1000, // Durations for the first part
                           500, 500, 500, 350, 150, 500, 350, 150, 1000, // Durations for the second part
                           500, 350, 150, 500, 350, 150, 500, 350, 150,  // Durations for the third part
                           1000, 500, 350, 150, 500, 350, 150, 500, 350, // Durations for the fourth part
                           150, 500, 350, 150, 500, 350, 150, 500, 350}; // Durations for the fifth part}; // Each note lasts 500 ms

    for (int i = 0; i < sizeof(alarmMelody) / sizeof(alarmMelody[0]); i++)
    { // Manually set frequency and duty cycle
        ledc_set_freq(LEDC_MODE, LEDC_TIMER, alarmMelody[i]);
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 2048); // 50% duty cycle (4096/2)
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

        vTaskDelay(pdMS_TO_TICKS(noteDurations[i]));

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

    adc_setOnThreshold(pot, POT_THRESHOLD, true, false, potThresholdCallback, NULL);

    int period = 10;

    onboard_rgb_led_t *rgb_led = rgb_led_init(RGB_LED_STATE, NULL, RGB_LED_PIN);
    rgb_led_configure(rgb_led);
    display_init();

    TickType_t last_wake_time = xTaskGetTickCount();

    while (1)
    {
        adc_update(photoresistor);
        adc_update(pot);
        int currentValue = adc_getValue(pot);
        ESP_LOGI("MAIN", "Potentiometer Value: %d", currentValue);
        adc_setOnThreshold(photoresistor, currentValue, true, false, photoresistorCallback, NULL);

        int currentValueSens = adc_getValue(photoresistor);

        pwm_update(buzzer);

        rgb_led_update_buffer(rgb_led, currentValueSens);
        int test = currentValue;
        int close = currentValue - 400;

        if (xTaskGetTickCount() - last_wake_time >= pdMS_TO_TICKS(CHECK_INTERVAL_MS))
        {
            last_wake_time = xTaskGetTickCount();
            int averageValue = rgb_led_buffer_average(rgb_led);
            ESP_LOGI("MAIN", "Average Sensor Value: %d", averageValue);
            if (averageValue <= currentValue)
            {
                play_melody(buzzer);
                rgb_led->led_state = 1;
            }
            else if (averageValue <= close)
            {
                rgb_led->led_state = 2;
            }
            else
            {
                pwm_set(buzzer, 0);
                rgb_led->led_state = 0;
            }
            rgb_led_set_state(rgb_led);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
