#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "main.h"
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

    sdl_t sdl = {0};
    if (initialize_sdl(&sdl, config))
        return 1;

    // Main Loop
    SDL_Event e;
    bool quit = false;
    while (!quit)
    {
        // Delay for 60hz/60fps
        // Update window with changes
        
        clear_screen(sdl, config);

        // Draw black rectangle
        SDL_SetRenderDrawColor(sdl.renderer, config.fg_color.r, config.fg_color.g, config.fg_color.b, config.fg_color.a);
        SDL_FRect rect = {0*config.window_scale, 0*config.window_scale, 10*config.window_scale, 10*config.window_scale};
        SDL_RenderFillRectF(sdl.renderer, &rect);

        // Poll for and handle events
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                quit = true;
            if (e.type == SDL_KEYDOWN)
                SDL_SetWindowSize(sdl.window, config.window_width*config.window_scale, config.window_height*config.window_scale);
            if (e.type == SDL_MOUSEBUTTONDOWN)
                SDL_SetWindowSize(sdl.window, config.window_width*config.window_scale/2, config.window_height*config.window_scale/2);
        } // ~Poll Events

        // Display renderer to window
        SDL_RenderPresent(sdl.renderer);

    } // ~Main Loop

    cleanup_sdl(&sdl);
    return 0;
}

void clear_screen(sdl_t sdl, const config_t config)
{
    // Clear screen to background color
    SDL_SetRenderDrawColor(sdl.renderer, config.bg_color.r, config.bg_color.g, config.bg_color.b, config.bg_color.a);
    SDL_RenderClear(sdl.renderer);
}


int initialize_sdl(sdl_t *sdl, config_t config)
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
