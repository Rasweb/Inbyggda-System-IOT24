#include "potentiometer_comp.h"

potentiometer *pot_init(PIN_GPIO pin, THRESHOLD threshold, CHANNEL_TYPE channel)
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
    ESP_ERROR_CHECK_WITHOUT_ABORT(adc_oneshot_config_channel(pot_handle, channel, &config));

    potentiometer *new_pot = pvPortMalloc(sizeof(potentiometer));
    new_pot->pin = pin;
    new_pot->onThresholdCallback = NULL;
    new_pot->threshold = threshold;
    new_pot->lastValue = 0;
    new_pot->adc_raw = 0;
    new_pot->risingEdge = 1;
    new_pot->handle = pot_handle;
    new_pot->adc_channel = channel;
    return new_pot;
}
void pot_update(potentiometer *pot)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(adc_oneshot_read(pot->handle, pot->adc_channel, &pot->adc_raw));

    // Nurvarande värde överstiger thresholden
    if (pot->adc_raw >= pot->threshold && pot->risingEdge)
    {
        if (pot->onThresholdCallback != NULL)
        {
            pot->onThresholdCallback(pot->adc_raw);
            // Tvingar threshold varningen att bara kallas en gång
            pot->risingEdge = 0;
        }
    }
    else if (pot->adc_raw < pot->threshold)
    {
        // Är värdet under thresholden, tillåt thresholden att kallas
        pot->risingEdge = 1;
    }

    pot->lastValue = pot->adc_raw;
}
int pot_getValue(potentiometer *pot)
{
    return pot->lastValue;
}

void pot_setOnThreshold(potentiometer *pot, void (*onThreshold)())
{
    pot->onThresholdCallback = onThreshold;
}

void pot_destroy(potentiometer *pot)
{
    if (pot != NULL)
    {
        vPortFree(pot);
        pot = NULL;
    }
}
