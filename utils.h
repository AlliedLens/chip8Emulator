// #include "chip8.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chip8.h"


int openRom(Chip8* cp, char filename[100]){
    unsigned char buff;
    FILE* ptr;

    ptr = fopen(filename, "rb");

    if (ptr == NULL){
        printf("invalid rom\n");
    }else{
        fread(cp->memory + 0x200, sizeof(unsigned char), sizeof(cp->memory) - 0x200, ptr);
        fclose(ptr);
    }
}
