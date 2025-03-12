#pragma once
#include <stdio.h>
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "lvgl.h"
#include "esp_lcd_io_i2c.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_panel_ssd1306.h"

#define TAG "Display"
#define I2C_BUS_PORT 0
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ (400 * 1000)
#define EXAMPLE_PIN_NUM_SDA 6
#define EXAMPLE_PIN_NUM_SCL 7
#define EXAMPLE_PIN_NUM_RST -1
#define EXAMPLE_I2C_HW_ADDR 0x3C

// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES 128
#define EXAMPLE_LCD_V_RES 64

// Bit number used to represent command and parameter
#define EXAMPLE_LCD_CMD_BITS 8
#define EXAMPLE_LCD_PARAM_BITS 8

typedef struct
{
    lv_disp_t *disp;
} display_component_t;

void display_ui(display_component_t *display);

display_component_t *display_init();

void free_display(display_component_t *display);
