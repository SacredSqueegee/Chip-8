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
