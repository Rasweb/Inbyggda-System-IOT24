#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "button_component.h"
#define BUTTON_PIN GPIO_NUM_2
#define BUTTON_PIN2 GPIO_NUM_3
#define PULL_DOWN_ENABLE 1
#define PULL_UP_ENABLE 0
#define INTR_TYPE_VAL GPIO_INTR_DISABLE

void buttonPressedCallback(int pin)
{
    printf("Tryckt");
}

void app_main(void)
{
    button_component btn1 = button_init(BUTTON_PIN, PULL_DOWN_ENABLE, PULL_UP_ENABLE, INTR_TYPE_VAL);
    button_component btn2 = button_init(BUTTON_PIN2, PULL_DOWN_ENABLE, PULL_UP_ENABLE, INTR_TYPE_VAL);

    TickType_t startTick = xTaskGetTickCount();
    while (1)
    {
        button_update(&btn1);
        button_update(&btn2);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}