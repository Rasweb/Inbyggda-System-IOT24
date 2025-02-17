#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_sleep.h"
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define DEEP_SLEEP_SECONDS 3      /* Time ESP32 will go to sleep (in seconds) */
int count = 0;

void counterFunc()
{
    ESP_LOGI("MAIN", "Count nr: %d", count);
    if (count == 10)
    {
        ESP_LOGI("MAIN", "Entering sleep mode for: %d", DEEP_SLEEP_SECONDS);
        esp_deep_sleep(uS_TO_S_FACTOR * DEEP_SLEEP_SECONDS);
    }
    count++;
}

void app_main(void)
{
    while (1)
    {
        counterFunc();

        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * 1));
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_light_sleep_start());
    }
}