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
#define AUDIO_LATENCY 3 // 2 to 4
#define AMP_LEV 6 // master level
#define SAMPLE_RATE 48000 // output sample rate
#define BUFF_SIZE 1024 // size of audio stream buffer
#define SCOPE_CHUNK 800

// command tweaks
const int PITCH_SLIDE_AMOUNT = 30; // 01 & 02 & 03
const double VOLUME_SLIDE_AMOUNT = 0.01; // 0D
////

//// NO TOUCHY!
#define CHANNELS 8 // number of channels
#define DISPLAYRECTS 40 // number of rects used to display steps
#define MAXSAMPLES 100 // max number of samples that can be used
#define MAXBLOCKS 200 // max number of blocks that can be used
#define BYTES_IN_SAMPLE 4 // number of bytes that makes up each sample (2 for 16bit format)
#define AUDIO_CHANNELS 2 // 1 for mono // 2 for stereo

const int BIT_REDUCT = 16;
const int refresh_delay_ms = 1000 / REFRESH_RATE;
const int TICK_LEN = SAMPLE_RATE / 32;
const double SEMITONE_MULTIPLIER = 1.0595555;
const double HALF_SEMITONE_MULTIPLIER = 1.02977775;
const double FINETUNE_MULTIPLIER = 1.005;
const int AUDIO_PEAK = 32760;
const int AUDIO_PEAK_LOW = -32760;
const int SAMPLE_PEAK = AUDIO_PEAK / BIT_REDUCT;

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
const Uint8 COM_ARPEGGIO = 7;
const Uint8 COM_VIBRATO = 8;
const Uint8 COM_LEVEL_FADE = 9; // 0D
const Uint8 COM_REVERSE = 10; // 0A
const Uint8 COM_TREMOLO = 11; // 07
const Uint8 COM_HOLD_AND_DECAY = 12; // 08
const Uint8 COM_DELAY = 13; // 05
const Uint8 COM_RETRIG = 14; // 06

struct Pallet{ // global color pallet
    SDL_Color bgd; // backround
    SDL_Color black;
    SDL_Color red;
    SDL_Color blue;
    SDL_Color green;
    SDL_Color white;
    SDL_Color highlight;
};

struct Button{
    SDL_Rect r;
    SDL_Texture *t = NULL;
};

struct Entry{
    SDL_Rect r;
    SDL_Texture *t = NULL;
    bool active = false;
    string text;
};

struct Slider{
    SDL_Rect slide;
    SDL_Rect pos;
};

struct AudioBuffer{ // global audio output buffer // passed into audio callback
    Uint8 *data = NULL;
    Uint32 len = 0;
    Uint32 read_pos = 0;
    Uint32 write_pos = 0;
    bool stop = false;
};
