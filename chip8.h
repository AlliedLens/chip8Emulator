#define MEM_SIZE 4096
#define REG_SIZE 16
#define SCREEN_WIDTH 64 * 8
#define SCREEN_HEIGHT 32 * 8
#define CALL_DEPTH 16
#define KEYPAD_CHARS 16



#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include <stdlib.h>
#include "graphics.h"
#include <SDL2/SDL.h>


/*

The systems memory map: 

0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
0x200-0xFFF - Program ROM and work RAM

Keypad                   Keyboard
+-+-+-+-+                +-+-+-+-+
|1|2|3|C|                |1|2|3|4|
+-+-+-+-+                +-+-+-+-+
|4|5|6|D|                |Q|W|E|R|
+-+-+-+-+       =>       +-+-+-+-+
|7|8|9|E|                |A|S|D|F|
+-+-+-+-+                +-+-+-+-+
|A|0|B|F|                |Z|X|C|V|
+-+-+-+-+                +-+-+-+-+



*/

static int keymap[0x10] = 
{
    SDL_SCANCODE_X,
    SDL_SCANCODE_1,
    SDL_SCANCODE_2,
    SDL_SCANCODE_3,
    SDL_SCANCODE_Q,
    SDL_SCANCODE_W,
    SDL_SCANCODE_E,
    SDL_SCANCODE_A,
    SDL_SCANCODE_S,
    SDL_SCANCODE_D,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_C,
    SDL_SCANCODE_4,
    SDL_SCANCODE_R,
    SDL_SCANCODE_F,
    SDL_SCANCODE_V
};


typedef struct Chip8{
    unsigned short opcode; //stores the current opcode, which is 2 bytes long
    unsigned char memory[MEM_SIZE]; //chip8 has 4 kilobytes of memory
    
    unsigned char V[REG_SIZE]; //chip8 has 16 8bit registers, with the last register being the flag

    unsigned short I; //index register, as there are 4096 bytes, we need 12 bits to address all of them, so we have a seperate register here to store a single memory address that cannot be modified using any type of artihmetic instruction

    unsigned short PC; //program counter has the address of the next variable

    unsigned char gfx[SCREEN_HEIGHT*8][SCREEN_WIDTH*8]; // screen has 2048 pixels with height 64 and width 32

    unsigned char delay_timer; //timer registers count at 60hz and when set above zero they will count down to zero 
    unsigned char sound_timer; //sound timer hits a buzzer when it reaches zero

    unsigned short call_stack[CALL_DEPTH]; //anytime a jump or subroutine is called its important to remember the memory address of the previous instruction, so a call stack is defined 
    unsigned short stack_pointer;
    unsigned int draw_flag;

    unsigned char keypad[KEYPAD_CHARS];

}Chip8;

void initialize(Chip8* cp, char* font, SDLapp* app){
    //initializes register and memory once

    cp->PC = 0x200;
    cp->opcode = 0x00;
    cp->I = 0x00;
    cp->stack_pointer = 0x00;

    //clear display
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    //clear registers
    for (int i = 0; i < REG_SIZE; i++){
        cp->V[i] = 0x00;
    }

    //empty keypresses
    for (int i = 0; i < KEYPAD_CHARS; i++){
        cp->keypad[i] = 0;
    }

    // for (int i = 0; i < 80; i++){

    // }

    cp->draw_flag = 0;
    
    memset(cp->memory, 0, sizeof(cp->memory));
    memset(cp->call_stack, 0, sizeof(cp->call_stack));
    memset(cp->V, 0, sizeof(cp->V));
    
    printf("Chip8 initialized...\n");
} 

void emulateCycle(Chip8* cp, SDLapp* app) {
    // Fetch opcode
    cp->opcode = cp->memory[cp->PC] << 8 | cp->memory[cp->PC + 1];

    int arithmetic_operation = cp->opcode & 0x000F;
    int ef_operation = cp->opcode & 0x00FF;
    
    short Xreg = (cp->opcode & 0x0F00) >> 8;
    short Yreg = (cp->opcode & 0x00F0) >> 4;
    
    short N = cp->opcode & 0x000F;
    short NN = cp->opcode & 0x00FF;
    short NNN = cp->opcode & 0x0FFF;

    Uint8* keyboard;

    char pixel;

    switch (cp->opcode & 0xF000) {
        case 0x0000:
            switch (cp->opcode) {
                case 0x00E0: // Clear screen
                    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
                    SDL_RenderClear(app->renderer);
                    cp->PC += 2;
                    break;
                case 0x00EE: // Return from subroutine
                    cp->stack_pointer--;
                    cp->PC = cp->call_stack[cp->stack_pointer];
                    cp->PC += 2;
                    break;
            }
            break;
        case 0x1000: // Jump to address NNN
            cp->PC = NNN;
            break;
        case 0x2000: // Call subroutine at NNN
            cp->call_stack[cp->stack_pointer] = cp->PC;
            cp->stack_pointer++;
            cp->PC = NNN;
            break;
        case 0x3000: // Skip next instruction if Vx == NN
            if (cp->V[Xreg] == NN) {
                cp->PC += 4;
            } else {
                cp->PC += 2;
            }
            break;
        case 0x4000: // Skip next instruction if Vx != NN
            if (cp->V[Xreg] != NN) {
                cp->PC += 4;
            } else {
                cp->PC += 2;
            }
            break;
        case 0x5000: // Skip next instruction if Vx == Vy
            if (cp->V[Xreg] == cp->V[Yreg]) {
                cp->PC += 4;
            } else {
                cp->PC += 2;
            }
            break;
        case 0x6000: // Set Vx = NN
            cp->V[Xreg] = NN;
            cp->PC += 2;
            break;
        case 0x7000: // Add NN to Vx
            cp->V[Xreg] += NN;
            cp->PC += 2;
            break;
        case 0x8000:
            switch (arithmetic_operation) {
                case 0x0000: // Set Vx = Vy
                    cp->V[Xreg] = cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0001: // Set Vx = Vx OR Vy
                    cp->V[Xreg] |= cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0002: // Set Vx = Vx AND Vy
                    cp->V[Xreg] &= cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0003: // Set Vx = Vx XOR Vy
                    cp->V[Xreg] ^= cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0004: // Add Vy to Vx, set VF on carry
                    cp->V[Xreg] += cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0005: // Subtract Vy from Vx, set VF on borrow
                    cp->V[Xreg] -= cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0006: // Shift Vx right by 1
                    cp->V[Xreg] >>= 1;
                    cp->PC += 2;
                    break;
                case 0x0007: // Set Vx = Vy - Vx, set VF on borrow
                    cp->V[Xreg] = cp->V[Yreg] - cp->V[Xreg];
                    cp->PC += 2;
                    break;
                case 0x000E: // Shift Vx left by 1
                    cp->V[Xreg] <<= 1;
                    cp->PC += 2;
                    break;
            }
            break;
        case 0x9000: // Skip next instruction if Vx != Vy
            if (cp->V[Xreg] != cp->V[Yreg]) {
                cp->PC += 4;
            } else {
                cp->PC += 2;
            }
            break;
        case 0xA000: // Set I = NNN
            cp->I = NNN;
            cp->PC += 2;
            break;
        case 0xB000: // Jump to address NNN + V0
            cp->PC = NNN + cp->V[0];
            break;
        case 0xC000: // Set Vx = random byte AND NN
            cp->V[Xreg] = (rand() % 256) & NN;
            cp->PC += 2;
            break;
        case 0xD000: // Draw sprite at (Vx, Vy)
            cp->V[0xF] = 0;
            for (int yline = 0; yline < N; yline++) {
                pixel = cp->memory[cp->I + yline];
                for (int xline = 0; xline < 8; xline++) {
                    if ((pixel & (0x80 >> xline)) != 0) {
                        if (cp->gfx[cp->V[Yreg] + yline][cp->V[Xreg] + xline] == 1)
                            cp->V[0xF] = 1;  // Collision
                        cp->gfx[cp->V[Yreg] + yline][cp->V[Xreg] + xline] ^= 1;
                        if (cp->gfx[cp->V[Yreg] + yline][cp->V[Xreg] + xline]) {
                            SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
                            drawPixel(app, cp->V[Xreg] + xline, cp->V[Yreg] + yline);
                        } else {
                            SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
                            drawPixel(app, cp->V[Xreg] + xline, cp->V[Yreg] + yline);
                        }
                    }
                }
            }
            cp->draw_flag = 1;
            cp->PC += 2;
            break;
        case 0xE000:
            switch (ef_operation) {
                case 0x009E: // EX9E: Skip if key
                    keyboard = (Uint8 *)SDL_GetKeyboardState(NULL);
				
                    if (keyboard[keymap[cp->V[Xreg]]])
                    {
                        cp->PC += 4;
                    }else{
                        cp->PC += 2;
                    }
			
                    break;
                case 0x00A1: // EXA1: Skip if key
                    keyboard = (Uint8 *)SDL_GetKeyboardState(NULL);		

                    if (!keyboard[keymap[cp->V[Xreg]]])
                    {
                        cp->PC += 4;
                    }else{
                        cp->PC += 2;
                    }

                    break;            }
            break;
        case 0xF000:
            switch (ef_operation) {
                case 0x0007: // Set Vx = delay_timer
                    cp->V[Xreg] = cp->delay_timer;
                    cp->PC += 2;
                    break;
                case 0x000A: // Wait for key press, store key value in Vx
                    keyboard = (Uint8 *)SDL_GetKeyboardState(NULL);

                    for (int i = 0; i < 0x10; i++)
                    {
                        if (keyboard[keymap[i]])
                        {
                            cp->V[Xreg] = i;
                            cp->PC += 2;
                        }
                    }
                    break;
                case 0x0015: // Set delay_timer = Vx
                    cp->delay_timer = cp->V[Xreg];
                    cp->PC += 2;
                    break;
                case 0x0018: // Set sound_timer = Vx
                    cp->sound_timer = cp->V[Xreg];
                    cp->PC += 2;
                    break;
                case 0x001E: // Set I = I + Vx
                    cp->I += cp->V[Xreg];
                    cp->PC += 2;
                    break;
                case 0x0029: // Set I = location of sprite for digit Vx
                    cp->I = cp->V[Xreg] * 5;
                    cp->PC += 2;
                    break;
                case 0x0033: // Store BCD of Vx in memory locations I, I+1, I+2
                    cp->memory[cp->I] = cp->V[Xreg] / 100;
                    cp->memory[cp->I + 1] = (cp->V[Xreg] / 10) % 10;
                    cp->memory[cp->I + 2] = cp->V[Xreg] % 10;
                    cp->PC += 2;
                    break;
                case 0x0055: // Store registers V0 through Vx in memory starting at I
                    for (int i = 0; i <= Xreg; i++) {
                        cp->memory[cp->I + i] = cp->V[i];
                    }
                    cp->PC += 2;
                    break;
                case 0x0065: // Read registers V0 through Vx from memory starting at I
                    for (int i = 0; i <= Xreg; i++) {
                        cp->V[i] = cp->memory[cp->I + i];
                    }
                    cp->PC += 2;
                    break;
            }
            break;

        default:
            // Handle unknown opcode if needed
            break;
    }
}

void updateTimers(Chip8* cp){
    cp->delay_timer -= 1;
    cp->sound_timer -= 1;
}

void PrintKeyInfo(SDL_KeyboardEvent *key) {
    if (key->type == SDL_KEYUP) {
        printf("Release: ");
    } else {
        printf("Press: ");
    }

    printf("Scancode: 0x%02X", key->keysym.scancode);
    const char *keyName = SDL_GetKeyName(key->keysym.sym);
    if (keyName) {
        printf(", Name: %s", keyName);
    } else {
        printf(", Name: Unknown");
    }

    printf("\n");
}

void printKeypad(Chip8* cp){
    for (int i = 0; i < 4; i++) {
        printf("+-+-+-+-+\n|");
        for (int j = 0; j < 4; j++) {
            printf("%c|", cp->keypad[i* 4 + j]);
        }
        printf("\n+-+-+-+-+\n");
    }
}


void setKeys(Chip8* cp, SDL_Event* e, int* quit){
    for (int i = 0; i < KEYPAD_CHARS; i++){
        cp->keypad[i] = 0;
    }

    char keypress;

    switch(e->type){
        case SDL_QUIT:
            *quit = 1;
        case SDL_KEYDOWN:
            // PrintKeyInfo(&e->key);
            switch(e->key.keysym.sym){
                case('1'):
                    cp->keypad[0] = 1;
                    break;
                case('2'):
                    cp->keypad[1] = 1;
                    break;
                case('3'):
                    cp->keypad[2] = 1;
                    break;
                case('4'):
                    cp->keypad[3] = 1;
                    break;
                case('q'):
                    cp->keypad[4] = 1;
                    break;
                case('w'):
                    cp->keypad[5] = 1;
                    break;
                case('e'):
                    cp->keypad[6] = 1;
                    break;
                case('r'):
                    cp->keypad[7] = 1;
                    break;
                case('a'):
                    cp->keypad[8] = 1;
                    break;
                case('s'):
                    cp->keypad[9] = 1;
                    break;
                case('d'):
                    cp->keypad[10] = 1;
                    break;
                case('f'):
                    cp->keypad[11] = 1;
                    break;                            
                case('z'):
                    cp->keypad[12] = 1;
                    break;
                case('x'):
                    cp->keypad[13] = 1;
                    break;
                case('c'):
                    cp->keypad[14] = 1;
                    break;
                case('v'):
                    cp->keypad[15] = 1;
                    break;
            }
            // printf("%c\n", e->key.keysym.sym);
            break;
    };
}




/*
    fetch opcode
        fetching opcode involves taking one opcode from the memory at the location specified by the program counter. since one opcode is 2 bytes, we need to fetch 2 successive elements to get an actual opcode. 

        i.e opcode = memory[pc] << 8 | memory[pc+1] as chip8 is bigendian

    decode opcode
        now that we have gotten the opcode we can now look up what the opcode does in our emulator,
        maybe u can do this through switch-case statements. 
        based on the instructions mentioend here https://github.com/mattmikolay/chip-8/wiki/Mastering-CHIP%E2%80%908

    exec opcode
        execution is done as per given instructions
        pc += 2

    update timers
        since the chip8 has a frequency of 60 hz we need to create something that makes
        the rate of the emulation cycle at 60 hz as well

        also, decrement the delay_timer and sound_timer until it reaches 0 if it has been set to zero

*/


#endif