#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "main.h"
#include "chip8.h"
#include "helpers/logging.h"


// 10-20 seem to be good scaling numbers
#define DISPLAY_SCALE 20
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

int main(int argc, char *argv[])
{
    // TODO: Add default usage statement...

    // initialize emulator configurations/options
    config_t config = (config_t) {
        .window_scale = DISPLAY_SCALE,
        .window_width = DISPLAY_WIDTH,
        .window_height = DISPLAY_HEIGHT,
        .fg_color.value = 0xFFFFFFFF,
        .bg_color.value = 0x000000FF,
        .rom_path = "./roms/",
        .config_path = "./configs/",
        .text_rom_name = "textSprites.bin",
        .entrypoint = 0x200
    };

    // Initialize SDL
    sdl_t sdl = {0};
    if (initialize_sdl(&sdl, config))
        return 1;
    
    // Get ROM name from cli args
    char *romName = "test/IBM Logo.ch8";
    if (argc > 1)
        romName = argv[1];
    // Log_Info("Loading ROM: %s", romName);
    
    // Initialize Chip-8 machine
    chip8_t chip8 = {0};
    if (initialize_chip8(&chip8, config, romName))
        return 1;

    // TODO:
    // Get loop start time

    // Main Loop
    while (chip8.state != QUIT)
    {
        // Handle User Input
        handle_input(sdl, config, &chip8);

        // TODO:
        // Check chip-8 state
        // if paused: skip/do something else???...
        // FIXME: need to pause audio during this as well...
        if (chip8.state == PAUSED) continue;

        // Emulate Chip-8 instructions
        emulate_instruction(&chip8);
        
        // Update window with changes
        update_screen(sdl, config);       

        // TODO:
        // Get elapsed time and
        // Delay for 60hz/60fps
        SDL_Delay(16);

    } // ~Main Loop

    destroy_chip8(&chip8);
    cleanup_sdl(&sdl);
    return 0;
}

void update_screen(sdl_t sdl, const config_t config)
{
        clear_screen(sdl, config);

        // Draw black rectangle
        SDL_SetRenderDrawColor(sdl.renderer, config.fg_color.r, config.fg_color.g, config.fg_color.b, config.fg_color.a);
        // SDL_FRect rect = {0*config.window_scale, 0*config.window_scale, 1*config.window_scale, 1*config.window_scale};
        // SDL_RenderFillRectF(sdl.renderer, &rect);

        // Draw checkerboard pattern
        for(uint32_t i=0; i<config.window_height; i++)
        {
            for(uint32_t j=0; j<config.window_width; j++)
            {
                if(j%2==0 && i%2==1)
                    continue;
                if(j%2==1 && i%2==0)
                    continue;
                SDL_FRect rect = {j*config.window_scale, i*config.window_scale, 1*config.window_scale, 1*config.window_scale};
                SDL_RenderFillRectF(sdl.renderer, &rect);
            }
        }

        // Display renderer to window
        SDL_RenderPresent(sdl.renderer);
}

void clear_screen(sdl_t sdl, const config_t config)
{
    // Clear screen to background color
    SDL_SetRenderDrawColor(sdl.renderer, config.bg_color.r, config.bg_color.g, config.bg_color.b, config.bg_color.a);
    SDL_RenderClear(sdl.renderer);
}

int initialize_sdl(sdl_t *sdl, const config_t config)
{
    // Initialize sub-systems
    if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0)
    {
        Log_Err("Could not initialize video subsystem: %s", SDL_GetError());
        return 1;
    }
    Log_Info("Initialized: VIDEO, EVENT, AUDIO, and TIMER modules");

    // Create window
    sdl->window = SDL_CreateWindow(
        "Chip-8 Interpreter",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config.window_width * config.window_scale,
        config.window_height * config.window_scale,
        SDL_WINDOW_SHOWN
    );
    if (sdl->window == NULL)
    {
        Log_Err("Could not create window: %s", SDL_GetError());
        return 1;
    }
    Log_Info("Created window");

    // Create renderer
    sdl->renderer = SDL_CreateRenderer(
        sdl->window,
        -1,
        SDL_RENDERER_ACCELERATED
    );
    if(sdl->renderer == NULL)
    {
        Log_Err("Could not create renderer: %s", SDL_GetError());
    }
    Log_Info("Created renderer");

    return 0;
}

void cleanup_sdl(sdl_t *sdl)
{
    printf("\n");
    Log_Warn("Shutting down app...");
    SDL_DestroyRenderer(sdl->renderer);
    Log_Info("Destroyed Renderer");
    SDL_DestroyWindow(sdl->window);
    Log_Info("Destroyed Window");
    SDL_Quit();
    Log_Info("Shutdown sub-modules and SDL");
}

void handle_input(sdl_t sdl, const config_t config, chip8_t *chip8)
{
    SDL_Event e;

    // Poll for and handle events
    while (SDL_PollEvent(&e))
    {        
        switch (e.type)
        {
            case SDL_QUIT:
                chip8->state = QUIT;
                break;
            
            case SDL_KEYDOWN:
                // switch of the specific key
                switch(e.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        chip8->state = QUIT;
                        break;
                    
                    case SDLK_SPACE:
                        // Disable toggling when key held
                        if (e.key.repeat == 1) continue;

                        if (chip8->state == RUNNING)
                        {
                            chip8->state = PAUSED;
                            Log_Info("Chip-8 is now: PAUSED");
                            break;
                        }
                        chip8->state = RUNNING;
                        Log_Info("Chip-8 is now: RUNNING");
                        break;
                    
                    default:
                        SDL_SetWindowSize(sdl.window, config.window_width*config.window_scale/2, config.window_height*config.window_scale/2);
                        break;
                }
                break;
            
            case SDL_KEYUP:
                SDL_SetWindowSize(sdl.window, config.window_width*config.window_scale, config.window_height*config.window_scale);
                break;
            
            default:
                break;
        }
    } // ~Poll Events
}

int initialize_chip8(chip8_t *chip8, const config_t config, char *romName)
{
    printf("\n");
    Log_Info("Creating Chip-8 object...");

    // Allocate memory for display data
    // +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
    int displaySize = (config.window_height*config.window_width);
    chip8->display = (bool*) calloc(displaySize, sizeof(bool));
    if(chip8->display == NULL)
        return Log_Err("Unable to allocate dynamic memory for Chip-8 display");
    
    Log_Info("Allocated %i [bytes] of display memory", displaySize*sizeof(bool));
    printf("\t\\_ For display of %ix%i\n", config.window_width, config.window_height);

    // Load Font
    // +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
    // allocate +1 more memory for \0; calloc zeros memory so no need to add \0 to end
    int textRomPathLen = strlen(config.text_rom_name) + strlen(config.config_path) + 1;
    char *textRomPath = (char*) calloc(textRomPathLen, sizeof(char));
    if(textRomPath == NULL)
        return Log_Err("Unable to allocate dynamic memory for Chip-8 textRomPath");
    Log_Info("Allocated %i [bytes] of textRomPath memory", textRomPathLen*sizeof(char));

    strcpy(textRomPath, config.config_path);
    strcat(textRomPath, config.text_rom_name);

    int num_elements = sizeof(chip8->textSprites) / sizeof(chip8->textSprites[0][0]);
    if (load_rom(textRomPath, chip8->textSprites, sizeof(uint8_t), num_elements) != 0)
        return 1;
    Log_Info("Loaded text sprite data file '%s', from '%s', into Chip-8 textSprites ROM", config.text_rom_name, textRomPath);

    free(textRomPath);
    Log_Info("Freed Chip-8 textRomPath memory");

    // Load Rom to Chip-8 Memory
    // +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
    // Create path to ROM
    chip8->romName = romName;

    // allocate +1 more memory for \0; calloc zeros memory so no need to add \0 to end
    int romPathLen = strlen(chip8->romName) + strlen(config.rom_path) + 1;
    chip8->romPath = (char*) calloc(romPathLen, sizeof(char));
    if(chip8->romPath == NULL)
        return Log_Err("Unable to allocate dynamic memory for Chip-8 romPath");
    Log_Info("Allocated %i [bytes] of romPath memory", (romPathLen*sizeof(char)));

    strcpy(chip8->romPath, config.rom_path);
    strcat(chip8->romPath, chip8->romName);

    // Set entrypoint from config data
    chip8->entrypoint = config.entrypoint;
    Log_Info("Setting entrypoint to %#03x", chip8->entrypoint);

    // Read ROM file into RAM
    void *ramEntry_ptr = &(chip8->ram[chip8->entrypoint]);
    int programSize = 4096-(chip8->entrypoint);
    if(load_rom(chip8->romPath, ramEntry_ptr, sizeof(uint8_t), programSize) != 0)
        return 1;
    Log_Info("Loaded ROM: '%s', from: '%s', into RAM", chip8->romName, chip8->romPath);

    // Set chip-8 defaults
    // +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
    chip8->state = RUNNING;             // Default Chip-8 state to on/running
    chip8->reg.PC = chip8->entrypoint;  // Default PC to RAM entrypoint


    return 0;       // success
}

void destroy_chip8(chip8_t *chip8)
{
    printf("\n");
    Log_Warn("Destroying Chip-8 object...");

    //de-allocate display memory
    free(chip8->display);
    Log_Info("Freed Chip-8 display memory");

    // de-allocate romPath memory
    free(chip8->romPath);
    Log_Info("Freed Chip-8 romPath memory");

}
