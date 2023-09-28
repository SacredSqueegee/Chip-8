#include <stdio.h>
#include <stdint.h>
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

    // need to save current address as we replace the Log_Info() function after PCs modification
    // during some instructions
    uint16_t currentAddress = chip8->reg.PC;

    // Enable/Disable message logging
    #define INSTRUCTION_DEBUG
    #ifndef INSTRUCTION_DEBUG
    #   define Log_Info(...)
    #else
    #   define Log_Info(...) printf("\n"); Log_Info("Address: 0x%04X, Opcode: 0x%04X", currentAddress, chip8->instruction.opcode); printf("\t\\_ "); printf(__VA_ARGS__); printf("\n");
    #endif

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
            bad_instruction(currentAddress, chip8->instruction.opcode);
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
        
        case 0x6:
            // 0x6xkk -> LD Vx, byte
            chip8->reg.Vx[chip8->instruction.X] = chip8->instruction.KK;
            Log_Info("Set V%01x to value: 0x%02X", chip8->instruction.X, chip8->instruction.KK);
            break;
        
        case 0x7:
            // 0x7xkk -> ADD Vx, byte
            chip8->reg.Vx[chip8->instruction.X] += chip8->instruction.KK;
            Log_Info("Add 0x%02X to register V%1x, V%1x value: 0x%02X", chip8->instruction.KK, chip8->instruction.X, chip8->instruction.X, chip8->reg.Vx[chip8->instruction.X]);
            break;
        
        case 0xA:
            // 0xAnnn -> LD I, addr
            chip8->reg.I = chip8->instruction.NNN;
            Log_Info("Set I to value: 0x%03X", chip8->reg.I);
            break;
        
        case 0xD:
            // 0xDxyn -> DRW Vx, Vy, nibble
            if (draw_instruction(chip8, currentAddress) != 0)
                return Log_Err("Fatal error, shutting down...");
            break;
        
        default:
            bad_instruction(currentAddress, chip8->instruction.opcode);
            break;
    }

    return 0;
    // Re-enable message logging / clear shenanigans from above
    #undef Log_Info
}



void bad_instruction(uint16_t address, uint16_t opcode)
{
    printf("\n");
    Log_Warn("Address: 0x%04X, Unimplemented instruction: 0x%04X", address, opcode);
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

// Validate that the sprite instruction is correct
int validate_sprite(chip8_t chip8)
{
    // Verify that n is within sprite size limits
    if (chip8.instruction.N > 15)
        return Log_Err("Sprite of size 0x%1X too big. Must be <= 15", chip8.instruction.N);

    return 0;
}

int draw_instruction(chip8_t *chip8, uint16_t currentAddress)
{
    // Validate size of sprite is <= 15
    if (validate_sprite(*chip8) != 0)
        return 1;

    // start_x/y are the starting display coordinates of the sprite on screen
    uint16_t start_x = chip8->reg.Vx[chip8->instruction.X] % chip8->displayX;
    uint16_t start_y = chip8->reg.Vx[chip8->instruction.Y] % chip8->displayY;
    // reset flags
    chip8->reg.VF = 0;

    Log_Info("Drawing sprite at location: 0x%03X, of size: [8-bits x %i] , at display coordinates V%1X(x): %i, V%1X(y): %i", chip8->reg.I, chip8->instruction.N, chip8->instruction.X, start_x, chip8->instruction.Y, start_y);

    // 2D loop over sprite in sprite coordinates -> (pixel, row)
    for (int row=0; row<chip8->instruction.N; row++)
    {
        // Read in current row sprite data from RAM
        uint8_t spriteData = chip8->ram[chip8->reg.I + row];

        // Convert sprite coordinates to display coordinates
        uint16_t disp_y = (start_y + row);
        if (chip8->displayWrap)
            disp_y %= chip8->displayY;      // wrap sprite

        // loop over pixels in current row
        for (int pixel=0; pixel<8; pixel++)
        {
            // Convert sprite coordinates to display coordinates
            uint16_t disp_x = (start_x + pixel);
            if (chip8->displayWrap)
                disp_x %= chip8->displayX;  // wrap sprite
            
            // if we are not wrapping make sure that the display pixel is within the screen
            // other wise skip to next row
            if (disp_x >= chip8->displayX)
                break;

            // get the current state of the pixel being displayed
            int disp_Index = (disp_y * chip8->displayX) + disp_x;
            bool currentPixel = *(chip8->display + disp_Index);

            // FIXME: You dumb shit, you reversed the bytes here...
            //          with your bad bit shift...
            //          hours wasted debugging this: 3.5 hrs
            // ============================================================================================
            // get the current state of the pixel in the sprite
            // bool spritePixel = (spriteData >> pixel) & 0x01;
            
            // get the current state of the pixel in the sprite
            bool spritePixel = (spriteData >> (7 - pixel)) & 0x01;

            // Update pixels of Vf, we update pixels by XORing the specified sprite to the screen
            // Vf is only set when both the screen pixel and the sprite pixel are both on
            // Check pixel collision
            if (spritePixel && currentPixel)
            {
                *(chip8->display + disp_Index) = false;
                chip8->reg.VF = 1;
            }
            // Check if need to write pixel
            else if (spritePixel && !currentPixel)
            {
                // *(currentPixelAddr) = true;
                *(chip8->display + disp_Index) = true;
            }
        }
    }
    return 0;
}
