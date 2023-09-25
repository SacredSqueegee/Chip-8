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


void emulate_instruction(chip8_t *chip8)
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
    
    // FIXME: Make below work on big/little endian machines
    // Load instruction from little endian host machine RAM into big endian Chip-8 RAM
    chip8->instruction.opcode = chip8->ram[chip8->reg.PC] << 8 | chip8->ram[chip8->reg.PC + 1];
    chip8->reg.PC += 2;
    // Log_Info("Instruction opcode: 0x%04x", instruction.opcode);
    // Log_Info("Instruction lower byte: 0x%02x", instruction.KK);
    // Log_Info("Instruction lower 12 bits: 0x%03x", instruction.NNN);

    

    chip8->state = QUIT;
}
