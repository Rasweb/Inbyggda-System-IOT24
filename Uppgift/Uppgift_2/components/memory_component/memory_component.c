#include "memory_component.h"

static void memory_init_error(esp_err_t err, char *namespace)
{
    if (err == ESP_ERR_NVS_NOT_FOUND)
        ESP_LOGE(TAG, "Invalid Namespace: %s", namespace);
    if (err == ESP_ERR_NVS_NOT_ENOUGH_SPACE)
        ESP_LOGE(TAG, "Not enough space in storage for: %s", namespace);
    if (err == ESP_ERR_NO_MEM)
        ESP_LOGE(TAG, "Not enough memory for: %s", namespace);
}
static void memory_get_error(esp_err_t err, char *namespace)
{
    if (err == ESP_ERR_NVS_NOT_FOUND)
        ESP_LOGE(TAG, "%s doesn't exist", namespace);
    if (err == ESP_ERR_NVS_INVALID_HANDLE)
        ESP_LOGE(TAG, "Handle for %s has been closed or is NULL", namespace);
    if (err == ESP_ERR_NVS_INVALID_NAME)
        ESP_LOGE(TAG, "%s name doesn't satisfy constrains", namespace);
    if (err == ESP_ERR_NVS_INVALID_LENGTH)
        ESP_LOGE(TAG, "Invalid length for %s", namespace);
}
static void memory_set_error(esp_err_t err, char *namespace)
{
    if (err == ESP_ERR_NVS_INVALID_HANDLE)
        ESP_LOGE(TAG, "Handle has been closed or is NULL for: %s", namespace);
    if (err == ESP_ERR_NVS_INVALID_NAME)
        ESP_LOGE(TAG, "%s name doesn't satisfy constrains", namespace);
    if (err == ESP_ERR_NVS_NOT_ENOUGH_SPACE)
        ESP_LOGE(TAG, "Not enough space for: %s", namespace);
    if (err == ESP_ERR_NVS_VALUE_TOO_LONG)
        ESP_LOGE(TAG, "String for %s is too long", namespace);
}

memory_component *memory_init(char *namespace_1, char *namespace_2)
{
    esp_err_t err_1;
    esp_err_t err_2;
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

    err_1 = nvs_open(namespace_1, NVS_READWRITE, &handle);
    err_2 = nvs_open(namespace_2, NVS_READWRITE, &handle);
    memory_init_error(err_1, namespace_1);
    memory_init_error(err_2, namespace_2);

    memory_component *new_memory = malloc(sizeof(memory_component));
    new_memory->device_name = "";
    new_memory->serial_number = "";
    new_memory->handle = handle;
    new_memory->first_namespace = namespace_1;
    new_memory->second_namespace = namespace_2;
    new_memory->err = 0;
    return new_memory;
}

char *memory_get_device_name(memory_component *memory)
{
    size_t req_size;
    memory->err = nvs_get_str(memory->handle, memory->second_namespace, NULL, &req_size);
    memory->device_name = malloc(req_size);
    memory->err = nvs_get_str(memory->handle, memory->second_namespace, memory->device_name, &req_size);
    memory_get_error(memory->err, DEVICE_NAME);
    return memory->device_name;
}

char *memory_get_serial_number(memory_component *memory)
{
    size_t req_size;
    memory->err = nvs_get_str(memory->handle, memory->first_namespace, NULL, &req_size);
    memory->serial_number = malloc(req_size);
    memory->err = nvs_get_str(memory->handle, memory->first_namespace, memory->serial_number, &req_size);
    memory_get_error(memory->err, SERIAL_NUMBER);
    return memory->serial_number;
}

void memory_set_device_name(memory_component *memory, char *key, char *string)
{
    memory->err = nvs_set_str(memory->handle, key, string);
    memory_set_error(memory->err, key);
    memory->err = nvs_commit(memory->handle);
    memory_set_error(memory->err, key);
}

void memory_set_serial_number(memory_component *memory, char *key, char *string)
{
    memory->err = nvs_set_str(memory->handle, key, string);
    memory_set_error(memory->err, key);
    memory->err = nvs_commit(memory->handle);
    memory_set_error(memory->err, key);
}

void memory_close(memory_component *memory)
{
    nvs_close(memory->handle);
    free(memory->device_name);
    free(memory->serial_number);
    free(memory);
}

static void memory_destroy(memory_component *memory)
{
    nvs_flash_erase();
}
