#pragma once
#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#define TAG "NVS_H"
#define DEVICE_NAME "Device name"
#define SERIAL_NUMBER "Serial number"
typedef struct
{
    char *device_name;
    char *serial_number;
    char *first_namespace;
    char *second_namespace;
    esp_err_t err;
    nvs_handle_t handle;
} memory_component;

// Uppgift - spara konfiguering

// Hämtar info från NVS till init och spara i arbetsminne
// Ladda alla parametrar från NVS
memory_component *memory_init(char *namespace_1, char *namespace_2);

// Hämta från arbetsminne
// Returnera device name parameter från arbetsminnet
char *memory_get_device_name(memory_component *memory);

// Returnera serial number parameter från arbetsminnet
char *memory_get_serial_number(memory_component *memory);

// Sätter på nvs och arbetsminnet
// Kopiera in nytt device name till arbetsminne och spara på nvs
void memory_set_device_name(memory_component *memory, char *key, char *string);

// Kopiera in nytt serial number till arbetsminne och spara på nvs
void memory_set_serial_number(memory_component *memory, char *key, char *string);

void memory_close(memory_component *memory);
