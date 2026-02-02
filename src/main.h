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