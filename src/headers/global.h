#pragma once

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

//// Customable
#define REFRESH_RATE 60
#define AUDIO_LATENCY 6 // 2 or 4 for responsivness // 6 or 8 for less glitching
#define AMP_LEV 1 // master level
#define SAMPLE_RATE 48000 // output sample rate
#define BUFF_SIZE 1024 // size of audio stream buffer. must be pow ^ 2

// command tweaks
const double PITCH_SLIDE_TUNE = 0.006; // 01 02
const int TARGET_SLIDE_SENS = 30; // 03
////

//// Changes will not work without modifing the code
#define CHANNELS 8 // number of channels
#define DISPLAYRECTS 40 // number of rects used to display steps
#define MAXSAMPLES 100 // max number of samples that can be used
#define MAXBLOCKS 200 // max number of blocks that can be used
#define BYTES_IN_SAMPLE 2 // number of bytes that makes up each sample (2 for 16bit format)
#define AUDIO_CHANNELS 1 // 1 for mono // 2 for stereo
////

//// NO TOUCHY!
const int refresh_delay_ms = 1000 / REFRESH_RATE;
const int TICK_LEN = SAMPLE_RATE / 24;

// flags for sequencer function
const Uint8 APPEND_SEQ = 50;
const Uint8 INSERT_SEQ = 100;
const Uint8 DELETE_SEQ = 150;
const Uint8 INC_SEQ = 200;
const Uint8 DEC_SEQ = 250;

// command type flags
const Uint8 COM_NONE = 0; // No command
const Uint8 COM_KILL = 1; // kills all sound in channel
const Uint8 COM_SET_LEVEL = 2; // set channel volume
const Uint8 COM_PITCH_UP = 3;
const Uint8 COM_PITCH_DOWN = 4;
const Uint8 COM_OFFSET = 5; // set sample offset
const Uint8 COM_SLIDE = 6; // slide pitch to target note

struct Pallet{ // global color pallet
    SDL_Color bgd; // backround
    SDL_Color black;
    SDL_Color red;
    SDL_Color blue;
    SDL_Color green;
};

struct Button{
    SDL_Rect r;
    SDL_Texture *t;
};

struct Slider{
    SDL_Rect slide;
    SDL_Rect pos;
};

struct AudioBuffer{ // global audio output buffer // passed into audio callback
    Uint8 *data;
    Uint32 len;
    Uint32 read_pos;
    Uint32 write_pos;
    bool stop;
};
