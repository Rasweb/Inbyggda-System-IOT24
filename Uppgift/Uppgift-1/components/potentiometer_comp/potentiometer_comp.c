#include "potentiometer_comp.h"

potentiometer *pot_init(PIN_GPIO pin, THRESHOLD threshold)
{
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
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(adc_oneshot_config_channel(pot_handle, ADC_CHANNEL_3, &config));

    potentiometer *new_pot = pvPortMalloc(sizeof(potentiometer));
    new_pot->pin = pin;
    new_pot->onThresholdCallback = NULL;
    new_pot->lastValue = 0;
    new_pot->adc_raw = threshold;
    new_pot->risingEdge = 1;
    new_pot->handle = pot_handle;
    return new_pot;
}
void pot_update(potentiometer *pot)
{
    // int adc_raw = 512;
    // ESP_ERROR_CHECK_WITHOUT_ABORT(adc_oneshot_read(pot->handle, ADC_CHANNEL_3, &adc_raw));
    ESP_ERROR_CHECK_WITHOUT_ABORT(adc_oneshot_read(pot->handle, ADC_CHANNEL_3, &pot->adc_raw));
    pot->lastValue = pot->adc_raw;
    int curr = pot_getValue(pot);
    printf("Current: %d", curr);
    printf("Threshold: %d", pot->threshold);
    if (pot->onThresholdCallback != NULL)
        pot->onThresholdCallback(pot->lastValue);
}
int pot_getValue(potentiometer *pot)
{
    return pot->lastValue;
}
// int threshold, bool risingEdge
void pot_setOnThreshold(potentiometer *pot, void (*onThreshold)())
{
    pot->onThresholdCallback = onThreshold;
}

void pot_destroy(potentiometer *pot)
{
    vPortFree(pot);
    pot = NULL;
}
