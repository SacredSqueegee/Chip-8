#ifndef MAIN_H_IRISH
#define MAIN_H_IRISH

#include <stdint.h>

struct Registers {
    // General Purpose Registers, 8-bit registers
    uint8_t V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, VA, VB, VC, VD, VE;
    // General purpose register for storing memory addresses, 16-bit register
    uint16_t VI;
    // Flag Register, 8-bit - can be general purpose though, but not recommended for GP use
    uint8_t VF;

    // Special Purpose registers, 8-bit registers
    // DT -> Delay Timer register
    // ST -> Sound Timer register
    uint8_t DT, ST;

    // "Hardware" registers
    // SP -> Stack Pointer register, 8-bit
    // PC -> Program Counter register, 16-bit
    uint8_t SP;
    uint16_t PC;
};

struct Chip_8 {
    // Chip-8 registers
    struct Registers reg;

    // Chip-8 Memory, 4KiB -> 4096-bytes
    uint8_t ram[4096];
};

#endif