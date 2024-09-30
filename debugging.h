#include <stdlib.h>
#include <stdio.h>
#include "chip8.h"

void printProgram(Chip8* cp, int bytes){
    printf("--------PROGRAM CODE LOADED-----------\n");
    for (int i = 0x0200; i < 0x200+bytes; i++){
        printf(" %x ", cp->memory[i]);
        if (i % 8 == 0){
            printf(" :%x \n", i);
        }
    }
    printf("\n=========================================\n");
    return;
}
