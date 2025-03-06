#include "memory_component.h"
memory_component memory_init(char *namespace)
{
    esp_err_t err;

    err = nvs_flash_erase();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "first nvs erase error");
    }

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "nvs_flash_init failed");
    }

    nvs_handle_t handle;
    err = nvs_open(namespace, NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "nvs open error");
    }

    size_t req_size_serial = 0;
    char *serial_numbers = NULL;
    err = nvs_get_str(handle, "serial_number", NULL, &req_size_serial);
    if (err == ESP_OK)
    {
        free(serial_numbers);
        serial_numbers = malloc(req_size_serial);
        nvs_get_str(handle, "serial_number", serial_numbers, &req_size_serial);
    }

    char *device_names = NULL;
    size_t req_size_device = 0;
    err = nvs_get_str(handle, "device_name", NULL, &req_size_device);
    if (err == ESP_OK)
    {
        free(device_names);
        device_names = malloc(req_size_device);
        nvs_get_str(handle, "device_name", device_names, &req_size_device);
    }

    memory_component new_memory;
    new_memory.device_name = device_names;
    new_memory.serial_number = serial_numbers;
    new_memory.handle = handle;
    memory_close(handle);
    return new_memory;
}
void memory_set_device_name(memory_component *memory, char *string)
{
    nvs_handle_t handle;
    esp_err_t err;

    int inp_string_size = strlen(string);
    if (inp_string_size > 16)
    {
        ESP_LOGE(TAG, "Device name can't be longer than 16 chars");
        return;
    }

    err = nvs_open("device_info", NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "nvs open error");
    }

    err = nvs_set_str(handle, "device_name", string);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set device name");
    }

    err = nvs_commit(handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to commit device name");
    }
    else
    {
        free(memory->device_name);
        memory->device_name = malloc(inp_string_size + 1);
        memcpy(memory->device_name, string, inp_string_size);
        memory->device_name[inp_string_size] = '\0';
    }
    memory_close(handle);
}

void memory_set_serial_number(memory_component *memory, char *string)
{
    nvs_handle_t handle;
    esp_err_t err;

    int inp_string_size = strlen(string);
    if (inp_string_size > 16)
    {
        ESP_LOGE(TAG, "Serial number can't be longer than 16 chars");
        return;
    }

    err = nvs_open("device_info", NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "nvs open error");
    }

    err = nvs_set_str(handle, "serial_number", string);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set serial number");
    }

    err = nvs_commit(handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to commit serial number");
    }
    else
    {
        free(memory->serial_number);
        memory->serial_number = malloc(inp_string_size + 1);
        memcpy(memory->serial_number, string, inp_string_size);
        memory->serial_number[inp_string_size] = '\0';
    }
    memory_close(handle);
}

char *memory_get_device_name(memory_component memory)
{
    return memory.device_name;
}

char *memory_get_serial_number(memory_component memory)
{
    return memory.serial_number;
}

void memory_close(nvs_handle_t handle)
{
    nvs_close(handle);
}

static void memory_destroy(memory_component *memory)
{
    nvs_flash_erase();
    // free(memory.device_name);
    // free(memory.serial_number);
    // free(memory);
}
