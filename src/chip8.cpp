#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "chip8.h"

static unsigned char Chip8Font[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void Chip8Initialize(chip8 *Processor)
{
    /* NOTE(koekeishiya): Clear entire processor struct to zero. */
    memset(Processor, 0, sizeof(chip8));


    /* NOTE(koekeishiya): Reset program counter to point to our starting location
     * and load font-data into memory. */
    Processor->Pc = 0x200;
    for(int Index = 0; Index < 80; ++Index)
        Processor->Memory[Index] = Chip8Font[Index];
}

bool Chip8LoadRom(chip8 *Processor, const char *Rom)
{
    FILE *FileHandle = fopen(Rom, "rb");
    char *Contents = NULL;
    unsigned int Length = 0;

    if(FileHandle)
    {
        fseek(FileHandle, 0, SEEK_END);
        Length = ftell(FileHandle);

        fseek(FileHandle, 0, SEEK_SET);
        Contents = (char *) malloc(Length + 1);

        fread(Contents, Length, 1, FileHandle);
        Contents[Length] = '\0';
        fclose(FileHandle);

        for(int Index = 0; Index < Length; ++Index)
            Processor->Memory[0x200 + Index] = Contents[Index];

        free(Contents);
        return true;
    }
    else
    {
        printf("Could not open file: %s\n", Rom);
        return false;
    }
}

void Chip8DoCycle(chip8 *Processor)
{
    Processor->Opcode = Processor->Memory[Processor->Pc] << 8 |
                        Processor->Memory[Processor->Pc + 1];

    /* NOTE(koekeishiya): Do we want to increment instantly, or increment
     * when executing the opcode ? */
    Processor->Pc += 2;

    /* NOTE(koekeishiya): The first 4-bits decide the operation. */
    switch(Processor->Opcode & 0xF000)
    {
        case 0x0000: // Behaviour depends on least-significant-bits.
        {
            switch(Processor->Opcode)
            {
                case 0x00E0: // 00E0: Clears the screen.
                {
                } break;
                case 0x00EE: // 00EE: Returns from subroutine.
                {
                    Processor->Pc = Processor->Stack[--Processor->Sp];
                } break;
                default: // 0NNN: Calls RCA 1802 program at address NNN. Not necessary for most ROMs.
                {
                } break;
            }
        } break;
        case 0x1000: // 0x1NNN: Jumps to address NNN.
        {
            Processor->Pc = Processor->Opcode & 0x0FFF;
        } break;
        case 0x2000: // 0x2NNN: Calls subroutine at address NNN.
        {
            Processor->Stack[Processor->Sp++] = Processor->Pc;
            Processor->Pc = Processor->Opcode & 0x0FFF;
        } break;
        case 0xA000: // ANNN: Sets I to the address NNN
        {
            Processor->I = Processor->Opcode & 0x0FFF;
        } break;
        default:
        {
            printf("Not yet implemented: 0x%X\n", Processor->Opcode);
        } break;
    }

    /* TODO(koekeishiya): Times should count down at 60hz,
     * implement a way to execute 60opcodes per second. */
    if(Processor->DelayTimer > 0)
    {
        --Processor->DelayTimer;
    }

    if(Processor->SoundTimer > 0)
    {
        if(Processor->SoundTimer == 1)
            printf("Make buzzer sound!\n");

        --Processor->SoundTimer;
    }

}

void Chip8DrawGraphics(chip8 *Processor)
{
}

void Chip8HandleInput(chip8 *Processor)
{
}
