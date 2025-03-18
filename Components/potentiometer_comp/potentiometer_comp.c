#include "potentiometer_comp.h"

adc_t *adc_init(PIN_GPIO pin, CHANNEL_TYPE channel, adc_unit_t adc_unit, int bufferSize)
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
    new_adc->buffer_index = 0;
    new_adc->buffer_sum = 0;
    new_adc->buffer_size = bufferSize;

    new_adc->buffer = (int *)malloc(bufferSize * sizeof(int));
    if (new_adc->buffer == NULL)
    {
        ESP_LOGI("POT", "Error allocating buffer size");
    }
    for (int i = 0; i < bufferSize; i++)
    {
        new_adc->buffer[i] = 0;
    }
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

// Updates buffer with a new value
void adc_update_buffer(adc_t *pot, int value)
{

    pot->buffer_sum -= pot->buffer[pot->buffer_index];
    pot->buffer[pot->buffer_index] = value;
    pot->buffer_sum += value;
    pot->buffer_index = (pot->buffer_index + 1) % pot->buffer_size;
}

int adc_buffer_average(adc_t *pot)
{
    int res = pot->buffer_sum / pot->buffer_size;
    return res;
}

void adc_destroy(adc_t *pot)
{
    if (pot != NULL)
    {
        free(pot->buffer);
        vPortFree(pot);
        pot = NULL;
    }
}
