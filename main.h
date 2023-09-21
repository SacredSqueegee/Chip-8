#ifndef MAIN_H_IRISH
#define MAIN_H_IRISH

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "chip8.h"

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

    char *config_path;
    char *rom_path;

    const uint16_t entrypoint;

} config_t;


// SDL Configuration Information
typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;


// forward declarations
// =======================================
void update_screen(sdl_t sdl, const config_t config);
void clear_screen(sdl_t sdl, const config_t config);
int initialize_sdl(sdl_t *sdl, const config_t config);
void cleanup_sdl(sdl_t *sdl);

void handle_input(sdl_t sdl, const config_t config, chip8_t *chip8);

int initialize_chip8(chip8_t *chip8, const config_t config, char *romName);
void destroy_chip8(chip8_t *chip8);

#endif