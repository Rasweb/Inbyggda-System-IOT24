#include <stdio.h>
#include "esp_log.h"

#define ANSI_RED "\033[0;31m" // Color code for red
#define ANSI_GREEN "\033[0;32m" // Color code for green
#define ANSI_YELLOW "\033[0;33m" // Color code for yellow
#define ANSI_BLUE "\033[0;34m" // Color code for blue
#define ANSI_MAGENTA "\033[0;35m" // Color code for magenta
#define ANSI_CYAN "\033[0;36m" // Color code for cyan
#define ANSI_RESET "\033[0m" // Resets to normal colour 
#define ANSI_RICH_BG "\e[0;106m	"; // High intensity BG
#define NEW_LINE "\n"  
#define NEW_WORLD_COUNT 1 

// Macro function like printf
// Backslash for newline
// __VA_ARGS__ for variadic functions, unkown parameters
#define PRINTF_COLOR(color, format_string, ...) \
    printf("%s" format_string "%s", color, ##__VA_ARGS__, ANSI_RESET); 

#define PRINTF_GROUP_1(format, ...) \
    PRINTF_COLOR(ANSI_BLUE, "Group 1 "); \
    PRINTF_COLOR(ANSI_CYAN, format, ##__VA_ARGS__);

#define PRINTF_COLOR_WITH_LINE(color, format_string, ...) \
    PRINTF_COLOR(ANSI_MAGENTA, "[%s:%d] ", __FILE__, __LINE__); \
    PRINTF_COLOR(color, format_string, ##__VA_ARGS__);

#define RASSE(color, format_string, ...)\
    PRINTF_COLOR(color, format_string, ##__VA_ARGS__);

const static char* TAG = "MAIN";

void app_main(void)
{
    // ESP_LOGI(TAG, "Hello World for my %d time", NEW_WORLD_COUNT); // Prints text to the console using ESP log
    // printf("\033[0;31mtest"); // A color code for red, test will print out red

   const char* test_string = "test";
   PRINTF_COLOR(ANSI_RED, "%s" NEW_LINE, test_string);

   PRINTF_GROUP_1("Hello World %d" NEW_LINE, NEW_WORLD_COUNT);

   PRINTF_COLOR_WITH_LINE(ANSI_GREEN, "Hello world %d" NEW_LINE, NEW_WORLD_COUNT);
   
//    RASSE(ANSI_CYAN, "%d", 12, "%s", "hej");
   RASSE(ANSI_CYAN, "waka" "%s", "waka");

}
