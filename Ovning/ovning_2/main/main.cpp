#include <stdio.h>
#include "helper_component.h"
#include "printer.h"
#include "esp_log.h"

// Will be managed as a c function
extern "C"{
    void app_main(void)
    {
        int a = ovning_2::print();
        ESP_LOGI("MAIN","got value %d", a);
        a = inbyggda_iot24::func();
        ESP_LOGI("MAIN","got value %d", a);
    }
}