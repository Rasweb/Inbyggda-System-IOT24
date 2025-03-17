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
#define LCD_PIXEL_CLOCK_HZ (400 * 1000)
#define RST_PIN_NUM -1
#define I2C_HW_ADDR 0x3C

// The pixel number in horizontal and vertical
#define LCD_H_RES 128
#define LCD_V_RES 64

// Bit number used to represent command and parameter
#define LCD_CMD_BITS 8

/*
    Screen conf from:
        https://docs.lvgl.io/master/index.html
*/
typedef struct
{
    lv_disp_t *disp;
} display_component_t;

void display_ui(display_component_t *display, int state, int average_value, int pot_value);

display_component_t *display_init(int sda_pin, int scl_pin);

void display_free(display_component_t *display);
