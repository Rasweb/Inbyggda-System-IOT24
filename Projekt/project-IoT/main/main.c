#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "passive_buzzer_component.h"
#include "photoresistor_component.h"
#include "potentiometer_comp.h"
#define TAG "MAIN"
#define POT_THRESHOLD 3000
#define CHANNEL ADC_CHANNEL_2

void onThresholdCallback(int value)
{
    ESP_LOGI(TAG, "Rising Edge %d", value);
}

void beforeThresholdCallback(int value)
{
    ESP_LOGI(TAG, "Falling Edge %d", value);
}

/*
   // Potentiometer
   potentiometer *pot1 = pot_init(GPIO_NUM_3, CHANNEL);

   pot_setOnThreshold(pot1, POT_THRESHOLD, true, true, onThresholdCallback, beforeThresholdCallback);

   while (1)
   {
       pot_update(pot1);
       int currentValue = pot_getValue(pot1);
       // ESP_LOGI("MAIN", "Potentiometer Värde: %d", currentValue);
       vTaskDelay(pdMS_TO_TICKS(100));
   }
*/

void app_main(void)
{
    ESP_LOGI(TAG, "Hej");
}