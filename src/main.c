#include <stdio.h>
#include <stdbool.h>
#include "SDL2/SDL.h"
#include "main.h"
#include "input.h"

#ifdef DEBUG
void print_debug_info(chip8_t chip8){

}
#endif
void input_handle(chip8_t* chip8){
    static bool any_pressed = false;
    static uint8_t key = 0xFF;
    for (size_t i = 0; key == 0xFF && i < 16; i++)
    {
        if(chip8->keypad[i]){
            key = i;
            any_pressed = true;
            
            break;
        }
    }
    if(!any_pressed)
        chip8->program_counter -= 2;
    else{
        if(chip8->keypad[key])
            chip8->program_counter -= 2;
        else{
            chip8->registers[chip8->instruction.X] = key;
            key = 0xFF;
            any_pressed = false;
            return;
        }
    }
}
void emulate_instruction(chip8_t* chip8){
    chip8->instruction.opcode = (chip8->memory[chip8->program_counter] << 8) | chip8->memory[chip8->program_counter+1];
    chip8->program_counter += 2;
    chip8->instruction.NNN = chip8->instruction.opcode & 0x0FFF;
    chip8->instruction.NN = chip8->instruction.opcode & 0x0FF;
    chip8->instruction.N = chip8->instruction.opcode & 0x0F;
    chip8->instruction.X = (chip8->instruction.opcode >> 8) & 0x0F;
    chip8->instruction.Y = (chip8->instruction.opcode >> 4) & 0x0F;
    //printf("Opcode: %X Program counter: %X Delay Timer: %d \n", chip8->instruction.opcode, chip8->program_counter-2, chip8->timer);
    switch ((chip8->instruction.opcode >> 12) & 0x0F)
    {
    case 0x00:
        if (chip8->instruction.NN == 0xE0)
        {
            // CLEAR DISP
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
    case 0x3:
        // If Vx == const Skip next instruction
        if(chip8->registers[chip8->instruction.X] == chip8->instruction.NN)
            chip8->program_counter += 2;
        break;
    case 0x4:
        // If Vx != const Skip next instruction 
        if(chip8->registers[chip8->instruction.X] != chip8->instruction.NN)
            chip8->program_counter += 2;
        break;
    case 0x5:
        // If Vx == Vy skip next instruction
        if(chip8->registers[chip8->instruction.X] == chip8->registers[chip8->instruction.Y])
            chip8->program_counter += 2;
        break;
    case 0x6:
        // Set Vx to const
        chip8->registers[chip8->instruction.X] = chip8->instruction.NN;
        break;
    case 0x7:
        // Add Vx by const
        chip8->registers[chip8->instruction.X] += chip8->instruction.NN;
        break;
    case 0x8:
        if(chip8->instruction.N == 0x0){
            //Sets Vx to Vy
            chip8->registers[chip8->instruction.X] = chip8->registers[chip8->instruction.Y];
        } else
        if(chip8->instruction.N == 0x1){
            //Vx is bitwise OR Vx | Vy
            chip8->registers[chip8->instruction.X] = chip8->registers[chip8->instruction.X] | chip8->registers[chip8->instruction.Y];
        } else
        if(chip8->instruction.N == 0x2){
            //Sets Vx to Vx & Vy
            chip8->registers[chip8->instruction.X] &= chip8->registers[chip8->instruction.Y];
        } else
        if(chip8->instruction.N == 0x3){
            //Vx is bitwise XOR Vx ^ Vy
            chip8->registers[chip8->instruction.X] = chip8->registers[chip8->instruction.X] ^ chip8->registers[chip8->instruction.Y];
        } else
        if(chip8->instruction.N == 0x4){
            //Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not.
            if(chip8->registers[chip8->instruction.X] + chip8->registers[chip8->instruction.Y] < chip8->registers[chip8->instruction.X])
                chip8->registers[0xF] = 1;
            else
                chip8->registers[0xF] = 0;
            chip8->registers[chip8->instruction.X] += chip8->registers[chip8->instruction.Y];
        } else
        if(chip8->instruction.N == 0x5){
            //Subtracts Vx Vx -= Vy VF is set to 1 when there is no underflow, and 0 if there is underflow
            if(chip8->registers[chip8->instruction.X] < chip8->registers[chip8->instruction.Y])
                chip8->registers[0xF] = 0;
            else
                chip8->registers[0xF] = 1;
            chip8->registers[chip8->instruction.X] -= chip8->registers[chip8->instruction.Y];
        } else
        if(chip8->instruction.N == 0x6){
            //Bit shift to the right, vf is = least bit of vx before shift
            
            uint8_t l_bit = chip8->registers[chip8->instruction.X] & 0x0001;
            chip8->registers[chip8->instruction.X] >>= 1;
            if(l_bit)
                chip8->registers[0xF] = 1;
            else
                chip8->registers[0xF] = 0;
        } else
        if(chip8->instruction.N == 0x7){
            //Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VY >= VX)
            if(chip8->registers[chip8->instruction.Y] < chip8->registers[chip8->instruction.X])
                chip8->registers[0xF] = 0;
            else
                chip8->registers[0xF] = 1;
            chip8->registers[chip8->instruction.X] = chip8->registers[chip8->instruction.Y] - chip8->registers[chip8->instruction.X];
        } else
        if(chip8->instruction.N == 0xE){
            // Bit shift to the left, VF is equal to 1 if the most significant bit of Vx before shift is > 0
            uint8_t m_bit = chip8->registers[chip8->instruction.X] & 0x80;
            chip8->registers[chip8->instruction.X] <<= 1;
            if(m_bit)
                chip8->registers[0xF] = 1;
            else
                chip8->registers[0xF] = 0;
        }
        else
            printf("Unimplemented instruction: 0x%04x, PC: 0x%04x\n", chip8->instruction.opcode, chip8->program_counter);       
        break;
    case 0x9:
        // Cond if vx!=vy
        if(chip8->registers[chip8->instruction.X] != chip8->registers[chip8->instruction.Y]){
            chip8->program_counter += 2;
        }
        break;
    case 0xA:
        //set I to address NNN
        chip8->index_register = chip8->instruction.NNN;
        break;
    case 0xB:
        //Jumps to Address V0 + NNN
        chip8->program_counter = chip8->registers[0x0] + chip8->instruction.NNN;
        break;
    case 0xC:
        //Randomly generate a number between 0 and 255 and & with NN const
        chip8->registers[chip8->instruction.X] = (rand() % 255) & chip8->instruction.NN;
        break;
    case 0xD:
        // Display sprite 8bits wide N height starting on (X,Y) and XOR with Display pixels
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
    case 0xE:
        if(chip8->instruction.NN == 0x9E){
            //Skips instruction if key in Vx is pressed
            if(chip8->keypad[chip8->registers[chip8->instruction.X]])
                chip8->program_counter += 2;
        } else
        if(chip8->instruction.NN == 0xA1){
            //Skips instruction if key in Vx is not pressed
            if(!chip8->keypad[chip8->registers[chip8->instruction.X]])
                chip8->program_counter += 2;
        }
        break;
    case 0xF:
        if(chip8->instruction.NN == 0x1E){
            // Adds Vx to I
            chip8->index_register += chip8->registers[chip8->instruction.X];
        } else
        if(chip8->instruction.NN == 0x07){
            // Set Vx to delay timer
            chip8->registers[chip8->instruction.X] = chip8->timer;
        } else
        if(chip8->instruction.NN == 0x15){
            //Set delay timer to Vx
            chip8->timer = chip8->registers[chip8->instruction.X];
        } else
        if(chip8->instruction.NN == 0x0A){
            // Await key press and store in Vx(blocking op), timers continue as usual
            input_handle(chip8);
        } else
        if(chip8->instruction.NN == 0x29){
            // Sets I to the digit font address, the digit thats to be represented onscreen corresponds to the value in Vx
            chip8->index_register = 0x50 + (5 * chip8->registers[chip8->instruction.X]);
        } else
        if (chip8->instruction.NN == 0x33){
            //BCD representation of value in Vx stored in I, I+1, I+2
            uint8_t Vx = chip8->registers[chip8->instruction.X];
            //printf("%d BCD: %d %d %d \n", Vx, Vx/100, Vx / 10 % 10, Vx % 10);
            chip8->memory[chip8->index_register] = Vx / 100;
            chip8->memory[chip8->index_register+1] = Vx / 10 % 10;
            chip8->memory[chip8->index_register+2] = Vx % 10;
        } else
        if(chip8->instruction.NN == 0x55){
            //Store values from reg. V0 to VX into memory starting at I and increment by 1
            for (size_t i = 0; i <= chip8->instruction.X; i++)
            {
                chip8->memory[chip8->index_register+i] = chip8->registers[i];
            }
            
        } else
        if(chip8->instruction.NN == 0x65){
            //Fills registers V0 to VX with values from memory starting at I and going +1
            for (size_t i = 0; i <= chip8->instruction.X; i++)
            {
                chip8->registers[i] = chip8->memory[chip8->index_register+i];
            }
        }
        else {
            printf("Unimplemented instruction: 0x%04x, PC: 0x%04x\n", chip8->instruction.opcode, chip8->program_counter);
        }
        break;
    default:
        printf("Unimplemented instruction: 0x%04x, PC: 0x%04x\n", chip8->instruction.opcode, chip8->program_counter);
        break;
    }
    if(chip8->timer > 0)
        chip8->timer--;
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
    chip8->timer = 0;
    return 1;
}
void update_screen(const chip8_t* chip8, SDL_Renderer* renderer){
    SDL_Rect rect = {.x = 0, .y = 0, .w = 10, .h = 10};
    for (size_t i = 0; i < sizeof chip8->display; i++)
    {
        rect.x = (i % 64) * 10;
        rect.y = (i / 64) * 10;
        if(chip8->display[i]){
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &rect);
        } else{
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
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
    char * romname = argv[1];
    if(!loadRom(&chip8, romname)) exit(EXIT_FAILURE);
    while(true){
        sdl_input_handle(&chip8);
        emulate_instruction(&chip8);
        update_screen(&chip8, renderer);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}