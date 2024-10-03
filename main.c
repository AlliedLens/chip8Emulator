#include "chip8.h"
#include "debugging.h"
#include "utils.h"
#include "graphics.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define REFRESH_RATE 100


unsigned char font[80] = {
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

    SDL_Event e;

    while (!quit){

        while (SDL_PollEvent(&e) != 0){
            if (e.type == SDL_QUIT){
                quit = 1;
            }
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

    // printMultipleInstructions(cp, 50);
    // for (;;){        
    // }
}