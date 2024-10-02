#include <stdlib.h>
#include <stdio.h>
#include "chip8.h"

void printProgram(Chip8* cp, int bytes){
    printf("===========PROGRAM CODE LOADED===========\n");
    for (int i = 0x200; i < 0x200+bytes; i = i+2){
        printf(" 0x%x ", cp->memory[i] << 8 | cp->memory[i+1]);
        if (i % 8 == 0){
            printf(" :0x%x \n", i);
        }
    }
    printf("\n=========================================\n");
    return;
}

void printInstruction(Chip8* cp){
    cp->opcode = cp->memory[cp->PC] << 8 | cp->memory[cp->PC+1];
    printf("address: 0x%x opcode: 0x%x: ", cp->PC, cp->opcode);
    int arithmetic_operation = cp->opcode & 0x000F;
    int ef_operation = cp->opcode & 0x00FF;
    switch(cp->opcode & 0xF000){ 
        case 0x0000:
            switch(cp->opcode)
            {
                case 0x00E0:        
                    printf("clear screen\n");
                    break;        
                case 0x00EE:           
                    printf("return subroutine\n");
                    break;     
            }
            break;
        case 0x1000:
            printf("jump 0x%x \n", cp->opcode & 0x0FFF);
            break;
        case 0x2000:
            printf("subroutine 0x%x \n", cp->opcode & 0x0FFF);
            break;
        case 0x3000:
            printf("if V%x != %x then\n", (cp->opcode&0x0F00)>>8, cp->opcode&0x00FF);
            break;
        case 0x4000:
            printf("if V%x == %x then\n", (cp->opcode&0x0F00)>>8, cp->opcode&0x00FF);
            break;
        case 0x5000:
            printf("if V%x != V%x then\n", (cp->opcode&0x0F00)>>8, (cp->opcode&0x0F00)>>4);
            break;
        case 0x6000:
            printf("V%x := %x\n", (cp->opcode&0x0F00)>>8, cp->opcode&0x00FF);
            break;
        case 0x7000:
            printf("V%x += %x\n", (cp->opcode&0x0F00)>>8, cp->opcode&0x00FF);
            break;
        case 0x8000:
            switch(arithmetic_operation){
                case 0x0000:
                    printf("V%x := V%x", (cp->opcode & 0x0F00)>>8, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x0001:
                    printf("V%x |= V%x", (cp->opcode & 0x0F00)>>8, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x0002:
                    printf("V%x &= V%x", (cp->opcode & 0x0F00)>>8, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x0003:
                    printf("V%x ^= V%x", (cp->opcode & 0x0F00)>>8, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x0004:
                    printf("V%x += V%x", (cp->opcode & 0x0F00)>>8, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x0005:
                    printf("V%x -= V%x", (cp->opcode & 0x0F00)>>8, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x0006:
                    printf("V%x >>= V%x", (cp->opcode & 0x0F00)>>8, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x0007:
                    printf("V%x =- V%x", (cp->opcode & 0x0F00)>>8, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x000E:
                    printf("V%x <<= V%x", (cp->opcode & 0x0F00)>>8, (cp->opcode & 0x00F0) >> 4);
                    break;
            }
            break;
        case 0x9000:
            printf("if V%x == V%x", (cp->opcode & 0x0F00)>>8, (cp->opcode & 0x00F0) >> 4);
            break;
        case 0xA000:
            printf("I := 0x%x\n", cp->opcode & 0x0FFF);
            break;
        case 0xB000:
            printf("jump0 0x%x\n", cp->opcode & 0x0FFF);
            break;
        case 0xC000:
            printf("V%x := random 0x%x\n", (cp->opcode & 0x0F00)>>8, cp->opcode & 0x00FF);
            break;
        case 0xD000:
            printf("sprite V%x V%x %x\n", (cp->opcode & 0x0F00)>>8, (cp->opcode & 0x00F0)>>4, cp->opcode & 0x000F);
            break;
        case 0xE000:
            switch(ef_operation){
                case 0x009E:
                    printf("if V%x -key then\n", (cp->opcode & 0x0F00)>>8);
                    break;
                case 0x00A1:
                    printf("if V%x key then\n", (cp->opcode & 0x0F00)>>8);
                    break;
            break;
            }
        case 0xF000:
            switch (ef_operation)
            {
                case 0x0007:
                    printf("V%x := delay\n", (cp->opcode & 0x0F00)>>8);
                    break;
                case 0x000A:
                    printf("V%x := key \n", (cp->opcode & 0x0F00)>>8);
                    break;
                case 0x0015:
                    printf("delay := V%x \n", (cp->opcode & 0x0F00)>>8);
                    break;
                case 0x0018:
                    printf("buzzer := V%x \n", (cp->opcode & 0x0F00)>>8);
                    break;
                case 0x001E:
                    printf("I += V%x \n", (cp->opcode & 0x0F00)>>8);
                    break;
                case 0x0029:
                    printf("i := hex V%x \n", (cp->opcode & 0x0F00)>>8);
                    break;
                case 0x0033:
                    printf("BCD V%x \n", (cp->opcode & 0x0F00)>>8);
                    break;
                case 0x0055:
                    printf("Save V%x \n", (cp->opcode & 0x0F00)>>8);
                    break;
                case 0x0065:
                    printf("Load V%x \n", (cp->opcode & 0x0F00)>>8);
                    break;

            }
            break;

        default:
            printf ("Unknown opcode\n");

    }
}

void printMultipleInstructions(Chip8* cp, int count){
    printf("\n===========Instructions==========\n");
    for (int i = cp->PC; i < 0x0200+count*2; i=i+2){
        cp->PC = i;
        printInstruction(cp);
    }
    printf("\n=====================================\n");
}
