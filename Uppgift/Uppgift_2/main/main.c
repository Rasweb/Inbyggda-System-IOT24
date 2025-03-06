#include <stdio.h>
#include "memory_component.h"
#include "freertos/FreeRTOS.h"
#define NVS_MAIN "NVS_MAIN"
#define NAMESPACE "device_info"
// #define NAMESPACE1 "serial_number"
// #define NAMESPACE2 "device_name"
#define MACRO_DEVICE_NAME "name"
#define MACRO_SERIAL_NUMBER "1234"

void app_main(void)
{
    memory_component new_memory = memory_init(NAMESPACE);
    memory_set_device_name(&new_memory, MACRO_DEVICE_NAME);
    memory_set_serial_number(&new_memory, MACRO_SERIAL_NUMBER);

    char *device_name = memory_get_device_name(new_memory);
    char *serial_number = memory_get_serial_number(new_memory);

    if (device_name != NULL && serial_number != NULL)
    {
        ESP_LOGI(NVS_MAIN, "Device name: %s", device_name);
        ESP_LOGI(NVS_MAIN, "Serial number: %s", serial_number);
    }
    else
    {
        ESP_LOGE(NVS_MAIN, "Failed to retrieve device name or serial number");
    }

    memory_close(new_memory.handle);

    // memory_component *new_memory = memory_init(NAMESPACE);
    // if (new_memory == NULL)
    // {
    //     ESP_LOGE(NVS_MAIN, "Failed to initialize memory component");
    //     return;
    // }

    // memory_set_device_name(new_memory, MACRO_DEVICE_NAME);
    // memory_set_serial_number(new_memory, MACRO_SERIAL_NUMBER);

    // char *device_name = memory_get_device_name(new_memory);
    // char *serial_number = memory_get_serial_number(new_memory);
    // if (device_name != NULL && serial_number != NULL)
    // {
    //     ESP_LOGI(NVS_MAIN, "TEST1 %s", device_name);
    //     ESP_LOGI(NVS_MAIN, "TEST2 %s", serial_number);
    // }
    // else
    // {
    //     ESP_LOGE(NVS_MAIN, "Failed to retrieve device name or serial number");
    // }

    // memory_close(new_memory);

    // char *device_name2 = memory_get_device_name(new_memory);
    // char *serial_number2 = memory_get_serial_number(new_memory);
}
