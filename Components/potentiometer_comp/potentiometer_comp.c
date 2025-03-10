#include "potentiometer_comp.h"

adc_t *adc_init(PIN_GPIO pin, CHANNEL_TYPE channel, adc_unit_t adc_unit)
{
    // ADC init
    adc_oneshot_unit_handle_t pot_handle;
    adc_oneshot_unit_init_cfg_t pot_conf = {
        .unit_id = adc_unit,
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

    adc_t *new_adc = pvPortMalloc(sizeof(adc_t));
    new_adc->pin = pin;
    new_adc->onThresholdCallback = NULL;
    new_adc->beforeThresholdCallback = NULL;
    new_adc->threshold = 0;
    new_adc->lastValue = 0;
    new_adc->adc_raw = 0;
    new_adc->risingEdge = true;
    new_adc->fallingEdge = false;
    new_adc->thresholdState = false;
    new_adc->handle = pot_handle;
    new_adc->adc_channel = channel;
    return new_adc;
}

void adc_update(adc_t *pot)
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

int adc_getValue(adc_t *pot)
{
    return pot->lastValue;
}

void adc_setOnThreshold(adc_t *pot, THRESHOLD threshold, bool after, bool before, void (*onThreshold)(), void (*beforeThreshold)())
{
    pot->onThresholdCallback = onThreshold;
    pot->beforeThresholdCallback = beforeThreshold;
    pot->threshold = threshold;
    pot->risingEdge = after;
    pot->fallingEdge = before;
}

void adc_destroy(adc_t *pot)
{
    if (pot != NULL)
    {
        vPortFree(pot);
        pot = NULL;
    }
}
