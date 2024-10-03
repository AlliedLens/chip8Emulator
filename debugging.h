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
    short arithmetic_operation = cp->opcode & 0x000F;
    short ef_operation = cp->opcode & 0x00FF;
    
    short NNN = cp->opcode & 0x0FFF;
    short NN = cp->opcode & 0x00FF;
    short N = cp->opcode & 0x000F;

    short X = (cp->opcode & 0x0F00) >> 8;
    short Y = (cp->opcode & 0x00F0) >> 4; 
    
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
            printf("jump 0x%x \n", NNN);
            break;
        case 0x2000:
            printf("subroutine 0x%x \n", NNN);
            break;
        case 0x3000:
            printf("if V%x != %x then\n", X, NN);
            break;
        case 0x4000:
            printf("if V%x == %x then\n", X, NN);
            break;
        case 0x5000:
            printf("if V%x != V%x then\n", X, (cp->opcode&0x0F00)>>4);
            break;
        case 0x6000:
            printf("V%x := %x\n", X, NN);
            break;
        case 0x7000:
            printf("V%x += %x\n", X, NN);
            break;
        case 0x8000:
            switch(arithmetic_operation){
                case 0x0000:
                    printf("V%x := V%x", X, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x0001:
                    printf("V%x |= V%x", X, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x0002:
                    printf("V%x &= V%x", X, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x0003:
                    printf("V%x ^= V%x", X, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x0004:
                    printf("V%x += V%x", X, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x0005:
                    printf("V%x -= V%x", X, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x0006:
                    printf("V%x >>= V%x", X, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x0007:
                    printf("V%x =- V%x", X, (cp->opcode & 0x00F0) >> 4);
                    break;
                case 0x000E:
                    printf("V%x <<= V%x", X, (cp->opcode & 0x00F0) >> 4);
                    break;
            }
            break;
        case 0x9000:
            printf("if V%x == V%x", X, (cp->opcode & 0x00F0) >> 4);
            break;
        case 0xA000:
            printf("I := 0x%x\n", NNN);
            break;
        case 0xB000:
            printf("jump0 0x%x\n", NNN);
            break;
        case 0xC000:
            printf("V%x := random 0x%x\n", X, cp->opcode & 0x00FF);
            break;
        case 0xD000:
            printf("sprite V%x V%x %x\n", X, (cp->opcode & 0x00F0)>>4, cp->opcode & 0x000F);
            break;
        case 0xE000:
            switch(ef_operation){
                case 0x009E:
                    printf("if V%x -key then\n", X);
                    break;
                case 0x00A1:
                    printf("if V%x key then\n", X);
                    break;
            break;
            }
        case 0xF000:
            switch (ef_operation)
            {
                case 0x0007:
                    printf("V%x := delay\n", X);
                    break;
                case 0x000A:
                    printf("V%x := key \n", X);
                    break;
                case 0x0015:
                    printf("delay := V%x \n", X);
                    break;
                case 0x0018:
                    printf("buzzer := V%x \n", X);
                    break;
                case 0x001E:
                    printf("I += V%x \n", X);
                    break;
                case 0x0029:
                    printf("i := hex V%x \n", X);
                    break;
                case 0x0033:
                    printf("BCD V%x \n", X);
                    break;
                case 0x0055:
                    printf("Save V%x \n", X);
                    break;
                case 0x0065:
                    printf("Load V%x \n", X);
                    break;

            }
            break;

        default:
            printf ("Unknown opcode\n");

    }
}

void printAndEmulateCycle(Chip8* cp, SDLapp* app) {
    // Fetch opcode
    cp->opcode = cp->memory[cp->PC] << 8 | cp->memory[cp->PC+1];

    // Print the current instruction (fetch and decode)
    printf("address: 0x%x opcode: 0x%x: ", cp->PC, cp->opcode);

    int arithmetic_operation = cp->opcode & 0x000F;
    int ef_operation = cp->opcode & 0x00FF;
    
    short Xreg = (cp->opcode & 0x0F00) >> 8;
    short Yreg = (cp->opcode & 0x00F0) >> 4;
    
    short N = cp->opcode & 0x000F;
    short NN = cp->opcode & 0x00FF;
    short NNN = cp->opcode & 0x0FFF;

    char pixel;

    switch (cp->opcode & 0xF000) {
        case 0x0000:
            switch(cp->opcode) {
                case 0x00E0: // Clear screen
                    printf("clear screen\n");
                    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
                    SDL_RenderClear(app->renderer);
                    cp->PC += 2;
                    break;
                case 0x00EE: // Return from subroutine
                    printf("return subroutine\n");
                    cp->stack_pointer--;
                    cp->PC = cp->call_stack[cp->stack_pointer];
                    break;
            }
            break;
        case 0x1000: // Jump to address NNN
            printf("jump 0x%x \n", NNN);
            cp->PC = NNN;
            break;
        case 0x2000: // Call subroutine at NNN
            printf("subroutine 0x%x \n", NNN);
            cp->call_stack[cp->stack_pointer] = cp->PC;
            cp->stack_pointer++;
            cp->PC = NNN;
            break;
        case 0x3000: // Skip next instruction if Vx == NN
            printf("if V%x == %x then\n", Xreg, NN);
            if (cp->V[Xreg] == NN) {
                cp->PC += 4;
            } else {
                cp->PC += 2;
            }
            break;
        case 0x4000: // Skip next instruction if Vx != NN
            printf("if V%x != %x then\n", Xreg, NN);
            if (cp->V[Xreg] != NN) {
                cp->PC += 4;
            } else {
                cp->PC += 2;
            }
            break;
        case 0x5000: // Skip next instruction if Vx == Vy
            printf("if V%x == V%x then\n", Xreg, Yreg);
            if (cp->V[Xreg] == cp->V[Yreg]) {
                cp->PC += 4;
            } else {
                cp->PC += 2;
            }
            break;
        case 0x6000: // Set Vx = NN
            printf("V%x := %x\n", Xreg, NN);
            cp->V[Xreg] = NN;
            cp->PC += 2;
            break;
        case 0x7000: // Add NN to Vx
            printf("V%x += %x\n", Xreg, NN);
            cp->V[Xreg] += NN;
            cp->PC += 2;
            break;
        case 0x8000:
            switch(arithmetic_operation) {
                case 0x0000: // Set Vx = Vy
                    printf("V%x := V%x\n", Xreg, Yreg);
                    cp->V[Xreg] = cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0001: // Set Vx = Vx OR Vy
                    printf("V%x |= V%x\n", Xreg, Yreg);
                    cp->V[Xreg] |= cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0002: // Set Vx = Vx AND Vy
                    printf("V%x &= V%x\n", Xreg, Yreg);
                    cp->V[Xreg] &= cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0003: // Set Vx = Vx XOR Vy
                    printf("V%x ^= V%x\n", Xreg, Yreg);
                    cp->V[Xreg] ^= cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0004: // Add Vy to Vx, set VF on carry
                    printf("V%x += V%x\n", Xreg, Yreg);
                    cp->V[Xreg] += cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0005: // Subtract Vy from Vx, set VF on borrow
                    printf("V%x -= V%x\n", Xreg, Yreg);
                    cp->V[Xreg] -= cp->V[Yreg];
                    cp->PC += 2;
                    break;
                case 0x0006: // Shift Vx right by 1
                    printf("V%x >>= V%x\n", Xreg, Yreg);
                    cp->V[Xreg] >>= 1;
                    cp->PC += 2;
                    break;
                case 0x0007: // Set Vx = Vy - Vx, set VF on borrow
                    printf("V%x =- V%x\n", Xreg, Yreg);
                    cp->V[Xreg] = cp->V[Yreg] - cp->V[Xreg];
                    cp->PC += 2;
                    break;
                case 0x000E: // Shift Vx left by 1
                    printf("V%x <<= V%x\n", Xreg, Yreg);
                    cp->V[Xreg] <<= 1;
                    cp->PC += 2;
                    break;
            }
            break;
        case 0x9000: // Skip next instruction if Vx != Vy
            printf("if V%x != V%x\n", Xreg, Yreg);
            if (cp->V[Xreg] != cp->V[Yreg]) {
                cp->PC += 4;
            } else {
                cp->PC += 2;
            }
            break;
        case 0xA000: // Set I = NNN
            printf("I := 0x%x\n", NNN);
            cp->I = NNN;
            cp->PC += 2;
            break;
        case 0xB000: // Jump to address NNN + V0
            printf("jump0 0x%x\n", NNN);
            cp->PC = NNN + cp->V[0];
            break;
        case 0xC000: // Set Vx = random byte AND NN
            printf("V%x := random 0x%x\n", Xreg, NN);
            cp->V[Xreg] = (rand() % 256) & NN;
            cp->PC += 2;
            break;
        case 0xD000: // Draw sprite at (Vx, Vy)
            printf("sprite %x %x %x\n", cp->V[Xreg], cp->V[Yreg], N);
            cp->V[0xF] = 0;
            for (int yline = 0; yline < N; yline++) {
                pixel = cp->memory[cp->I + yline];
                for (int xline = 0; xline < 8; xline++) {
                    if ((pixel & (0x80 >> xline)) != 0) {
                        if (cp->gfx[cp->V[Yreg] + yline][cp->V[Xreg] + xline] == 1)
                            cp->V[0xF] = 1;  // Collision
                        cp->gfx[cp->V[Yreg] + yline][cp->V[Xreg] + xline] ^= 1;
                        if (cp->gfx[cp->V[Yreg] + yline][cp->V[Xreg] + xline]){
                            SDL_SetRenderDrawColor(app->renderer, 255,255,255, 255);
                            drawPixel(app, cp->V[Xreg]+xline, cp->V[Yreg]+yline );
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
            switch(ef_operation) {
                case 0x009E: // Skip next instruction if key with value Vx is pressed
                    printf("if V%x -key then\n", Xreg);
                    break;
                case 0x00A1: // Skip next instruction if key with value Vx is not pressed
                    printf("if V%x key then\n", Xreg);
                    break;
            }
            break;
        case 0xF000:
            switch(ef_operation) {
                case 0x0007: // Set Vx = delay_timer
                    printf("V%x := delay\n", Xreg);
                    cp->V[Xreg] = cp->delay_timer;
                    cp->PC += 2;
                    break;
                case 0x000A: // Wait for key press, store key value in Vx
                    printf("V%x := key \n", Xreg);
                    // TODO: Implement key wait logic
                    break;
                case 0x0015: // Set delay_timer = Vx
                    printf("delay := V%x \n", Xreg);
                    cp->delay_timer = cp->V[Xreg];
                    cp->PC += 2;
                    break;
                case 0x0018: // Set sound_timer = Vx
                    printf("buzzer := V%x \n", Xreg);
                    cp->sound_timer = cp->V[Xreg];
                    cp->PC += 2;
                    break;
                case 0x001E: // Set I = I + Vx
                    printf("I += V%x \n", Xreg);
                    cp->I += cp->V[Xreg];
                    cp->PC += 2;
                    break;
                case 0x0029: // Set I = location of sprite for digit Vx
                    printf("i := hex V%x \n", Xreg);
                    cp->PC += 2;
                    break;
                case 0x0033: // Store BCD of Vx in memory locations I, I+1, I+2
                    printf("BCD V%x \n", Xreg);
                    cp->memory[cp->I] = cp->V[Xreg] / 100;
                    cp->memory[cp->I + 1] = (cp->V[Xreg] / 10) % 10;
                    cp->memory[cp->I + 2] = cp->V[Xreg] % 10;
                    cp->PC += 2;
                    break;
                case 0x0055: // Store registers V0 through Vx in memory starting at I
                    printf("Save V%x \n", Xreg);
                    for (int i = 0; i <= Xreg; i++) {
                        cp->memory[cp->I + i] = cp->V[i];
                    }
                    cp->PC += 2;
                    break;
                case 0x0065: // Read registers V0 through Vx from memory starting at I
                    printf("Load V%x \n", Xreg);
                    for (int i = 0; i <= Xreg; i++) {
                        cp->V[i] = cp->memory[cp->I + i];
                    }
                    cp->PC += 2;
                    break;
            }
            break;

        default:
            printf("Unknown opcode\n");
            break;
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


void debug1NNN(Chip8* cp){
    // printf("");
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