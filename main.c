#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"

int main()
{

    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
    {
        printf("Could not initialize video subsystem: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Chip-8 Interpreter",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,
        480,
        SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        printf("could not create window: %s\n", SDL_GetError());
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
                quit = true;
            if (e.type == SDL_MOUSEBUTTONDOWN)
                quit = true;
        }
    }

    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
