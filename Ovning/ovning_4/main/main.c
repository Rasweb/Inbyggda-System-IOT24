#include <stdio.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "esp_task_wdt.h"

#define PULL_DOWN 0
#define PULL_UP 1
#define BUTTON_PIN GPIO_NUM_2
#define TIMEOUT_IN_MS 3000
#define TRIGGER_PANIC true
#define BUTTON_PRESSED_FALSE false
#define BUTTON_PRESSED_TRUE true
#define MS_TO_TICKS 10
#define BUTTON_LEVEL 0

void app_main(void) {
    // Knapp konfiguration
    gpio_config_t buttonConfig = {
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE, // GPIO_INTR_NEGEDGE - För Pull Up
        .pull_down_en = PULL_DOWN,
        .pull_up_en = PULL_UP,
        .pin_bit_mask = 1ULL << BUTTON_PIN,
    };
    // Error hantering för gpio konfigureringen
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&buttonConfig));
    bool pressed = BUTTON_PRESSED_FALSE;

    esp_task_wdt_config_t taskC = {
        .idle_core_mask = (1 << CONFIG_FREERTOS_NUMBER_OF_CORES) - 1,
        .timeout_ms = TIMEOUT_IN_MS,
        .trigger_panic = TRIGGER_PANIC,    
    };
    // Error hantering för gpio omkonfigueringen
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_task_wdt_reconfigure(&taskC));
    // Lägger till main() som en task att titta på
    esp_task_wdt_add(NULL);

    while (1)
    {
        int lvl = gpio_get_level(BUTTON_PIN);
        if(lvl == BUTTON_LEVEL) {
            pressed = BUTTON_PRESSED_TRUE;
            printf("Pressed\n");
        }
        if(pressed) {
            // Startar om den nurvarande tasken
            esp_task_wdt_reset();
            pressed = BUTTON_PRESSED_FALSE;
        }
        vTaskDelay(pdMS_TO_TICKS(MS_TO_TICKS));
    }
    // Tar bort den nurvarande tasken
    esp_task_wdt_delete(NULL);
}