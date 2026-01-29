#include <stdio.h>
#include <stdbool.h>
#include "SDL2/SDL.h"
typedef struct{
    uint16_t opcode;
    uint16_t NNN; // 12 bit address
    uint8_t NN; // 8 bit constant
    uint8_t N; // 4bit constant
    uint8_t X; // 4 bit register ids
    uint8_t Y; //
}instruction_t;
typedef struct {
    uint8_t memory[4096];
    uint8_t registers[16];
    uint16_t index_register;
    uint16_t program_counter;
    uint16_t stack[16];
    uint16_t* stack_pointer;
    uint8_t timer;
    uint8_t sound_timer;
    bool display[64*32];
    uint8_t keypad[16];
    instruction_t instruction;
} chip8_t;
#ifdef DEBUG
void print_debug_info(chip8_t chip8){

}
#endif
void emulate_instruction(chip8_t* chip8){
    chip8->instruction.opcode = (chip8->memory[chip8->program_counter] << 8) | chip8->memory[chip8->program_counter+1];
    chip8->program_counter+=2;
    chip8->instruction.NNN = chip8->instruction.opcode & 0x0FFF;
    chip8->instruction.NN = chip8->instruction.opcode & 0x0FF;
    chip8->instruction.N = chip8->instruction.opcode & 0x0F;
    chip8->instruction.X = (chip8->instruction.opcode >> 8) & 0x0F;
    chip8->instruction.Y = (chip8->instruction.opcode >> 4) & 0x0F;
    printf("opcode: %X program counter: %X \n", chip8->instruction.opcode, chip8->program_counter);
    switch ((chip8->instruction.opcode >> 12) & 0x0F)
    {
    case 0x00:
        if (chip8->instruction.NN == 0xE0)
        {
            memset(&chip8->display[0], false, sizeof chip8->display);
        } else if (chip8->instruction.NN == 0xEE){
            // RET
            chip8->program_counter = *--chip8->stack_pointer;
        }
        break;
    case 0x01:
        // JMP
        chip8->program_counter = chip8->instruction.NNN;
        break;
    case 0x02:
        // CALL
        *chip8->stack_pointer++ = chip8->program_counter;
        chip8->program_counter = chip8->instruction.NNN;
        break;
    case 0xA:
        //set I
        chip8->index_register = chip8->instruction.NNN;
        break;
    case 0x6:
        chip8->registers[chip8->instruction.X] = chip8->instruction.NN;
        break;
    case 0x7:
        chip8->registers[chip8->instruction.X] += chip8->instruction.NN;
        break;
    case 0xD:
        uint8_t X_coord = chip8->registers[chip8->instruction.X] & 63;
        uint8_t Y_coord = chip8->registers[chip8->instruction.Y] & 31;
        chip8->registers[0xF] = 0;
        const uint8_t orgX = X_coord;
        for (size_t i = 0; i < chip8->instruction.N; i++)
        {
            X_coord = orgX;
            const uint8_t sprite_data = chip8->memory[chip8->index_register+i];
            for (int8_t j = 7; j >= 0; j--)
            {
                bool* pixel = &chip8->display[Y_coord * 64 + X_coord];
                const bool sprite_bit = (sprite_data & (1 << j));
                // If both sprite is on and display pixel is on, Carry flag is on
                if (sprite_bit && *pixel)
                {
                    chip8->registers[0xF] = 1;
                }
                // XOR display pixels with sprite
                *pixel ^= sprite_bit;
                if(++X_coord >= 64) break;

            }
            if (++Y_coord >= 32) break;
        }
        break;
    default:
        printf("Unimplemented instruction 0x%04x \n", chip8->instruction.opcode);
        break;
    }
    
}
int loadRom(chip8_t* chip8, const char * romname){
    
    const int entry_point = 0x200;
    const unsigned int FONTSET_START_ADDRESS = 0x50;
    const unsigned int FONTSET_SIZE = 80;

    const uint8_t fontset[] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
	{
		chip8->memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}
    FILE* rom = fopen(romname, "rb");
    if (!rom)
    {
        SDL_Log("Failed to load rom %s", romname);
        return -1;
    }
    fseek(rom,0, SEEK_END);
    const size_t rom_size = ftell(rom);
    const size_t max_size = sizeof chip8->memory - entry_point;
    rewind(rom);
    if(rom_size > max_size){
        SDL_Log("Failed to load rom %s\n", romname);
        return -1;
    }
    fread(&chip8->memory[entry_point], rom_size, 1, rom);
    fclose(rom);
    chip8->program_counter = entry_point;
    chip8->stack_pointer = &chip8->stack[0];
    return 1;


}
void update_screen(const chip8_t chip8, SDL_Renderer* renderer){
    SDL_Rect rect = {.x = 0, .y = 0, .w = 2, .h = 2};
    for (size_t i = 0; i < sizeof chip8.display; i++)
    {
        rect.x = (i % 64) * 2;
        rect.y = (i / 64) * 2;
        if(chip8.display[i]){
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        } else{
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    
    SDL_RenderPresent(renderer);
}
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
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    chip8_t chip8 = {
        0
    };
    char romname[100] = "IBM Logo.ch8";
    if(!loadRom(&chip8, romname)) exit(EXIT_FAILURE);
    while(true){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            {
                exit(EXIT_SUCCESS);
            }
            break;
        }
        emulate_instruction(&chip8);
        update_screen(chip8, renderer);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}