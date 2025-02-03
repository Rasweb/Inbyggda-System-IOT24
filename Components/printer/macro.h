#define ANSI_RED "\033[0;31m" // Color code for red
#define ANSI_GREEN "\033[0;32m" // Color code for green
#define ANSI_YELLOW "\033[0;33m" // Color code for yellow
#define ANSI_BLUE "\033[0;34m" // Color code for blue
#define ANSI_MAGENTA "\033[0;35m" // Color code for magenta
#define ANSI_CYAN "\033[0;36m" // Color code for cyan
#define ANSI_RESET "\033[0m" // Resets to normal colour 
#define ANSI_RICH_BG "\e[0;106m	"; // High intensity BG
#define HIGH_BLACK "\e[0;90m";	
// \e[0;91m	Red
// \e[0;92m	Green
// \e[0;93m	Yellow
// \e[0;94m	Blue
// \e[0;95m	Purple
// \e[0;96m	Cyan
// \e[0;97m	White

#define TASK_DELAY_MS(ms) \
    vTaskDelay(ms/portTICK_PERIOD_MS);

#define PRINTF_COLOR(color, format_string, ...)\
    printf("%s" format_string "%s", color, ##__VA_ARGS__, ANSI_RESET); 


