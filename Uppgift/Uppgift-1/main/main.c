#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "button_component.h"
#include "potentiometer_comp.h"
#include "binary_led_component.h"
#include "esp_log.h"
#define BTN_1_PIN GPIO_NUM_2
#define BTN_1_PULL_DOWN 1
#define BTN_1_PULL_UP 0
#define INTR_TYPE_VAL GPIO_INTR_DISABLE
#define POT_THRESHOLD 3000
#define CHANNEL ADC_CHANNEL_3
#define BINARY_LED_MODE GPIO_MODE_OUTPUT
#define BINARY_LED_PIN GPIO_NUM_4
#define BINARY_LED_PULL_DOWN 1
#define BINARY_LED_PULL_UP 0

/*
Binary led ingen pmw (av och på) - För digital led: https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32c6/api-reference/peripherals/gpio.html#gpio-rtc-gpio
Analog led (ljustyrka) - För analog led: https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32c6/api-reference/peripherals/ledc.html
- (sin(x) + 1)
- E.g sin(1,5 pi) = -1 // Plusa på 1 för att få 0
https://setosa.io/ev/sine-and-cosine/

*/

void callbackFunc(int pin)
{
    printf("Tryck: %d\n", pin);
}

void thresholdCallback(int value)
{
    ESP_LOGI("MAIN", "Threshold reached %d", value);
}

void app_main(void)
{

    /*
    // BTN
    button_component *btn1 = btn_init(BTN_1_PIN, BTN_1_PULL_DOWN, BTN_1_PULL_UP, INTR_TYPE_VAL);
    btn_setOnPressed(btn1, callbackFunc);
    while (1)
    {

    btn_update(btn1);
    vTaskDelay(pdMS_TO_TICKS(10));
}
*/

    /*
    // Potentiometer
    potentiometer *pot1 = pot_init(GPIO_NUM_3, POT_THRESHOLD, CHANNEL);

    pot_setOnThreshold(pot1, thresholdCallback);

    while (1)
    {
        pot_update(pot1);
        int currentValue = pot_getValue(pot1);
        // ESP_LOGI("MAIN", "Potentiometer Värde: %d", currentValue);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    */

    binary_led_component *binary_led1 = binary_led_init(BINARY_LED_MODE, BINARY_LED_PIN, BINARY_LED_PULL_DOWN, BINARY_LED_PULL_UP);
    binary_led_setLed(BINARY_LED_PIN, 0);

    // Start blink:
    binary_led_blink(binary_led1, 500, 500);
    while (1)
    {
        binary_led_update(binary_led1);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}