#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "main.h"
#include "chip8.h"
#include "helpers/logging.h"


// 10-20 seem to be good scaling numbers
#define DISPLAY_SCALE 20
#define DISPLAY_WIDTH 64
#define DISPALY_HEIGHT 32


int main()
{
    // initialize emulator configurations/options
    config_t config = (config_t) {
        .window_scale = DISPLAY_SCALE,
        .window_width = DISPLAY_WIDTH,
        .window_height = DISPALY_HEIGHT,
        .fg_color.value = 0xFFFFFFFF,
        .bg_color.value = 0x000000FF
    };

    // Initialize SDL
    sdl_t sdl = {0};
    if (initialize_sdl(&sdl, config))
        return 1;
    
    // Initialize Chip-8 machine
    chip8_t chip8 = {0};
    if (initialize_chip8(&chip8, config))
        return 1;

    // TODO:
    // Get loop start time

    // Main Loop
    chip8.state = RUNNING;
    while (chip8.state == RUNNING)
    {
        // Handle User Input
        handle_input(sdl, config, &chip8);

        // TODO:
        // Check chip-8 state
        // if paused: skip/do something else???...

        // Emulate Chip-8 instructions
        
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

int initialize_chip8(chip8_t *chip8, const config_t config)
{
    printf("\n");
    Log_Info("Creating Chip-8 object...");

    // Allocate memory for display data
    int displaySize = (config.window_height*config.window_width);
    chip8->display = (bool*) calloc(displaySize, sizeof(bool));
    if(chip8->display == NULL)
    {
        Log_Err("Unable to allocate dynamic memory for Chip-8 display");
        return 1;
    }
    Log_Info("Allocated %i [bytes] of display memory", displaySize*sizeof(bool));
    printf("\t\\_ For display of %ix%i\n", config.window_width, config.window_height);

    return 0;       // success
}

void destroy_chip8(chip8_t *chip8)
{
    printf("\n");
    Log_Warn("Destroying Chip-8 object...");

    //de-allocate display memory
    free(chip8->display);
    Log_Info("Freed Chip-8 display memory");

}
