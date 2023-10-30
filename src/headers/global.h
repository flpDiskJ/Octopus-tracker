#ifndef GLOBAL
#define GLOBAL

#include <stdio.h>
using namespace std;
#include <SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <string.h>
#include <cmath>
#include <sstream>

#define CHANNELS 8 // number of channels
#define DISPLAYRECTS 40 // number of rects used to display steps
#define MAXSAMPLES 100 // max number of samples that can be used
#define MAXBLOCKS 200 // max number of blocks that can be used
#define BUFF_SIZE 1024 // size of audio buffer

struct Pallet{
    SDL_Color black;
    SDL_Color red;
    SDL_Color blue;
    SDL_Color green;
};

#endif
