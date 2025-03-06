#include "potentiometer_comp.h"

potentiometer *pot_init(PIN_GPIO pin, CHANNEL_TYPE channel)
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
    new_pot->beforeThresholdCallback = NULL;
    new_pot->threshold = 0;
    new_pot->lastValue = 0;
    new_pot->adc_raw = 0;
    new_pot->risingEdge = true;
    new_pot->fallingEdge = false;
    new_pot->thresholdState = false;
    new_pot->handle = pot_handle;
    new_pot->adc_channel = channel;
    return new_pot;
}

void pot_update(potentiometer *pot)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(adc_oneshot_read(pot->handle, pot->adc_channel, &pot->adc_raw));

    // Om nurvarande värde överstiger gränsen
    if (pot->adc_raw >= pot->threshold)
    {
        if (pot->risingEdge && pot->onThresholdCallback != NULL && !pot->thresholdState)
        {
            pot->onThresholdCallback(pot->adc_raw);
            pot->thresholdState = true;
        }
    }
    else
    {
        // Om nurvarande värde understiger gränsen
        if (pot->fallingEdge && pot->beforeThresholdCallback != NULL && pot->thresholdState)
        {
            pot->beforeThresholdCallback(pot->adc_raw);
            pot->thresholdState = false;
        }
    }
    if (pot->adc_raw < pot->threshold)
    {
        pot->thresholdState = false;
    }
    else
    {
        pot->thresholdState = true;
    }
    pot->lastValue = pot->adc_raw;
}
// void pot_update(potentiometer *pot)
// {
//     ESP_ERROR_CHECK_WITHOUT_ABORT(adc_oneshot_read(pot->handle, pot->adc_channel, &pot->adc_raw));

//     // Nurvarande värde överstiger thresholden
//     if (pot->adc_raw >= pot->threshold && pot->risingEdge)
//     {
//         if (pot->onThresholdCallback != NULL)
//         {
//             pot->onThresholdCallback(pot->adc_raw);
//             // Tvingar threshold varningen att bara kallas en gång
//             pot->risingEdge = 0;
//         }
//     }
//     else if (pot->adc_raw < pot->threshold)
//     {
//         // Är värdet under thresholden, tillåt thresholden att kallas
//         pot->risingEdge = 1;
//     }
//     pot->lastValue = pot->adc_raw;
// }
int pot_getValue(potentiometer *pot)
{
    return pot->lastValue;
}

void pot_setOnThreshold(potentiometer *pot, THRESHOLD threshold, bool after, bool before, void (*onThreshold)(), void (*beforeThreshold)())
{
    pot->onThresholdCallback = onThreshold;
    pot->beforeThresholdCallback = beforeThreshold;
    pot->threshold = threshold;
    pot->risingEdge = after;
    pot->fallingEdge = before;
}

void pot_destroy(potentiometer *pot)
{
    if (pot != NULL)
    {
        vPortFree(pot);
        pot = NULL;
    }
}
