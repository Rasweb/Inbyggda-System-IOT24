#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "button_component.h"
#include "potentiometer_comp.h"
#include "binary_led_component.h"
#include "analog_led_component.h"
#include "esp_log.h"
#define BTN_1_PIN GPIO_NUM_2
#define BTN_1_PULL_DOWN 1
#define BTN_1_PULL_UP 0
#define INTR_TYPE_VAL GPIO_INTR_DISABLE

#define POT_THRESHOLD 3000
#define CHANNEL ADC_CHANNEL_2

#define BINARY_LED_MODE GPIO_MODE_OUTPUT
#define BINARY_LED_PIN GPIO_NUM_4
#define BINARY_LED_PULL_DOWN 0
#define BINARY_LED_PULL_UP 1

#define ANALOG_LED_PIN GPIO_NUM_4
#define ANALOG_LED_DUTY_RES LEDC_TIMER_12_BIT
#define ANALOG_LED_SPEED_MODE LEDC_LOW_SPEED_MODE
#define ANALOG_LED_HERTZ 1000
#define ANALOG_LED_TIMER_NUM LEDC_TIMER_0
#define ANALOG_LED_CHANNEL LEDC_CHANNEL_0
#define ANALOG_LED_DUTY_RANGE 0
#define ANALOG_FADE_DURATION 100

/*
- (sin(x) + 1)
- E.g sin(1,5 pi) = -1 // Plusa på 1 för att få 0
https://setosa.io/ev/sine-and-cosine/

*/

void callbackFunc(int pin)
{
    printf("Tryck: %d\n", pin);
}

void onThresholdCallback(int value)
{
    ESP_LOGI("MAIN", "Rising Edge %d", value);
}

void beforeThresholdCallback(int value)
{
    ESP_LOGI("MAIN", "Falling Edge %d", value);
}
void app_main(void)
{

    /*
    // BTN
    button_component *btn1 = btn_init(BTN_1_PIN, BTN_1_PULL_DOWN, BTN_1_PULL_UP, INTR_TYPE_VAL);
    btn_setOnPressed(btn1, callbackFunc);
    // while (1)
    //{

    // btn_update(btn1);
    // vTaskDelay(pdMS_TO_TICKS(10));
    //}
    */

    // Potentiometer och knapp
    potentiometer *pot1 = pot_init(GPIO_NUM_3, CHANNEL);

    pot_setOnThreshold(pot1, POT_THRESHOLD, true, true, onThresholdCallback, beforeThresholdCallback);

    while (1)
    {
        pot_update(pot1);
        int currentValue = pot_getValue(pot1);
        ESP_LOGI("MAIN", "Potentiometer Värde: %d", currentValue);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    /*
    // Binary LED
    binary_led_component *binary_led1 = binary_led_init(BINARY_LED_MODE, BINARY_LED_PIN, BINARY_LED_PULL_DOWN, BINARY_LED_PULL_UP);
    binary_led_setLed(binary_led1, 0);

    // Start blink:
    binary_led_blink(binary_led1, 500, 3000);
    // binary_led_setLed(binary_led1, 1);
    while (1)
    {

    binary_led_update(binary_led1);
    vTaskDelay(pdMS_TO_TICKS(50));
    }
    */

    // Analog LED
    analog_led_component *analog_led1 = analog_led_init(ANALOG_LED_PIN, ANALOG_LED_HERTZ, ANALOG_LED_SPEED_MODE, ANALOG_LED_TIMER_NUM, ANALOG_LED_CHANNEL, ANALOG_LED_DUTY_RANGE, ANALOG_FADE_DURATION);
    // analog_setLed(analog_led1, ANALOG_FADE_IN);

    int period = 5;
    // int d = pdMS_TO_TICKS(100);
    analog_sin(analog_led1, period);
    analog_setLed(analog_led1, 2000);
    // analog_setLed(analog_led1, ANALOG_FADE_IN);

    while (1)
    {
        analog_led_update(analog_led1);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}