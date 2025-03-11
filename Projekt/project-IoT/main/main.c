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

#define RGB_LED_PIN GPIO_NUM_8

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

// För display
// https://github.com/espressif/esp-idf/tree/v5.2.5/examples/peripherals/lcd/i2c_oled

/*
inbyggd rgb led
passive buzzer - ljud
photoresistor - ljusnivå - adc/analog
potentiometer
display - ssd1306
*/

void potThresholdCallback(int value)
{
    ESP_LOGI(TAG, "Rising Edge %d", value);
}

void photoresistorCallback(int value)
{
    ESP_LOGI(TAG, "Photoresistor callback %d", value);
}

void play_melody(pwm_component_t *buzzer)
{
    int alarmMelody[] = {700, 900, 700, 900, 700, 900, 700, 900};   // G5, A5, G5, A5, G5, A5, G5, A5 notes
    int noteDurations[] = {200, 200, 200, 200, 200, 200, 200, 200}; // Duration of each note in milliseconds

    for (int i = 0; i < sizeof(alarmMelody) / sizeof(alarmMelody[0]); i++)
    {
        // Manually set frequency and duty cycle
        ledc_set_freq(LEDC_MODE, LEDC_TIMER, alarmMelody[i]);
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 2048); // 50% duty cycle (4096/2)
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

        vTaskDelay(pdMS_TO_TICKS(noteDurations[i]));

        // Turn off the buzzer between notes
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

        vTaskDelay(pdMS_TO_TICKS(10)); // Short pause between notes
        /* code */
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

    while (1)
    {

        adc_update(photoresistor);
        adc_update(pot);
        int currentValue = adc_getValue(pot);
        // ESP_LOGI("MAIN", "Potentiometer Value: %d", currentValue);
        adc_setOnThreshold(photoresistor, currentValue, true, false, photoresistorCallback, NULL);

        int currentValueSens = adc_getValue(photoresistor);
        ESP_LOGI("MAIN", "Sensor Value: %d", currentValueSens);
        pwm_update(buzzer);
        rgb_led_update(rgb_led);
        if (currentValueSens <= 800)
        {
            play_melody(buzzer);
            rgb_led->led_state = 1;
        }
        else
        {
            pwm_set(buzzer, 0);
            rgb_led->led_state = 0;
            // rgb_led_update(rgb_led);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// while (1)
// {

// }

/*
// Potentiometer
potentiometer *pot1 = pot_init(POT_PIN, POT_CHANNEL);

pot_setOnThreshold(pot1, POT_THRESHOLD, true, false, onThresholdCallback, NULL);

while (1)
{
   pot_update(pot1);
   int currentValue = pot_getValue(pot1);
   // ESP_LOGI("MAIN", "Potentiometer Värde: %d", currentValue);
   vTaskDelay(pdMS_TO_TICKS(100));
}
*/
