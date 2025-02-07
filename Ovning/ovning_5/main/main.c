#include "driver/i2c_slave.h"
#include "freertos/FreeRTOS.h"

#define I2C_SLAVE_NUM GPIO_NUM_5
#define I2C_SLAVE_SCL_IO GPIO_NUM_7
#define I2C_SLAVE_SDA_IO GPIO_NUM_6
#define ESP_SLAVE_ADDR 1

i2c_slave_config_t i2c_slv_config = {
    .i2c_port = I2C_SLAVE_NUM,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .scl_io_num = I2C_SLAVE_SCL_IO,
    .sda_io_num = I2C_SLAVE_SDA_IO,
    .slave_addr = ESP_SLAVE_ADDR,
    .send_buf_depth = 100,
};

i2c_slave_dev_handle_t slave_handle;

typedef enum
{
    I2C_SLAVE_EVT_RX,
    I2C_SLAVE_EVT_TX
} i2c_slave_event_t;

QueueHandle_t queue;

// Slave Read
static bool i2c_slave_receive_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg)
{
    i2c_slave_event_t evt = I2C_SLAVE_EVT_RX;
    BaseType_t xTaskWoken = 0;
    // You can get data and length via i2c_slave_rx_done_event_data_t
    // xQueueSendFromISR(context->event_queue, &evt, &xTaskWoken);
    xQueueCRSendFromISR(queue) return xTaskWoken;
}

// Register callback in a task
i2c_slave_event_callbacks_t cbs = {
    .on_receive = i2c_slave_receive_cb,
};
ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(context.handle, &cbs, &context));

void app_main(void)
{
    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    // Uninstall Slave
    // i2c_del_slave_device(&slave_handle);
}

/* - WORKING UART
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

    ESP_ERROR_CHECK(uart_set_pin(uart_num, TX_GPIO_PIN, RX_GPIO_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

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
    */
}