#ifndef DEBUG_MODE
#define DEBUG_MODE 0
#endif

namespace DEBUG
{
    const char quit = 'q';
    const char continue_end = 'c';
    const char continue_until = 'u';
    const char show_register = 'r';
    const char show_memory = 'm';
    
    unsigned short get_break_point();
    char debug_menu();
}
