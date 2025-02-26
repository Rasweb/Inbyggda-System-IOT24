#include <stdio.h>
#include "memory_component.h"
#include "freertos/FreeRTOS.h"
#define NVS_MAIN "NVS_MAIN"
#define NAMESPACE1 "serial_number"
#define NAMESPACE2 "device_name"
#define MACRO_DEVICE_NAME "bob4"
#define MACRO_SERIAL_NUMBER "1234"

void app_main(void)
{
    memory_component *new_memory = memory_init(NAMESPACE1, NAMESPACE2);
    memory_set_device_name(new_memory, NAMESPACE1, MACRO_DEVICE_NAME);
    memory_set_serial_number(new_memory, NAMESPACE2, MACRO_SERIAL_NUMBER);

    char *device_name = memory_get_device_name(new_memory);
    char *serial_number = memory_get_serial_number(new_memory);

    ESP_LOGI(NVS_MAIN, "TEST1 %s", device_name);
    ESP_LOGI(NVS_MAIN, "TEST2 %s", serial_number);

    memory_close(new_memory);
}
