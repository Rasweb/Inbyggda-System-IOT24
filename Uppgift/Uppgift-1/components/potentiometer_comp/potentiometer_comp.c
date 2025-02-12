#include "potentiometer_comp.h"

void pot_init(PIN_GPIO pin)
{
    printf("pot pin: %d", pin);
    // ADC init
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
    // potentiometer new_pot;
    // new_pot.pot_pin = pin;
    // new_pot.onThresholdCallback = NULL;

    // Recycle ADC
    // ESP_ERROR_CHECK(adc_oneshot_del_unit(pot_handle));
}
void update()
{
}
int getValue()
{
    return 0;
}
void setOnThreshold(potentiometer *pot, int threshold, int risingEdge, void (*onThreshold)())
{
    pot->onThresholdCallback = onThreshold;
}