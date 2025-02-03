#include "printer.h"
#include "macro.h"

namespace printer{
    int func(){
        return CONFIG_PRINT_LEVEL;
    }
    
    void printNumbers(int start, int stop){
        while(start <= stop) {
            ESP_LOGI("Nr", "%d", start);
            start++;
        }        
    }   
    
    void takeTimeAndPrintEverySecond(int time){
        int timeElapsed = 1000;
        while(timeElapsed <= time){
            PRINTF_COLOR(ANSI_CYAN, "string" "%s", "test");
            printf("%d", timeElapsed);
            TASK_DELAY_MS(1000);
            timeElapsed+=1000;
        }
    }

    int multiply(int a, int b)
    {
        return a * b;
    }

}
