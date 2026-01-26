#include <stdio.h>
#include <stdbool.h>
#include "SDL2/SDL.h"
/*typedef struct {
    state
} chip8*/
int main(int argc, char* argv[]) {
    (void) argc;
    (void) argv;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Chip 8 emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, 0);
    if (!window)
    {
        SDL_Log("error %s\n", SDL_GetError());
        return 1;
    }
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 200, 100, 50, 255);
    while(true){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if (event.type == SDL_KEYDOWN)
            {
                exit(EXIT_SUCCESS);
            }
            
        }
        SDL_RenderClear(renderer);
        SDL_Delay(16);
        SDL_RenderPresent(renderer);

    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}