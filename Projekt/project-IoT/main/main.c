#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "potentiometer_comp.h"
#include "button_component.h"
#include "display_component.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "rgb_component.h"
#include "pwm_component.h"
#include "esp_sleep.h"
#include "ulp_lp_core.h"
#include "main_macro.h"

extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[] asm("_binary_ulp_main_bin_end");

void ulp_program();
void test_callback(int pin, void *arg);
void loop();
void FreeForAll(adc_t *pot, adc_t *photoresistor, pwm_component_t *buzzer, onboard_rgb_led_t *rgb_led, display_component_t *display);
void app_main(void)
{
    // Init Components
    adc_t *pot = adc_init(POT_PIN, POT_CHANNEL, POT_ADC_UNIT, POT_BUFFER_SIZE);

    if (pot == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize potentiometer");
    }

    adc_t *photoresistor = adc_init(PHOTORESISTOR_PIN, PHOTORESISTOR_CHANNEL, PHOTORESISTOR_ADC_UNIT, PHOTORESISTOR_BUFFER_SIZE);
    if (photoresistor == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize photoresistor");
    }
    pwm_component_t *buzzer = pwm_init(BUZZER_PIN, BUZZER_HERTZ, BUZZER_LEDC_MODE, BUZZER_LEDC_TIMER, BUZZER_LEDC_CHANNEL, 4096, 1000);
    if (buzzer == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize buzzer");
    }
    onboard_rgb_led_t *rgb_led = rgb_led_init(RGB_LED_STATE, NULL, RGB_LED_PIN);
    if (rgb_led == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize rgb_led");
    }
    display_component_t *display = display_init(DISPLAY_PIN_NUM_SDA, DISPLAY_PIN_NUM_SCL);
    if (display == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize display");
    }

    gpio_set_level(RGB_LED_PIN, 0);
    button_component *btn = btn_init(BTN_PIN, BTN_PULL_DOWN, BTN_PULL_UP, BTN_INTR_TYPE);
    btn_setOnPressed(btn, test_callback, (void *)buzzer);

    adc_setOnThreshold(pot, POT_THRESHOLD, true, false, NULL, NULL);
    rgb_led_configure(rgb_led);

    TickType_t last_check_time = xTaskGetTickCount();
    TickType_t below_threshold_start_time = xTaskGetTickCount();

    choose_melody(buzzer, 1);

    /* code */
    loop(photoresistor, pot, buzzer, btn, last_check_time, rgb_led, display, below_threshold_start_time);

    FreeForAll(pot, photoresistor, buzzer, rgb_led, display);
}

void loop(adc_t *photoresistor, adc_t *pot, pwm_component_t *buzzer, button_component *btn, TickType_t last_check_time, onboard_rgb_led_t *rgb_led, display_component_t *display, TickType_t below_threshold_start_time)
{
    while (1)
    {

        // esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
        // /* not a wakeup from ULP, load the firmware */
        // if (cause != ESP_SLEEP_WAKEUP_ULP)
        // {
        //     printf("Not a ULP wakeup, initializing it! \n");
        //     ulp_program();
        // }

        // /* ULP read and detected a change in GPIO_0, prints */
        // if (cause == ESP_SLEEP_WAKEUP_ULP)
        // {
        //     uint32_t adc_value = RTC_SLOW_MEM[0];

        //     printf("ULP woke up the main CPU! \n");
        // }

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
        btn_update(btn);

        // esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

        // Check if time has passed acording to CHECK_INTERVAL_MS macro
        if (xTaskGetTickCount() - last_check_time >= pdMS_TO_TICKS(CHECK_INTERVAL_MS))
        {
            last_check_time = xTaskGetTickCount();
            int average_sensor_value = adc_buffer_average(photoresistor);

            int close = average_pot_value - PHOTORESISTOR_CLOSE_VALUE;
            if (average_sensor_value >= average_pot_value)
            {
                rgb_led->led_state = 1;
                ESP_LOGI(TAG, "LED State set to 1 (Alert)");
                display_ui(display, 1, average_sensor_value, average_pot_value, false);
                rgb_led_set_state(rgb_led);
                if (average_sensor_value >= average_pot_value + BUZZER_TRIGGER_VALUE)
                {
                    pwm_set_melody(buzzer, 2048, PWM_MELODY_PLAYING);
                }
            }
            else if (average_sensor_value >= close)
            {
                rgb_led->led_state = 2;
                display_ui(display, 2, average_sensor_value, average_pot_value, false);
                ESP_LOGI(TAG, "LED State set to 2 (Rising)");
                rgb_led_set_state(rgb_led);
                pwm_set_melody(buzzer, 0, PWM_OFF);
            }
            else
            {
                rgb_led->led_state = 0;
                rgb_led_set_state(rgb_led);
                pwm_set_melody(buzzer, 0, PWM_OFF);

                /*
                    Funkar med sleep och timer enligt projektbeskrivningen.
                    Ska denna användas då behöver man inte vissa displayen när statusen är safe för energibesparning
                */
                // För sleep - funkar ganska bra, display:en
                if (xTaskGetTickCount() - below_threshold_start_time >= pdMS_TO_TICKS(SLEEP_TIMEOUT_MS))
                {
                    // display_ui(display, 0, average_sensor_value, average_pot_value, true);
                    below_threshold_start_time = xTaskGetTickCount();

                    // esp_sleep_enable_ulp_wakeup();
                    // esp_deep_sleep_start();

                    // ESP_LOGI(TAG, "Wake up");
                    // esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * WAKE_UP_CHECK);
                    // if (average_sensor_value <= average_pot_value)
                    // {
                    //     // ulp - co processor
                    //     ESP_LOGI(TAG, "Entering Sleep");
                    //     vTaskDelay(pdMS_TO_TICKS(100));
                    //     esp_deep_sleep_start();
                    // }
                }
                else
                {
                    ESP_LOGI(TAG, "LED State set to 0 (Safe)");
                    display_ui(display, 0, average_sensor_value, average_pot_value, false);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
void FreeForAll(adc_t *pot, adc_t *photoresistor, pwm_component_t *buzzer, onboard_rgb_led_t *rgb_led, display_component_t *display)
{
    adc_free(pot);
    adc_free(photoresistor);
    pwm_free(buzzer);
    rgb_led_free(rgb_led);
    display_free(display);
}
void ulp_program()
{
    esp_err_t err = ulp_lp_core_load_binary(ulp_main_bin_start, (ulp_main_bin_end - ulp_main_bin_start));
    ESP_ERROR_CHECK(err);

    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER,
        .lp_timer_sleep_duration_us = 10000,
    };

    err = ulp_lp_core_run(&cfg);
}
void test_callback(int pin, void *arg)
{
    pwm_component_t *buzzer = (pwm_component_t *)arg;

    buzzer->melody_choice++;
    if (buzzer->melody_choice > 5)
    {
        buzzer->melody_choice = 1;
    }
    choose_melody(buzzer, buzzer->melody_choice);
}