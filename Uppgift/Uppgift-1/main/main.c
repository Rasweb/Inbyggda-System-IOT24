#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "button_component.h"
#include "potentiometer_comp.h"
#include "esp_log.h"
#define BTN_1_PIN GPIO_NUM_2
#define BTN_1_PULL_DOWN 1
#define BTN_1_PULL_UP 0

#define INTR_TYPE_VAL GPIO_INTR_DISABLE

void callbackFunc(int pin)
{
    printf("Tryck: %d\n", pin);
}

void app_main(void)
{
    // pot_init(GPIO_NUM_3);
    adc_oneshot_unit_handle_t pot_handle;
    adc_oneshot_unit_init_cfg_t pot_conf = {
        .unit_id = ADC_UNIT_1,
        .clk_src = 0,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(adc_oneshot_new_unit(&pot_conf, &pot_handle));

    // ADC config
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_0,
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(adc_oneshot_config_channel(pot_handle, ADC_CHANNEL_0, &config));

    static int adc_raw[2][10];
    ESP_ERROR_CHECK_WITHOUT_ABORT(adc_oneshot_read(pot_handle, ADC_CHANNEL_0, &adc_raw[0][0]));
    ESP_LOGI("Main", "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, ADC_CHANNEL_0, adc_raw[0][0]);

    // BTN
    // button_component btn1 = button_init(BTN_1_PIN, BTN_1_PULL_DOWN, BTN_1_PULL_UP, INTR_TYPE_VAL);
    // setOnPressed(&btn1, callbackFunc);

    // while (1)
    // {
    //     button_update(&btn1);
    //     vTaskDelay(pdMS_TO_TICKS(10));
    // }
}