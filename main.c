#include "chip8.h"
#include "debugging.h"
#include "utils.h"
#include "graphics.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define REFRESH_RATE 700


unsigned char font[80] =
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


void main(int argc, char* argv[] ){
    unsigned char filename[100];
    Chip8* cp;
    SDLapp* app;    

    strcpy(filename, argv[1]);
    printf("loading %s rom\n", filename);
    
    cp = (Chip8*)malloc(sizeof(Chip8));
    app = (SDLapp*)malloc(sizeof(SDLapp));
    
    initialize(cp, font, app);
    openRom(cp, filename);

    setUpGraphics(app);

    printProgram(cp, 36);

    int quit = 0;

    SDL_Event event;

    while (!quit){

        while (SDL_PollEvent(&event) != 0){
            setKeys(cp, &event, &quit);
        }

        printAndEmulateCycle(cp, app);
        updateTimers(cp);

        if (cp->draw_flag){
            cp->draw_flag = 0;
            SDL_RenderPresent(app->renderer);  
        }
        SDL_Delay(1000/REFRESH_RATE); 
        // cp->PC += 2;   
    }

    // printMultipleInstructions(cp, 150);
}


/*
IBM render

address: 0x200 opcode: 0xe0: clear screen
address: 0x202 opcode: 0xa22a: I := 0x22a
address: 0x204 opcode: 0x600c: V0 := c
address: 0x206 opcode: 0x6108: V1 := 8
address: 0x208 opcode: 0xd01f: sprite V0 V1 f
address: 0x20a opcode: 0x7009: V0 += 9
address: 0x20c opcode: 0xa239: I := 0x239
address: 0x20e opcode: 0xd01f: sprite V0 V1 f
address: 0x210 opcode: 0xa248: I := 0x248
address: 0x212 opcode: 0x7008: V0 += 8
address: 0x214 opcode: 0xd01f: sprite V0 V1 f
address: 0x216 opcode: 0x7004: V0 += 4
address: 0x218 opcode: 0xa257: I := 0x257
address: 0x21a opcode: 0xd01f: sprite V0 V1 f
address: 0x21c opcode: 0x7008: V0 += 8
address: 0x21e opcode: 0xa266: I := 0x266
address: 0x220 opcode: 0xd01f: sprite V0 V1 f
address: 0x222 opcode: 0x7008: V0 += 8
address: 0x224 opcode: 0xa275: I := 0x275
address: 0x226 opcode: 0xd01f: sprite V0 V1 f
address: 0x228 opcode: 0x1228: jump 0x228 

*/

/*
Release: Scancode: 0x27, Name: 0
Release: Scancode: 0x1E, Name: 1
Release: Scancode: 0x1F, Name: 2
Release: Scancode: 0x20, Name: 3
Release: Scancode: 0x21, Name: 4
Release: Scancode: 0x22, Name: 5
Release: Scancode: 0x23, Name: 6
Release: Scancode: 0x24, Name: 7
Release: Scancode: 0x25, Name: 8
Release: Scancode: 0x26, Name: 9
Release: Scancode: 0x04, Name: A
Release: Scancode: 0x05, Name: B
Release: Scancode: 0x06, Name: C
Release: Scancode: 0x07, Name: D
Release: Scancode: 0x08, Name: E
Release: Scancode: 0x09, Name: F

Maps To

Release: Scancode: 0x1E, Name: 1
Release: Scancode: 0x1F, Name: 2
Release: Scancode: 0x20, Name: 3
Release: Scancode: 0x21, Name: 4
Release: Scancode: 0x14, Name: Q
Release: Scancode: 0x1A, Name: W
Release: Scancode: 0x08, Name: E
Release: Scancode: 0x15, Name: R
Release: Scancode: 0x04, Name: A
Release: Scancode: 0x16, Name: S
Release: Scancode: 0x07, Name: D
Release: Scancode: 0x09, Name: F
Release: Scancode: 0x1D, Name: Z
Release: Scancode: 0x1B, Name: X
Release: Scancode: 0x06, Name: C
Release: Scancode: 0x19, Name: V


*/