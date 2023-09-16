#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "main.h"
#include "helpers/logging.h"

// 10-20 seem to be good scaling numbers
#define DISPLAY_SCALE 20
#define DISPLAY_WIDTH 64 * DISPLAY_SCALE
#define DISPALY_HEIGHT 32 * DISPLAY_SCALE

int main()
{

    if (initialize_sdl())
        return 1;

    SDL_Window *window = SDL_CreateWindow(
        "Chip-8 Interpreter",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        DISPLAY_WIDTH,
        DISPALY_HEIGHT,
        SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        Log_Err("could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Event e;
    bool quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                quit = true;
            if (e.type == SDL_KEYDOWN)
                SDL_SetWindowSize(window, DISPLAY_WIDTH, DISPALY_HEIGHT);
            if (e.type == SDL_MOUSEBUTTONDOWN)
                SDL_SetWindowSize(window, DISPLAY_WIDTH/2, DISPALY_HEIGHT/2);
        }
    }

    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}

int initialize_sdl()
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0)
    {
        Log_Err("Could not initialize video subsystem: %s\n", SDL_GetError());
        return 1;
    }
    Log_Info("Initialized: VIDEO, AUDIO, and TIMER modules.");
    return 0;
}
