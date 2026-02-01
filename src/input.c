#include <stdio.h>
#include <stdbool.h>
#include "SDL2/SDL.h"
#include "main.h"

void sdl_input_handle(chip8_t* chip8){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if (event.type == SDL_KEYDOWN){
            switch(event.key.keysym.sym){
                case SDLK_ESCAPE:
                    exit(EXIT_SUCCESS);
                    break;
                case SDLK_1:
                    chip8->keypad[0x1] = 1;
                    break;
                case SDLK_2:
                    chip8->keypad[0x2] = 1;
                    break;
                case SDLK_3:
                    chip8->keypad[0x3] = 1;
                    break;
                case SDLK_4:
                    chip8->keypad[0xC] = 1;
                    break;
                case SDLK_q:
                    chip8->keypad[0x4] = 1;
                    break;
                case SDLK_w:
                    chip8->keypad[0x5] = 1;
                    break;
                case SDLK_e:
                    chip8->keypad[0x6] = 1;
                    break;
                case SDLK_r:
                    chip8->keypad[0xD] = 1;
                    break;
                case SDLK_a:
                    chip8->keypad[0x7] = 1;
                    break;
                case SDLK_s:
                    chip8->keypad[0x8] = 1;
                    break;
                case SDLK_d:
                    chip8->keypad[0x9] = 1;
                    break;
                case SDLK_f:
                    chip8->keypad[0xE] = 1;
                    break;
                case SDLK_z:
                    chip8->keypad[0xA] = 1;
                    break;
                case SDLK_x:
                    chip8->keypad[0x0] = 1;
                    break;
                case SDLK_c:
                    chip8->keypad[0xB] = 1;
                    break;
                case SDLK_v:
                    chip8->keypad[0xF] = 1;
                    break;
                default:
                    break;
            }
        } else
        if (event.type == SDL_KEYUP){
            switch(event.key.keysym.sym){
                case SDLK_1:
                    chip8->keypad[0x1] = 0;
                    break;
                case SDLK_2:
                    chip8->keypad[0x2] = 0;
                    break;
                case SDLK_3:
                    chip8->keypad[0x3] = 0;
                    break;
                case SDLK_4:
                    chip8->keypad[0xC] = 0;
                    break;
                case SDLK_q:
                    chip8->keypad[0x4] = 0;
                    break;
                case SDLK_w:
                    chip8->keypad[0x5] = 0;
                    break;
                case SDLK_e:
                    chip8->keypad[0x6] = 0;
                    break;
                case SDLK_r:
                    chip8->keypad[0xD] = 0;
                    break;
                case SDLK_a:
                    chip8->keypad[0x7] = 0;
                    break;
                case SDLK_s:
                    chip8->keypad[0x8] = 0;
                    break;
                case SDLK_d:
                    chip8->keypad[0x9] = 0;
                    break;
                case SDLK_f:
                    chip8->keypad[0xE] = 0;
                    break;
                case SDLK_z:
                    chip8->keypad[0xA] = 0;
                    break;
                case SDLK_x:
                    chip8->keypad[0x0] = 0;
                    break;
                case SDLK_c:
                    chip8->keypad[0xB] = 0;
                    break;
                case SDLK_v:
                    chip8->keypad[0xF] = 0;
                    break;
                default:
                    break;
            }
        }
    }
}
