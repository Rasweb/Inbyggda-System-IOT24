#include "memory_component.h"
// Uppgift - spara konfiguering

// Hämtar info från NVS till init och spara i arbetsminne
// Ladda alla parametrar från NVS
nvs_handle_t memory_init()
{
    nvs_handle_t handle;

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = nvs_open("serial_number", NVS_READWRITE, &handle);
    err = nvs_open("device_name", NVS_READWRITE, &handle);

    // Freeing the handles
    nvs_close(handle);
    nvs_close(handle);

    return handle;
}

// Hämta från arbetsminne
// Returnera device name parameter från arbetsminnet
char *memory_get_device_name(memory_component *memory)
{
    // Get string for given key:
    size_t req_size;
    nvs_handle_t handle;
    ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_get_str(handle, "device_name", NULL, &req_size));
    memory->device_name = malloc(req_size);
    ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_get_str(handle, "device_name", memory->device_name, &req_size));
    return memory->device_name;
}

// Returnera serial number parameter från arbetsminnet
char *memory_get_serial_number(memory_component *memory)
{

    // Get string for given key:
    // esp_err_t nvs_get_str(nvs_handle_t handle, const char *key, char *out_value, size_t *length)

    return memory->serial_number;
}

// Sätter på nvs och arbetsminnet
// Kopiera in nytt device name till arbetsminne och spara på nvs
void memory_set_device_name(char *string)
{
    nvs_handle_t handle;
    ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_set_str(handle, "device_name", string));
    ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_commit(handle));
}

// Kopiera in nytt serial number till arbetsminne och spara på nvs
void memory_set_serial_number(char *string) {}
