#ifndef GRAPHICS
#define GRAPHICS

#include <SDL2/SDL.h>
#include <stdio.h>
#include "chip8.h"

typedef struct
{
    SDL_Window *window; //The window we'll be rendering to
    SDL_Surface* screenSurface; //The surface contained by the window
    SDL_Renderer* renderer;

} SDLapp;


void setUpGraphics(SDLapp* app){
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }else{
        printf("SDL has initialized.\n");
    }

    app->window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (app->window == NULL){
        printf("Window could not be created! %s\n", SDL_GetError());
    }

    printf("Window Initialized\n");
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_SOFTWARE);

    if (app->renderer == NULL){
        printf("Renderer has not been created %s.\n", SDL_GetError());
    }
    printf("Renderer has been created.\n");

    SDL_SetRenderDrawColor(app->renderer, 0,0,0,255);
}

void drawPixel(SDLapp* app, char x, char y)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {	
            SDL_RenderDrawPoint(app->renderer, (x * 8) + i , (y * 8) + j);
        }
    }	
}


#endif