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
        struct __attribute__((packed))
        {
            // reverse order b/c of endianness
            uint8_t a: 8;
            uint8_t b: 8;
            uint8_t g: 8;
            uint8_t r: 8;
        };
        
    }fg_color;
    union
    {
        uint32_t value;
        struct __attribute__((packed))
        {
            // reverse order b/c of endianness
            uint8_t a: 8;
            uint8_t b: 8;
            uint8_t g: 8;
            uint8_t r: 8;
        };
        
    }bg_color;

    char *rom_path;

    char *config_path;
    const char *text_rom_name;
    
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
void update_screen(sdl_t sdl, const config_t config, bool *display);
void sdl_clear_screen(sdl_t sdl, const config_t config);
int initialize_sdl(sdl_t *sdl, const config_t config);
void cleanup_sdl(sdl_t *sdl);

void handle_input(sdl_t sdl, const config_t config, chip8_t *chip8);

int initialize_chip8(chip8_t *chip8, const config_t config, char *romName);
void destroy_chip8(chip8_t *chip8);

#endif