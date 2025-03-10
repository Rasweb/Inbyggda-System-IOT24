
#include "rgb_component.h"

// void rgb_led()
// {

//     // gpio_set_direction(RGB_LED_PIN, GPIO_MODE_OUTPUT);
//     // gpio_set_level(RGB_LED_PIN, 1);
//     // gpio_set_level(RGB_LED_PIN, 0);

//     // Configure LED PWM
//     ledc_timer_config_t red_ledc_timer = {
//         .speed_mode = LEDC_LOW_SPEED_MODE,
//         .timer_num = LEDC_TIMER_0,
//         .duty_resolution = LEDC_TIMER_10_BIT,
//         .freq_hz = 5000,
//         .clk_cfg = LEDC_AUTO_CLK,
//     };
//     ledc_timer_config(&red_ledc_timer);

//     ledc_channel_config_t red_ledc_channel = {
//         .speed_mode = LEDC_LOW_SPEED_MODE,
//         .channel = RED_CHANNEL,
//         .timer_sel = LEDC_TIMER_0,
//         .intr_type = LEDC_INTR_DISABLE,
//         .gpio_num = RGB_LED_PIN, // Adjust according to your setup
//         .duty = 0,
//         .hpoint = 0,
//     };
//     ledc_channel_config(&red_ledc_channel);

//     // Configure LED PWM
//     ledc_timer_config_t green_ledc_timer = {
//         .speed_mode = LEDC_LOW_SPEED_MODE,
//         .timer_num = LEDC_TIMER_0,
//         .duty_resolution = LEDC_TIMER_10_BIT,
//         .freq_hz = 5000,
//         .clk_cfg = LEDC_AUTO_CLK,
//     };
//     ledc_timer_config(&green_ledc_timer);

//     ledc_channel_config_t green_ledc_channel = {
//         .speed_mode = LEDC_LOW_SPEED_MODE,
//         .channel = GREEN_CHANNEL,
//         .timer_sel = LEDC_TIMER_0,
//         .intr_type = LEDC_INTR_DISABLE,
//         .gpio_num = RGB_LED_PIN, // Adjust according to your setup
//         .duty = 0,
//         .hpoint = 0,
//     };
//     ledc_channel_config(&green_ledc_channel);

//     // Configure LED PWM
//     ledc_timer_config_t blue_ledc_timer = {
//         .speed_mode = LEDC_LOW_SPEED_MODE,
//         .timer_num = LEDC_TIMER_0,
//         .duty_resolution = LEDC_TIMER_10_BIT,
//         .freq_hz = 5000,
//         .clk_cfg = LEDC_AUTO_CLK,
//     };
//     ledc_timer_config(&blue_ledc_timer);

//     ledc_channel_config_t blue_ledc_channel = {
//         .speed_mode = LEDC_LOW_SPEED_MODE,
//         .channel = BLUE_CHANNEL,
//         .timer_sel = LEDC_TIMER_0,
//         .intr_type = LEDC_INTR_DISABLE,
//         .gpio_num = RGB_LED_PIN, // Adjust according to your setup
//         .duty = 0,
//         .hpoint = 0,
//     };
//     ledc_channel_config(&blue_ledc_channel);
// }

// void setColor(uint8_t red, uint8_t green, uint8_t blue)
// {
//     ledc_set_duty(LEDC_LOW_SPEED_MODE, RED_CHANNEL, red);
//     ledc_set_duty(LEDC_LOW_SPEED_MODE, GREEN_CHANNEL, green);
//     ledc_set_duty(LEDC_LOW_SPEED_MODE, BLUE_CHANNEL, blue);
//     ledc_update_duty(LEDC_LOW_SPEED_MODE, RED_CHANNEL);
//     ledc_update_duty(LEDC_LOW_SPEED_MODE, GREEN_CHANNEL);
//     ledc_update_duty(LEDC_LOW_SPEED_MODE, BLUE_CHANNEL);
// }