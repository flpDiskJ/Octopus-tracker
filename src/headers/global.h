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
#define BUFF_SIZE 512 // size of audio buffer. must be power of 2
#define BYTES_IN_SAMPLE 2 // number of bytes that makes up each sample (2 for 16bit format)
#define SAMPLE_RATE 48000 // output sample rate
#define AUDIO_CHANNELS 1 // 1 for mono // 2 for stereo
#define AMP_LEV 15 // master level // 255 max for Signed 16bit range without clipping

struct Pallet{
    SDL_Color black;
    SDL_Color red;
    SDL_Color blue;
    SDL_Color green;
};

struct AudioBuffer{
    Uint8 *data;
    Uint32 len;
    Uint32 back_pos;
    bool update; // if true audio_works() fills buffer
    bool stop;
};

#endif
