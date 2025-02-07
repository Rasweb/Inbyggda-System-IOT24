#include <stdio.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include <string.h>
#include "esp_log.h"

#define TX_GPIO_PIN GPIO_NUM_5
#define RX_GPIO_PIN GPIO_NUM_4
#define TX_BUFFER_SIZE 2048
#define RTX_BUFFER_SIZE 2048
#define UART_RECEIVE_BUFFER_SIZE 128
#define SEND_UART_PERIOD 2000

const uart_port_t uart_num = UART_NUM_1;

uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};

QueueHandle_t uart_queue;

void app_main(void)
{
    ESP_ERROR_CHECK(uart_driver_install(uart_num, TX_BUFFER_SIZE, RTX_BUFFER_SIZE, 10, &uart_queue, 0));
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    // Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19)
    // ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, 4, 5, 18, 19));

    ESP_ERROR_CHECK(uart_set_pin(uart_num, TX_GPIO_PIN, RX_GPIO_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // ESP_ERROR_CHECK(uart_driver_install(uart_num, TX_BUFFER_SIZE, RTX_BUFFER_SIZE, 10, &uart_queue, 0));

    TickType_t sentTimer = xTaskGetTickCount();
    while (1)
    {

        TickType_t elapsedTime = xTaskGetTickCount() - sentTimer;
        if (elapsedTime > pdMS_TO_TICKS(SEND_UART_PERIOD))
        {
            sentTimer += pdMS_TO_TICKS(SEND_UART_PERIOD);
            // Write data to UART.
            char *test_str = "Hej hej William.\n";
            int bytesSent = uart_write_bytes(uart_num, (const char *)test_str, strlen(test_str));
            if (bytesSent < 0)
            {
                ESP_LOGE("MAIN", "Error writing uart");
            }
            else if (bytesSent > 0)
            {
                ESP_LOGI("MAIN", "Sent %d bytes on uart %s", bytesSent, test_str);
            }
            else
            {
                ESP_LOGI("MAIN", "Sent no bytes on uart");
            }
        }

        // Read data from UART.
        uint8_t data[UART_RECEIVE_BUFFER_SIZE] = {0};
        int length = 0;
        ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t *)&length));
        if (length > UART_RECEIVE_BUFFER_SIZE - 1)
        {
            ESP_LOGI("MAIN", "Bytes in buffer %d", length);
            length = uart_read_bytes(uart_num, data, UART_RECEIVE_BUFFER_SIZE - 1, pdMS_TO_TICKS(100));
        }
        else if (length > 0)
        {
            ESP_LOGI("MAIN", "Bytes in buffer %d", length);
            length = uart_read_bytes(uart_num, data, length, pdMS_TO_TICKS(100));
        }
        else
        {
            // ESP_LOGI("MAIN", "Bytes in buffer %d", length);

            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        if (length < 0)
        {
            ESP_LOGE("MAIN", "Error reading uart");
        }
        else if (length > 0)
        {
            data[length] = 0;
            ESP_LOGI("MAIN", "Received %d bytes on uart %s", length, data);
        }
        else
        {
            ESP_LOGI("MAIN", "Received no bytes on uart");
        }
    }

    //  // Configure a temporary buffer for the incoming data
    //  uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    //  while (1) {
    //      // Read data from the UART
    //      int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
    //      // Write data back to the UART
    //      uart_write_bytes(ECHO_UART_PORT_NUM, (const char *) data, len);
    //      if (len) {
    //          data[len] = '\0';
    //          ESP_LOGI(TAG, "Recv str: %s", (char *) data);
    //      }
    //  }
}