#ifndef MAIN_H_IRISH
#define MAIN_H_IRISH

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

// Configuration Specification Structure
typedef struct
{
    uint8_t window_scale;
    uint32_t window_width;
    uint32_t window_height;

    union
    {
        uint32_t value;
        struct
        {
            uint8_t a, b, g, r;     // reverse order b/c of endianness
        };
        
    }fg_color;

    union
    {
        uint32_t value;
        struct
        {
            uint8_t a, b, g, r;     // reverse orcer b/c of endianness
        };
        
    }bg_color;
} config_t;


// SDL Configuration Information
typedef struct
{
    bool running;
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;


// Emulator states
typedef enum {
    QUIT = 0,
    RUNNING,
    PAUSED
} emulator_state_t;

// CHIP-8 Machine object
typedef struct {
    emulator_state_t state;
} chip8_t;


// forward declerations
// =======================================
void update_screen(sdl_t sdl, const config_t config);
void clear_screen(sdl_t sdl, const config_t config);
int initialize_sdl(sdl_t *sdl, const config_t config);
void cleanup_sdl(sdl_t *sdl);

void handle_input(sdl_t *sdl, const config_t config);

int initialize_chip8(chip8_t *chip8);

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