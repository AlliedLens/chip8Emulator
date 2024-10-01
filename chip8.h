#define MEM_SIZE 4096
#define REG_SIZE 16
#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 32
#define CALL_DEPTH 16
#define KEYPAD_CHARS 16
#include <stdio.h>

#ifndef CHIP8_H
#define CHIP8_H

/*
The systems memory map: 

0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
0x200-0xFFF - Program ROM and work RAM

*/

typedef struct Chip8{
    unsigned short opcode; //stores the current opcode, which is 2 bytes long
    unsigned char memory[MEM_SIZE]; //chip8 has 4 kilobytes of memory
    
    unsigned char V[REG_SIZE]; //chip8 has 16 8bit registers, with the last register being the flag

    unsigned short I; //index register, as there are 4096 bytes, we need 12 bits to address all of them, so we have a seperate register here to store a single memory address that cannot be modified using any type of artihmetic instruction

    unsigned short PC; //program counter has the address of the next variable

    unsigned char gfx[SCREEN_HEIGHT* SCREEN_WIDTH]; // screen has 2048 pixels with height 64 and width 32

    unsigned char delay_timer; //timer registers count at 60hz and when set above zero they will count down to zero 
    unsigned char sound_timer; //sound timer hits a buzzer when it reaches zero

    unsigned short call_stack[CALL_DEPTH]; //anytime a jump or subroutine is called its important to remember the memory address of the previous instruction, so a call stack is defined 
    unsigned short stack_pointer;

    unsigned char keypad[KEYPAD_CHARS];

}Chip8;

void initialize(Chip8* cp, char* font){
    //initializes register and memory once

    cp->PC = 0x200;
    cp->opcode = 0x00;
    cp->I = 0x00;
    cp->stack_pointer = 0x00;

    //clear display

    //clear registers
    for (int i = 0; i < 16; i++){
        cp->V[i] = 0x00;
    }

    //load fonts
    for (int i = 0; i < 80;i++ ){
        cp->memory[i] = font[i];
    }

    
    printf("Chip8 initialized...\n");
} 



// void emulateCycle(Chip8* cp){
//     cp->opcode = cp->memory[cp->PC] << 8 | cp->memory[cp->PC+1];

//     switch(cp->opcode & 0xF000){
//         case 0xA000:

//     }
// }

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