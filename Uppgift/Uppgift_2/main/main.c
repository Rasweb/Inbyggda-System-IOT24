#include <stdio.h>
#include "memory_component.h"

void app_main(void)
{
    char namespace1[] = "serial_number";
    char namespace2[] = "device_name";
    memory_component *new_memory = memory_init(namespace1, namespace2);
    memory_set_device_name(new_memory, namespace1, "bob4");
    memory_set_serial_number(new_memory, namespace2, "1234");

    char *device_name = memory_get_device_name(new_memory);
    char *serial_number = memory_get_serial_number(new_memory);

    ESP_LOGI("MAIN", "TEST1 %s", device_name);
    ESP_LOGI("MAIN", "TEST2 %s", serial_number);
    memory_destroy(new_memory);
    char *res2Extra = memory_get_serial_number(new_memory);
    char *res1 = memory_get_device_name(new_memory);
}
