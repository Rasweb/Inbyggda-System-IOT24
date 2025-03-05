#include "memory_component.h"

memory_component *memory_init(char *namespace)
{
    esp_err_t err;
    err = nvs_flash_erase();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "first nvs erase error");
        return NULL;
    }
    nvs_handle_t handle;

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "nvs_flash_init failed");
        return NULL;
    }

    if (nvs_open(namespace, NVS_READWRITE, &handle) != ESP_OK)
    {
        ESP_LOGE(TAG, "nvs open error");
        return NULL;
    }

    size_t req_size_serial = 0;
    char *serial_number = NULL;
    if (nvs_get_str(handle, "serial_number", NULL, &req_size_serial) == ESP_OK)
    {
        serial_number = (char *)malloc(req_size_serial);
        // if (serial_number == NULL)
        // {
        //     ESP_LOGE(TAG, "Memory allocation failed for serial number");
        //     nvs_close(handle);
        //     return NULL;
        // }
        if (nvs_get_str(handle, "serial_number", serial_number, &req_size_serial) != ESP_OK)
        {
            ESP_LOGE(TAG, "serial number get str second");
            // free(serial_number);
            // serial_number = NULL;
        }
    }

    size_t req_size_device = 0;
    char *device_name = NULL;
    if (nvs_get_str(handle, "device_name", NULL, &req_size_device) == ESP_OK)
    {
        device_name = (char *)malloc(req_size_device);
        // if (device_name == NULL)
        // {
        //     ESP_LOGE(TAG, "Memory allocation failed for device name");
        //     nvs_close(handle);
        //     free(device_name);
        //     return NULL;
        // }
        if (nvs_get_str(handle, "device_name", device_name, &req_size_device) != ESP_OK)
        {
            ESP_LOGE(TAG, "device name get str second");
            // free(device_name);
            // device_name = NULL;
        }
    }

    memory_component *new_memory = malloc(sizeof(memory_component));
    new_memory->device_name = device_name;
    new_memory->serial_number = serial_number;
    new_memory->handle = handle;
    return new_memory;
}
void memory_set_device_name(memory_component *memory, char *string)
{
    if (memory == NULL || memory->handle == 0 || string == NULL)
        ESP_LOGE(TAG, "Invalid parameters for setting device name");

    if (nvs_set_str(memory->handle, "device_name", string) != ESP_OK)
        ESP_LOGE(TAG, "Failed to set device name");

    if (nvs_commit(memory->handle) != ESP_OK)
        ESP_LOGE(TAG, "Failed to commit device name");
    else
        ESP_LOGI(TAG, "Device name committed successfully");
}

void memory_set_serial_number(memory_component *memory, char *string)
{
    if (memory == NULL || memory->handle == 0 || string == NULL)
        ESP_LOGE(TAG, "Invalid parameters for setting serial number");

    if (nvs_set_str(memory->handle, "serial_number", string) != ESP_OK)
        ESP_LOGE(TAG, "Failed to set serial number");

    if (nvs_commit(memory->handle) != ESP_OK)
        ESP_LOGE(TAG, "Failed to commit serial number");
    else
        ESP_LOGI(TAG, "Serial number committed successfully");
}

char *memory_get_device_name(memory_component *memory)
{
    if (memory == NULL)
        ESP_LOGE(TAG, "Memory components is NULL");

    if (memory->device_name == NULL)
        ESP_LOGW(TAG, "Device name is not set");

    printf("device name: %s", memory->device_name);
    return memory->device_name;
}

char *memory_get_serial_number(memory_component *memory)
{
    if (memory == NULL)
        ESP_LOGE(TAG, "Memory components is NULL");

    if (memory->serial_number == NULL)
    {
        ESP_LOGW(TAG, "Serial number is not set");
    }

    printf("serial number: %s", memory->device_name);
    return memory->serial_number;
}

void memory_close(memory_component *memory)
{
    nvs_close(memory->handle);
    // free(memory->device_name);
    // free(memory->serial_number);
    // free(memory);
}

static void memory_destroy(memory_component *memory)
{
    nvs_flash_erase();
}
