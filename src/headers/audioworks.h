#pragma once

#include "tracker.h"

class AudioW{
private:
    Tracker *t;
    SDL_Renderer *render;
    AudioBuffer *b;
    SDL_PixelFormat *fmt;
    Pallet *pallet;
    Uint32 tick_count; // counts ticks for effects commands

    struct Scope{
        SDL_Texture *t = NULL;
        SDL_Rect r;
        Uint8 *data = NULL;
        Uint16 data_size = 0;
        bool active = false;
    };

    Scope scope[CHANNELS];
    SDL_Texture *scope_idle_t;

public:
    Uint32 sample_count; // used to determine timing

    AudioW(Tracker *tracker, AudioBuffer *buffer, SDL_Renderer *ren, SDL_Window *window, Pallet *plt);

    ~AudioW();

    void generate_default_wave();

    void deinit();

    void render_scopes();

    void play_note(SDL_Event *e); // plays note from keyboard

    void play_sample(SDL_Event *e, int sample); // plays note from keyboard of specific sample only and always to channel 0

    void tick();

    void audio_works(); // run in program loop // generates buffer data

};
