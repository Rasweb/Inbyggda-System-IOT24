#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "button_component.h"
#include "potentiometer_comp.h"
#include "esp_log.h"
#define BTN_1_PIN GPIO_NUM_2
#define BTN_1_PULL_DOWN 1
#define BTN_1_PULL_UP 0
#define POT_THRESHOLD 3000

/*
Button state machine - https://github.com/CreeksideAB/inbyggda_system_iot24/tree/main/Ovning/ovning_state_machine
Threshold callback ska kallas när den har nått thresholden - För potentiometer: https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32c6/api-reference/peripherals/adc_oneshot.html
Binary led ingen pmw (av och på) - För digital led: https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32c6/api-reference/peripherals/gpio.html#gpio-rtc-gpio
Analog led (ljustyrka) - För led: https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32c6/api-reference/peripherals/ledc.html
*/

#define INTR_TYPE_VAL GPIO_INTR_DISABLE

void callbackFunc(int pin)
{
    printf("Tryck: %d\n", pin);
}

void thresholdCallback(int value)
{
    ESP_LOGI("MAIN", "Threshold reached");
}

void app_main(void)
{

    // Potentiometer
    potentiometer *pot1 = pot_init(GPIO_NUM_3, POT_THRESHOLD);

    pot_setOnThreshold(pot1, thresholdCallback);

    while (1)
    {
        pot_update(pot1);
        int currentValue = pot_getValue(pot1);
        ESP_LOGI("MAIN", "Potentiometer Värde: %d", currentValue);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}