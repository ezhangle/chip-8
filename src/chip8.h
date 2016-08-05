#ifndef CHIP_8
#define CHIP_8

struct chip8
{
    /* NOTE(koekeishiya): Current Opcode. The chip-8 has 35opcodes, all two bytes long. */
    unsigned short Opcode;

    /* NOTE(koekeishiya): The chip-8 has 4096 memory locations, all of which are 8-bits.
     * The interpreter itself occupies the first 512 bytes of memory and programs therefore
     * begin at memory location 512 (0x200). The uppermost 256 bytes are reserved for display
     * refresh, and the 96 bytes below that are reserved for the call stack, internal use and
     * other variables. */
    unsigned char Memory[0x1000];

    /* NOTE(koekeishiya): 16-bits wide address register, used with opcodes that
     * involve memory operations. */
    unsigned short I;

    /* NOTE(koekeishiya): 16-bits wide program counter. */
    unsigned short Pc;

    /* NOTE(koekeishiya): 8-bit data registers named from V0 to VF.
     * The VF register doubles as a carry flag. */
    unsigned char V[16];

    /* NOTE(koekeishiya): The chip-8 graphics system is black & white and has a total
     * of 2048 pixels. Drawing is done in XOR mode, setting the VF register when a pixel
     * is turned off. This is used for collision detection. */
    unsigned char Graphics[64 * 32];

    /* NOTE(koekeishiya): The stack is only used to store return addresses when
     * subroutines are called. must have at least 16 levels. */
    unsigned short Stack[16];
    unsigned short Sp;

    /* NOTE(koekeishiya): Timer registers count at 60 Hz. When greater than zero, count to zero.
     * Buzzer sounds whenever the sound timer reaches zero. */
    unsigned char DelayTimer;
    unsigned char SoundTimer;

    /* NOTE(koekeishiya): The chip-8 has a HEX based keypad. */
    unsigned char Key[16];

    /* NOTE(koekeishiya): We do not draw every cycle! */
    bool Draw;
};

void Chip8Initialize(chip8 *Processor);

bool Chip8LoadRom(chip8 *Processor, const char *Rom);

void Chip8DoCycle(chip8 *Processor);

void Chip8DrawGraphics(chip8 *Processor);

void Chip8HandleInput(chip8 *Processor);

#endif
