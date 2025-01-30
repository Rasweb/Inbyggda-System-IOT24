#include <stdio.h>
#include "helper_component.h"
#include "printer.h"
#include "esp_log.h"

// Will be managed as a c function
extern "C"{
    void app_main(void)
    {
        int a = print();
        ESP_LOGI("MAIN","got value %d", a);
    }
}