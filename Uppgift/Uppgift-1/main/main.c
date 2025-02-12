#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "button_component.h"
#define BTN_1_PIN GPIO_NUM_2
#define BTN_1_PULL_DOWN 1
#define BTN_1_PULL_UP 0

#define INTR_TYPE_VAL GPIO_INTR_DISABLE

void callbackFunc(int pin)
{
    printf("Tryck: %d\n", pin);
}

void app_main(void)
{
    button_component btn1 = button_init(BTN_1_PIN, BTN_1_PULL_DOWN, BTN_1_PULL_UP, INTR_TYPE_VAL);
    setOnPressed(&btn1, callbackFunc);

    while (1)
    {
        button_update(&btn1);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}