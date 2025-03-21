#pragma once

#define TAG "MAIN"

// Pot component
#define POT_THRESHOLD 3000
#define POT_CHANNEL ADC_CHANNEL_2
#define POT_PIN GPIO_NUM_2
#define POT_ADC_UNIT 0
#define POT_BUFFER_SIZE 15

// Sensor component
#define PHOTORESISTOR_CHANNEL ADC_CHANNEL_4
#define PHOTORESISTOR_PIN GPIO_NUM_4
#define PHOTORESISTOR_ADC_UNIT 0
#define PHOTORESISTOR_BUFFER_SIZE 15
#define PHOTORESISTOR_CLOSE_VALUE 300

// Buzzer component
#define BUZZER_PIN GPIO_NUM_18
#define BUZZER_LEDC_TIMER LEDC_TIMER_0
#define BUZZER_LEDC_MODE LEDC_LOW_SPEED_MODE
#define BUZZER_LEDC_CHANNEL LEDC_CHANNEL_0
#define BUZZER_HERTZ 1000
#define BUZZER_TRIGGER_VALUE 200

// Onboard rgb led component
#define RGB_LED_STATE 0
#define RGB_LED_PIN GPIO_NUM_8
#define CHECK_INTERVAL_MS 2000

// Display component
#define DISPLAY_PIN_NUM_SDA 6
#define DISPLAY_PIN_NUM_SCL 7

// Button component
#define BTN_PIN GPIO_NUM_3
#define BTN_PULL_DOWN 0
#define BTN_PULL_UP 1
#define BTN_INTR_TYPE GPIO_INTR_DISABLE

#define SLEEP_TIMEOUT_MS 5000
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define WAKE_UP_CHECK 1