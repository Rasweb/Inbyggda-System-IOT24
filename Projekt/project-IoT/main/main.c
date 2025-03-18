#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "potentiometer_comp.h"
#include "display_component.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "rgb_component.h"
#include "pwm_component.h"

#define TAG "MAIN"

// Pot component
#define POT_THRESHOLD 3000
#define POT_CHANNEL ADC_CHANNEL_2
#define POT_PIN GPIO_NUM_2
#define POT_ADC_UNIT 0
#define POT_BUFFER_SIZE 10

// Sensor component
#define PHOTORESISTOR_CHANNEL ADC_CHANNEL_4
#define PHOTORESISTOR_PIN GPIO_NUM_4
#define PHOTORESISTOR_ADC_UNIT 1
#define PHOTORESISTOR_BUFFER_SIZE 10

// Buzzer component
#define BUZZER_PIN GPIO_NUM_18
#define BUZZER_LEDC_TIMER LEDC_TIMER_0
#define BUZZER_LEDC_MODE LEDC_LOW_SPEED_MODE
#define BUZZER_LEDC_CHANNEL LEDC_CHANNEL_0
#define BUZZER_HERTZ 1000

// Onboard rgb led component
#define RGB_LED_STATE 0
#define RGB_LED_PIN GPIO_NUM_8
#define CHECK_INTERVAL_MS 2000

// Display component
#define DISPLAY_PIN_NUM_SDA 6
#define DISPLAY_PIN_NUM_SCL 7

void app_main(void)
{
    // Onboard rgb led
    gpio_set_level(RGB_LED_PIN, 0);

    // Pot component
    adc_t *pot = adc_init(POT_PIN, POT_CHANNEL, POT_ADC_UNIT, POT_BUFFER_SIZE);
    if (pot == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize potentiometer ADC");
        return;
    }

    // Sensor component
    adc_t *photoresistor = adc_init(PHOTORESISTOR_PIN, PHOTORESISTOR_CHANNEL, PHOTORESISTOR_ADC_UNIT, PHOTORESISTOR_BUFFER_SIZE);
    if (photoresistor == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize photoresistor ADC");
        return;
    }

    // Buzzer component
    pwm_component_t *buzzer = pwm_init(BUZZER_PIN, BUZZER_HERTZ, BUZZER_LEDC_MODE, BUZZER_LEDC_TIMER, BUZZER_LEDC_CHANNEL, 4096, 1000);
    if (buzzer == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize buzzer PWM");
    }

    // Pot
    adc_setOnThreshold(pot, POT_THRESHOLD, true, false, NULL, NULL);

    // RGB
    onboard_rgb_led_t *rgb_led = rgb_led_init(RGB_LED_STATE, NULL, RGB_LED_PIN);
    rgb_led_configure(rgb_led);

    // Display
    display_component_t *new_display = display_init(DISPLAY_PIN_NUM_SDA, DISPLAY_PIN_NUM_SCL);

    TickType_t last_check_time = xTaskGetTickCount();
    while (1)
    {
        // Sensor and pot update
        adc_update(photoresistor);
        adc_update(pot);

        // Get Pot value and use it in sensor
        int current_pot_value = adc_getValue(pot);
        adc_update_buffer(pot, current_pot_value);
        int average_pot_value = adc_buffer_average(pot);
        adc_setOnThreshold(photoresistor, average_pot_value, true, false, NULL, NULL);

        int current_sensor_value = adc_getValue(photoresistor);

        pwm_update(buzzer);
        adc_update_buffer(photoresistor, current_sensor_value);

        // Check if time has passed acording to CHECK_INTERVAL_MS macro
        if (xTaskGetTickCount() - last_check_time >= pdMS_TO_TICKS(CHECK_INTERVAL_MS))
        {
            last_check_time = xTaskGetTickCount();
            int average_value = adc_buffer_average(photoresistor);

            int close = average_pot_value - 400;
            if (average_value >= average_pot_value)
            {
                rgb_led->led_state = 1;
                ESP_LOGI("MAIN", "LED State set to 1 (Alert)");
                display_ui(new_display, 1, average_value, average_pot_value);
                rgb_led_set_state(rgb_led);
                pwm_set_melody(buzzer, 2048, PWM_MELODY_PLAYING);
            }
            else if (average_value >= close)
            {
                rgb_led->led_state = 2;
                display_ui(new_display, 2, average_value, average_pot_value);
                ESP_LOGI("MAIN", "LED State set to 2 (Rising)");
                rgb_led_set_state(rgb_led);
                pwm_set_melody(buzzer, 0, PWM_OFF);
            }
            else
            {
                display_ui(new_display, 0, average_value, average_pot_value);
                rgb_led->led_state = 0;
                ESP_LOGI("MAIN", "LED State set to 0 (Safe)");
                rgb_led_set_state(rgb_led);
                pwm_set_melody(buzzer, 0, PWM_OFF);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    adc_destroy(pot);
    adc_destroy(photoresistor);
    rgb_led_free(rgb_led);
    pwm_free(buzzer);
    display_free(new_display);
}
