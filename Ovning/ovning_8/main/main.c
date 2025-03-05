#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
// Problem med 1024 bytes för STACK_SIZE när esp_logi() används
#define STACK_SIZE 2024 // 2048 // 4096
#define TAG "MAIN"
#define DELAY_TIME_BLINK_TASK 1000
#define DELAY_TIME_PRINT_TASK 500
#define DELAY_TIME_ADC_TASK 800
#define LED_PIN GPIO_NUM_2

static uint8_t ucParameterToPass;
TaskHandle_t blinkTaskHandle;
TaskHandle_t printTaskHandle;
TaskHandle_t adcTaskHandle;

void blink_task();
void print_task();
void adc_task();
void delete_blink_task();
void delete_print_task();
void delete_adc_task();

void app_main(void)
{
    if (xTaskCreate(blink_task, "Blink Task", STACK_SIZE, &ucParameterToPass, 5, &blinkTaskHandle) != pdPASS)
        printf("Blink task error!\n");
    if (xTaskCreate(print_task, "Print Task", STACK_SIZE, &ucParameterToPass, 3, &printTaskHandle) != pdPASS)
        printf("Print task error!\n");
    if (xTaskCreate(adc_task, "ADC Task", STACK_SIZE, &ucParameterToPass, 3, &adcTaskHandle) != pdPASS)
        printf("ADC task error!\n");
}

void blink_task()
{
    int state = 1;
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(DELAY_TIME_BLINK_TASK));
        gpio_set_level(LED_PIN, state);
        state = !state;
    }
}
void print_task()
{
    while (1)
    {
        printf("Print task\n");
        vTaskDelay(pdMS_TO_TICKS(DELAY_TIME_PRINT_TASK));
    }
}

void adc_task()
{

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(DELAY_TIME_ADC_TASK));
    }
}
void delete_blink_task()
{
    vTaskDelete(blinkTaskHandle);
}

void delete_print_task()
{
    vTaskDelete(printTaskHandle);
}
void delete_adc_task()
{
    vTaskDelete(adcTaskHandle);
}