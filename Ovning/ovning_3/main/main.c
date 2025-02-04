#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "driver/ledc.h"

void app_main(void) {

    gpio_config_t io_conf_2;
    io_conf_2.mode = GPIO_MODE_INPUT;
    io_conf_2.intr_type = GPIO_INTR_DISABLE;
    io_conf_2.pull_down_en = 1;
    io_conf_2.pull_up_en = 0;
    io_conf_2.pin_bit_mask = 1ULL << GPIO_NUM_2;

    gpio_config_t io_conf_3;
    io_conf_3.mode = GPIO_MODE_INPUT;
    io_conf_3.intr_type = GPIO_INTR_DISABLE;
    io_conf_3.pull_down_en = 0;
    io_conf_3.pull_up_en = 1;
    io_conf_3.pin_bit_mask = 1ULL << GPIO_NUM_3;

    ledc_timer_config_t ledcTimerConfig;
    ledcTimerConfig.clk_cfg = LEDC_AUTO_CLK;
    ledcTimerConfig.deconfigure = false;
    ledcTimerConfig.duty_resolution = LEDC_TIMER_12_BIT;
    ledcTimerConfig.freq_hz = 1000;
    ledcTimerConfig.speed_mode = LEDC_LOW_SPEED_MODE;
    ledcTimerConfig.timer_num = LEDC_TIMER_0;
    ledc_timer_config(&ledcTimerConfig);

    ledc_channel_config_t ledcChannelConfig;
    ledcChannelConfig.channel = LEDC_CHANNEL_0;
    ledcChannelConfig.duty = 0;
    ledcChannelConfig.flags.output_invert = 0;
    ledcChannelConfig.gpio_num = GPIO_NUM_23;
    ledcChannelConfig.hpoint = 0;
    ledcChannelConfig.intr_type = LEDC_INTR_DISABLE;
    ledcChannelConfig.speed_mode = LEDC_LOW_SPEED_MODE;
    ledcChannelConfig.timer_sel = LEDC_TIMER_0;
    ledc_channel_config(&ledcChannelConfig);

    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0b111111111111/2, 0);

    esp_err_t error_2 = gpio_config(&io_conf_2);
    esp_err_t error_3 = gpio_config(&io_conf_3);

    int pull_down = gpio_get_level(GPIO_NUM_2);
    int pull_up = gpio_get_level(GPIO_NUM_3);
    TickType_t starTick_2 = xTaskGetTickCount();
    TickType_t starTick_3 = xTaskGetTickCount();

    int latch_2 = 0;
    int latch_3 = 0;

    int duty = 0;
    int previous_duty = 0;
    while(1){
        vTaskDelay(pdMS_TO_TICKS(10));

        pull_down = gpio_get_level(GPIO_NUM_2);
        if(pull_down == 1 && latch_2 == 0){
            TickType_t tickDifference = xTaskGetTickCount() - starTick_2;
            if(tickDifference >= pdMS_TO_TICKS(300)){
                printf("down: 1\n");
                starTick_2 = xTaskGetTickCount();
                latch_2 = 1;
                duty += 50;
                if(duty > 4095) duty = 4095;
                ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0, duty, 0);
            }
        }
        else if(pull_down == 0 && latch_2 == 1){
            TickType_t tickDifference = xTaskGetTickCount() - starTick_2;
            if(tickDifference >= pdMS_TO_TICKS(300)){
                latch_2 = 0;
            }
        }

        pull_up = gpio_get_level(GPIO_NUM_3);
        if(pull_up == 1 && latch_3 == 0){
            TickType_t tickDifference = xTaskGetTickCount() - starTick_3;
            if(tickDifference >= pdMS_TO_TICKS(300)){
                printf("up: 1\n");
                starTick_3 = xTaskGetTickCount();
                latch_3 = 1;
                duty -= 50;
                if(duty < 0) duty = 0;
                ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty, 0);
            }
        }
        else if(pull_up == 0 && latch_3 == 1){
            TickType_t tickDifference = xTaskGetTickCount() - starTick_3;
            if(tickDifference >= pdMS_TO_TICKS(300)){
                latch_3 = 0;
            }
        }
    }
}


