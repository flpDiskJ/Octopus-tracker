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
#include <unistd.h>
#include <iostream>

#define REFRESH_RATE 60
#define CHANNELS 8 // number of channels
#define DISPLAYRECTS 40 // number of rects used to display steps
#define MAXSAMPLES 100 // max number of samples that can be used
#define MAXBLOCKS 200 // max number of blocks that can be used
#define BUFF_SIZE 512 // size of audio buffer. must be multiple of 16
#define BYTES_IN_SAMPLE 2 // number of bytes that makes up each sample (2 for 16bit format)
#define SAMPLE_RATE 48000 // output sample rate
#define AUDIO_CHANNELS 1 // 1 for mono // 2 for stereo
#define AMP_LEV 15 // master level // 255 max for Signed 16bit range without clipping

const int refresh_delay_ms = 1000 / REFRESH_RATE;

const int samples_in_ms = SAMPLE_RATE / 1000; // number of audio samples in 1 milisecond

struct Pallet{ // global color pallet
    SDL_Color black;
    SDL_Color red;
    SDL_Color blue;
    SDL_Color green;
};

struct AudioBuffer{ // global audio output buffer // passed into audio callback
    Uint8 *data;
    Uint32 len;
    Uint32 read_pos;
    Uint32 write_pos;
    bool stop;
    Uint32 time;
    void *tracker_class;
};

#endif
