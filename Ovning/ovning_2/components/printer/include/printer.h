#pragma once // Kan bara inkluderas en gång
#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

namespace printer{
    int func();
    void printNumbers(int start, int stop);
    void takeTimeAndPrintEverySecond(int time);
    int multiply(int a, int b);
}