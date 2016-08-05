#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "chip8.h"

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

int main(int argc, char **argv)
{
    if(argc != 2)
        Fatal("Usage: chip8 /path/to/rom\n");

    Chip8Initialize(&Processor);
    if(!Chip8LoadRom(&Processor, argv[1]))
        Fatal("Failed to load rom: %s\n", argv[1]);

    /*
    CreateWindow();
    CreateInput();
    */

    while(1)
    {
        Chip8DoCycle(&Processor);

        if(Processor.Draw)
        {
            Chip8DrawGraphics(&Processor);
            Processor.Draw = false;
        }

        Chip8HandleInput(&Processor);
    }

    return 0;
}
