#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "chip8.h"
#include "helpers/logging.h"

// char *romPath    -> pointer to string of the path to the ROM file
// void *dest       -> pointer to starting address in RAM to load ROM file into
// int sz_inp       -> data width of the elements from the input ROM file
// int num_elements -> number of elements at the dest of size sz_inp
//
// Returns
//      0           -> success
//      *           -> anything else on failure
int load_rom(char *romPath, void *dest, int sz_inp, int num_elements)
{
    // Open ROM in readonly binary mode
    FILE *fp = fopen(romPath, "rb");
    if (fp == NULL)
    {
        Log_Err("Unable to open ROM from: %s", romPath);
        fprintf(stderr, "\t\\_ Error: %i -> %s", errno, strerror(errno));
        return 1;
    }

    // Calculate file and dest size
    const long dest_size = num_elements * sz_inp;

    fseek(fp, 0, SEEK_END);
    const long rom_size = ftell(fp);
    rewind(fp);

    // Error if dest_size is too small to fit ROM 
    if (rom_size > dest_size)
        return Log_Err("Destination size [%i Bytes] is smaller than input ROM size [%i Bytes]", dest_size, rom_size);

    // Load ROM to destination
    if (fread(dest, sz_inp, num_elements, fp) == 0)
        return Log_Err("Error reading ROM from: '%s' or rom is empty", romPath);
    
    // Cleanup and return
    if (fclose(fp) != 0)
        Log_Warn("Unable to close file pointer to ROM: '%s'", romPath);

    return 0;
}


 // Enable/Disable message logging
#define INSTRUCTION_DEBUG
#ifndef INSTRUCTION_DEBUG
#   define Log_Info(...)
#else
#   define Log_Info(...) printf("\n"); Log_Info("Executing instruction: 0x%04X", chip8->instruction.opcode); printf("\t\\_ "); printf(__VA_ARGS__); printf("\n");
#endif
int emulate_instruction(chip8_t *chip8)
{
    // Chip-8 Instruction Reference
    // =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
    // Instruction word size:   2-bytes
    // Instructions stored as:  Big-Endian format
    // Instructions first bytes stored at EVEN address
    //      -> if program includes sprite data it should be padded
    //         so that any instructions following it will be 
    //         properly aligned in RAM
    //
    // Variables used in instruction comments:
    //      - nnn || addr   -> a 12-bit value, lowest 12 bits of the instruction
    //      - n || nibble   -> a 4-bit value, lowest 4 bits of the instruction
    //      - x             -> a 4-bit value, lower 4 bits of the high byte of the instruction
    //      - y             -> a 4-bit value, upper 4 bits of the low byte of the instruction
    //      - kk || byte    -> 8-bit value, the lowest 8 bits of the instruction

    // Make sure PC is set to valid address for instruction execution
    if (validate_PC(*chip8) != 0)
    {
        Log_Err("Fatal error, shutting down...");
        return 1;
    }

    // FIXME: Make below work on big/little endian machines
    // Load instruction from little endian host machine RAM into big endian Chip-8 RAM
    chip8->instruction.opcode = chip8->ram[chip8->reg.PC] << 8 | chip8->ram[chip8->reg.PC + 1];
    chip8->reg.PC += 2;

    // Switch off of upper nibble of instruction
    switch ((chip8->instruction.opcode >> 12) & 0x0F)
    {
        case 0x0:
            // 0x0nnn -> SYS addr - not implemented on newer machines
            // 0x00E0 -> CLS - clear screen
            if (chip8->instruction.KK == 0xE0)
            {
                Log_Info("Clearing screen");
                memset(chip8->display, '\0', chip8->displaySize);
                break;
            }
            // 0x00EE -> RET - return from subroutine
            if (chip8->instruction.KK == 0xEE)
            {
                chip8->reg.PC = chip8->stack[chip8->reg.SP];
                Log_Info("Return to address: 0x%04X", chip8->reg.PC);
                chip8->reg.SP--;
                break;
            }
            bad_instruction(chip8->instruction.opcode);
            break;
        
        case 0x1:
            // 0x1nnn -> JP addr - jump to location nnn
            chip8->reg.PC = chip8->instruction.NNN;
            Log_Info("Jump to address: 0x%04X", chip8->reg.PC);
            break;

        case 0x2:
            // 0x2nnn -> CALL addr - call subroutine at nnn
            chip8->reg.SP++;
            chip8->stack[chip8->reg.SP] = chip8->reg.PC;
            chip8->reg.PC = chip8->instruction.NNN;
            Log_Info("Calling function at: 0x%04X, Pushed address: 0x%04X on stack[0x%01X]", chip8->reg.PC, chip8->stack[chip8->reg.SP], chip8->reg.SP);
            break;
        
        case 0x3:
            break;
        
        default:
            printf("\n");
            bad_instruction(chip8->instruction.opcode);
            break;
    }

    // chip8->state = QUIT;
}
// Re-enable message logging
#ifndef INSTRUCTION_DEBUG
#   undef Log_Info
#endif


void bad_instruction(uint16_t opcode)
{
    printf("\n");
    Log_Warn("Unimplemented instruction: 0x%04X", opcode);
}


// Validate that we are executing a correct address in RAM
int validate_PC(chip8_t chip8)
{
    // Make sure we don't execute outside RAM
    // we do >= of the sizeof(ram)-1 as we don't want to execute if PC >= 4095
    // 4095 is technically a valid RAM address but instructions are aligned to
    // the even address thus executing from the last odd address is not allowed.
    // So, in this case addresses <= 4094 are valid
    if (chip8.reg.PC >= sizeof(chip8.ram) - 1)
        return Log_Err("Chip-8 is trying to execute invalid RAM address: 0x%04X", chip8.reg.PC);

    // Make sure PC is even, as instructions must be aligned to the even address
    if (chip8.reg.PC % 2 == 1)
        return Log_Err("Error, Chip-8 trying to execute non-even RAM address: 0x%04X", chip8.reg.PC);
    
    return 0;
}
