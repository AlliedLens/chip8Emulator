# Chip8 Emulator Built in C

## Description

Beginner to emulating tries his hand at building one of the most basic emulator/interpreter, for the chip8, which is a language that used to run on devices from the 80s, using C. Oh I almost forgot, trying to do this project without any chatgpt,  ( I failed :( ).

## Usage

##### clone the repository
##### install SDL2, ask google how to do it and get it up and running
##### run `gcc main.c -o main -lSDL2` on the terminal to compile the main.c file
##### `./main (filepath to the rom) ` to get the rom running.
##### Use the text file with the corresponding rom name for instructions, keep in mind the keybindings are 

![Sample Image](keypad.jpeg)

## Project Structure

### Chip8Emulator 
#### --Roms
Contains all of the roms needed to load the program
#### blueprint.txt
Had the structure needed to for setup of the code
#### chip8.h
Has the basic structure of the chip8, as well as all of the useful commands needed for operations
#### debugging.h
Contained a bunch of code filled with printf statements and what have you.
#### graphics.h
Contains Functions useful to rendering graphics
#### main.c
Where the magic happens
#### utils.h
Wish I used this more, was supposed to have all the functions that i was using for something else.

## Resources

[I used this to get an overview of how a chip8 works](http://johnearnest.github.io/Octo/docs/chip8ref.pdf)\
[CHIP-8 - Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)\
[This tutorial provides the logic to make the chip8, refered to this mainly](https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)\
[This website has a working Chip8, helped me see what my ROMS were supposed to run like, very useful for debugging.](https://chipo.ber.gp/)\
[This Website Has like so many roms](https://github.com/kripod/chip8-roms/tree/master)\
[Has all of the machine code and its corresponding logical statement](http://johnearnest.github.io/Octo/docs/chip8ref.pdf)\
[Godsend, got the SDL rendering code from here.](https://github.com/guyus15)
[:( sad](https://chatgpt.com)
