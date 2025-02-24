#include "memory_component.h"
// Uppgift - spara konfiguering

// Hämtar info från NVS till init och spara i arbetsminne
// Ladda alla parametrar från NVS
memory_component *memory_init(char *namespace_1, char *namespace_2)
{
    esp_err_t err1;
    esp_err_t err2;
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

    err1 = nvs_open(namespace_1, NVS_READWRITE, &handle);
    err2 = nvs_open(namespace_2, NVS_READWRITE, &handle);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err1);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err2);

    if (err1 == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE("MAIN", "Invalid Namespace: %s", namespace_1);
    }
    if (err2 == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE("MAIN", "Invalid Namespace: %s", namespace_2);
    }

    // button_component *new_btn = pvPortMalloc(sizeof(button_component));
    memory_component *new_memory = malloc(sizeof(memory_component));
    new_memory->device_name = "";
    new_memory->serial_number = "";
    new_memory->handle = handle;
    return new_memory;
}

static void memory_error_check(esp_err_t err, char *namespace)
{
    if (err == ESP_ERR_NVS_NOT_FOUND || err == ESP_ERR_NVS_INVALID_HANDLE)
    {
        ESP_LOGE("MAIN", "%s doesn't exist or Handle has been closed", namespace);
    }
}
// Hämta från arbetsminne
// Returnera device name parameter från arbetsminnet
char *memory_get_device_name(memory_component *memory)
{
    // Get string for given key:
    esp_err_t err;

    size_t req_size;
    err = nvs_get_str(memory->handle, "device_name", NULL, &req_size);
    memory->device_name = malloc(req_size);
    err = nvs_get_str(memory->handle, "device_name", memory->device_name, &req_size);
    memory_error_check(err, "Device name");
    return memory->device_name;
}

// Returnera serial number parameter från arbetsminnet
char *memory_get_serial_number(memory_component *memory)
{
    // Get string for given key:
    esp_err_t err;

    size_t req_size;
    err = nvs_get_str(memory->handle, "serial_number", NULL, &req_size);
    memory->serial_number = malloc(req_size);
    err = nvs_get_str(memory->handle, "serial_number", memory->serial_number, &req_size);
    memory_error_check(err, "Serial number");
    return memory->serial_number;
}

// Sätter på nvs och arbetsminnet
// Kopiera in nytt device name till arbetsminne och spara på nvs
void memory_set_device_name(memory_component *memory, char *key, char *string)
{
    esp_err_t err;
    err = nvs_set_str(memory->handle, key, string);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);
    err = nvs_commit(memory->handle);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);
}

// Kopiera in nytt serial number till arbetsminne och spara på nvs
void memory_set_serial_number(memory_component *memory, char *key, char *string)
{
    esp_err_t err;
    err = nvs_set_str(memory->handle, key, string);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);
    err = nvs_commit(memory->handle);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);
}

void memory_destroy(memory_component *memory)
{
    // Freeing the handles
    nvs_close(memory->handle);

    // nvs_flash_erase();
}
