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
    for (int i = 0; i < 16; i++){
        cp->V[i] = 0x00;
    }

    cp->draw_flag = 0;
    
    memset(cp->memory, 0, sizeof(cp->memory));
    memset(cp->call_stack, 0, sizeof(cp->call_stack));
    memset(cp->V, 0, sizeof(cp->V));
    memcpy(cp->memory, font, sizeof(font));
    
    printf("Chip8 initialized...\n");
} 

void emulateCycle(Chip8* cp, SDLapp* app){
    cp->opcode = cp->memory[cp->PC] << 8 | cp->memory[cp->PC+1];
    printf("address: 0x%x opcode: 0x%x \n", cp->PC, cp->opcode);
    int arithmetic_operation = cp->opcode & 0x000F;
    int ef_operation = cp->opcode & 0x00FF;
    
    char Xreg = (cp->opcode&0x0F00)>>8;
    char Yreg = (cp->opcode&0x0F00)>>4;
    
    char N = cp->opcode&0x000F;
    char NN = cp->opcode&0x00FF;
    char NNN = cp->opcode&0x0FFF;
    
    char pixel;

    char x;
    char y;    

    
    switch(cp->opcode & 0xF000){ 
        case 0x0000:
            switch(cp->opcode)
            {
                case 0x00E0: //clear the screen       
                    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
                    SDL_RenderClear(app->renderer);
                    break;        
                case 0x00EE: // exit a subroutine
                    cp->stack_pointer -= 1;
                    cp->PC = cp->call_stack[cp->stack_pointer];
                    break;     
            }
            cp->PC += 2;
            break;
        case 0x1000: // jump NNN
            cp->PC = NNN;
            break;
        
        case 0x2000: // call subroutine
            cp->call_stack[cp->stack_pointer] = cp->PC;
            cp->stack_pointer += 1;
            cp->PC = NNN;
            break;
        
        case 0x3000: //Conditional Skip
        
            if (cp->memory[Xreg] == NN){
                cp->PC += 4;
            }else{
                cp->PC += 2;
            }

            break;
        
        case 0x4000: //anotehr conditional skip
            if (cp->memory[Xreg] != NN){
                cp->PC += 4;
            }else{
                cp->PC += 2;
            }
            break;

        case 0x5000: //conditional skip wtih registers
            if (cp->V[Xreg] == cp->V[Yreg]){
                cp->PC += 4;  
            }else{
                cp->PC += 2;
            }
            break;

        case 0x6000: //assigning values to registers
            cp->V[Xreg] = NN;
            cp->PC += 2;
            break;
        
        case 0x7000: // arithmetic opeartions with registers
            cp->V[Xreg] += NN;
            cp->PC += 2;
            break;
        
        case 0x8000:
            switch(arithmetic_operation){
                case 0x0000:
                    cp->V[Xreg] = cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0001:
                    cp->V[Xreg] |= cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0002:
                    cp->V[Xreg] &= cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0003:
                    cp->V[Xreg] ^= cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0004:
                    cp->V[Xreg] += cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0005:
                    cp->V[Xreg] -= cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0006:
                    cp->V[Xreg] >>= cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0007:
                    cp->V[Xreg] =- cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x000E:
                    cp->V[Xreg] <<= cp->V[Yreg];
                    cp->PC += 2;
                    break;
            }
            break;
        case 0x9000:
            if (cp->V[Xreg] != cp->V[Yreg]) {
                cp->PC += 4;
            }else{
                cp->PC += 2;
            }
            break;
        case 0xA000:
            cp->I = NNN;
            cp->PC += 2;
            break;
        case 0xB000:
            cp->PC = NNN + cp->V[0];
            break;
        case 0xC000:
            cp->V[Xreg] = NN&rand();
            cp->PC += 2;
            break;
        case 0xD000:
            printf("---sprite V%x V%x %x\n", (cp->opcode & 0x0F00)>>8, (cp->opcode & 0x00F0)>>4, cp->opcode & 0x000F);
            for (int yline =0 ; yline < N; yline++){
                pixel = cp->memory[cp->I + yline];
                for (int xline = 0; xline < 8; xline++){
                    if ( (pixel & (0x80 >> xline)) != 0 ){
                        if (cp->gfx[Yreg +yline][Xreg + xline] == 1) cp->V[0xF] = 1;
                        cp->gfx[Yreg+yline][Xreg+xline] ^= 1; 
                        if (cp->gfx[Yreg+yline][Xreg+xline]){
                            SDL_SetRenderDrawColor(app->renderer, 255,255,255, 255);
                            drawPixel(app, Xreg+xline, Yreg+yline );
                        }else{
                            SDL_SetRenderDrawColor(app->renderer, 0,0,0,255);
                            drawPixel(app, Xreg+xline, Yreg+yline);
                        }

                    }
                }
                
            }
            cp->draw_flag = 1;
            cp->PC += 2;
            break;
        case 0xE000:
            switch(ef_operation){
                case 0x009E:
                    printf("---if V%x -key then\n", (cp->opcode & 0x0F00)>>8);
                    cp->PC += 2;
                    break;
                case 0x00A1:
                    printf("---if V%x key then\n", (cp->opcode & 0x0F00)>>8);
                    cp->PC += 2;
                    break;
                break;
            }
        case 0xF000:
            switch (ef_operation)
            {
                case 0x0007:
                    printf("V%x := delay\n", (cp->opcode & 0x0F00)>>8);
                    cp->V[Xreg] = cp->delay_timer;
                    break;
                case 0x000A:
                    printf("V%x := key \n", (cp->opcode & 0x0F00)>>8);
                    break;
                case 0x0015:
                    cp->delay_timer = cp->V[Xreg];
                    break;
                case 0x0018:
                    cp->sound_timer = cp->V[Xreg];
                    break;
                case 0x001E:
                    cp->I += cp->V[Xreg];
                    break;
                case 0x0029:

                    printf("---i := hex V%x \n", (cp->opcode & 0x0F00)>>8);
                    cp->I = cp->V[Xreg] * 5;
                    break;
                case 0x0033:
                    cp->memory[cp->I] = cp->V[Xreg] / 100;
                    cp->memory[cp->I+1] = (cp->V[Xreg] / 10)%10;
                    cp->memory[cp->I+2] = (cp->V[Xreg])%10;                    
                    break;
                case 0x0055:
                    for (int i = 0; i <= Xreg; i++){
                        cp->memory[cp->I + i] = cp->V[i];
                    }
                    break;
                case 0x0065:
                    for (int i = 0; i <= Xreg; i++){
                        cp->V[i] = cp->memory[cp->I + i];
                    }
                    break;

            }

        default:
            printf ("Unknown opcode\n");
            break;

    }
}

void updateTimers(Chip8* cp){
    cp->delay_timer -= 1;
    cp->sound_timer -= 1;
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