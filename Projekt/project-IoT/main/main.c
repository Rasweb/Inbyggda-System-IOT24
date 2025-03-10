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

void app_main(void)
{

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
        if (currentValueSens <= 300)
        {
            pwm_sine(buzzer, period);
        }
        else
        {
            pwm_set(buzzer, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }

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
}
// gpio_set_level(RGB_LED_PIN, 0);
// while (1)
// {

//     set_rgb_led(255, 0, 0); // Set the LED to red
//     vTaskDelay(pdMS_TO_TICKS(100));

//     set_rgb_led(0, 255, 0); // Set the LED to green
//     vTaskDelay(pdMS_TO_TICKS(100));

//     set_rgb_led(0, 0, 255); // Set the LED to blue
//     vTaskDelay(pdMS_TO_TICKS(100));

//     // setColor(255, 0, 0); // RED?
//     // vTaskDelay(pdMS_TO_TICKS(100));

//     // setColor(0, 255, 0); // GREEN?
//     // vTaskDelay(pdMS_TO_TICKS(100));

//     // setColor(0, 0, 255); // BLUE?
//     // vTaskDelay(pdMS_TO_TICKS(100));
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
