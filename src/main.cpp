#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "chip8.h"

#include <ncurses.h>
#include <unistd.h>

#define global_variable static
#define internal static

global_variable chip8 Processor;

internal inline void
Fatal(const char *Format, ...)
{
    va_list Args;

    va_start(Args, Format);
    vprintf(Format, Args);
    va_end(Args);

    exit(1);
}

internal inline void
CreateWindow()
{
    WINDOW *Window = initscr();
    cbreak();
    nodelay(Window, TRUE);
    noecho();
    curs_set(FALSE);
    resize_term(DISPLAY_HEIGHT, DISPLAY_WIDTH);
    start_color();
    init_pair(1, COLOR_RED, COLOR_WHITE);
    init_pair(2, COLOR_RED, COLOR_BLACK);
}

internal inline void
CloseWindow()
{
    endwin();
}

int main(int argc, char **argv)
{
    if(argc != 2)
        Fatal("Usage: chip8 /path/to/rom\n");

    Chip8Initialize(&Processor);
    if(!Chip8LoadRom(&Processor, argv[1]))
        Fatal("Failed to load rom: %s\n", argv[1]);

    CreateWindow();
    Processor.Running = true;

    int Counter = 0;
    while(Processor.Running)
    {
        if(Counter++ < 1000)
        {
            Chip8DoCycle(&Processor);

            if(Processor.Draw)
            {
                Chip8DrawGraphics(&Processor);
                Processor.Draw = false;
            }

            Chip8HandleInput(&Processor);
            Counter = 0;
        }
    }

    CloseWindow();
    return 0;
}
