#ifndef CHIP8_H_IRISH
#define CHIP8_H_IRISH

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    // General Purpose Registers
    union __attribute__((__packed__))
    {
        uint8_t Vx[16];
        struct
        {
            // General Purpose Registers, 8-bit registers
            uint8_t V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, VA, VB, VC, VD, VE;
            // Flag Register, 8-bit - can be general purpose though, but not recommended for GP use
            uint8_t VF;
        };
    };
    
    // General purpose register for storing memory addresses, 16-bit register
    uint16_t I;

    // Special Purpose registers, 8-bit registers
    // DT -> Delay Timer register
    // ST -> Sound Timer register
    uint8_t DT, ST;

    // "Hardware" registers
    // SP -> Stack Pointer register, 8-bit
    // PC -> Program Counter register, 16-bit
    uint8_t SP;
    uint16_t PC;
} Registers_t;

// Emulator states
typedef enum {
    QUIT = 0,       // Ready to quit emulation
    RUNNING,        // Chip-8 is running in real time
    STEPPING,       // Chip-8 is in step-mode, pauses after each instruction
    PAUSED          // chip-8 is paused and doing nothing
} emulator_state_t;

// TODO: verify that bitfields are indeed packed by compiler
// TODO: correct for any endianness issues between big/little
typedef union __attribute__((__packed__))
{
    // Whole 2-bytes of the instruction
    uint16_t opcode;

    // 12-bit address value in the instruction
    struct
    {
        uint16_t NNN: 12;
    };

    // N, X, Y values contained in lower 12-bits of the instruction
    struct
    {
        uint8_t N: 4;   // lowest 4-bits of instruction's low-byte
        uint8_t Y: 4;   // upper 4-bits of instruction's low-byte
        uint8_t X: 4;   // lowest 4-bits of instruction's high-byte
    };

    // KK value contained in the low byte of the instruction
    struct
    {
        uint8_t KK: 8;  // lowest 8-bits of the instruction, the instruction's whole low-byte
    };
} instruction_t;

// CHIP-8 Machine object
typedef struct {
    emulator_state_t state;         // Current state of Chip-8
    Registers_t reg;                // Chip-8 Registers
    uint8_t ram[4096];              // 4 KiB of RAM
    uint16_t stack[16];             // 16 Byte stack for function calling
    uint32_t displaySize;           // Size of the memory of the display
    bool *display;                  // Pointer to display data
    uint16_t displayX;              // number of pixels for x direction of display
    uint16_t displayY;              // number of pixels for y direction of display
    bool displayWrap;               // should the sprites wrap on screen
    bool keypad[16];                // Hexadecimal keypad 0x0-0xF
    uint8_t *textSprites[16][5];    // Pointer to default text sprites
    uint8_t spriteData[16];         // Temporary storage for sprite data
    char *romName;                  // Name of ROM currently loaded
    char *romPath;                  // Path to ROM currently loaded

    uint16_t entrypoint;            // Entrypoint for chip-8 programs
    instruction_t instruction;      // Currently executing instruction
} chip8_t;

// Chip-8 Utility functions
int load_rom(char *romPath, void *dest, int sz_inp, int num_elements);
int emulate_instruction(chip8_t *chip8);
void bad_instruction(uint16_t address, uint16_t opcode);
int validate_PC(chip8_t chip8);
int validate_sprite(chip8_t chip8);

// Chip-8 Instruction functions, too big for switch statement
int draw_instruction(chip8_t *chip8);

#endif