#pragma once

#include "tracker.h"

class AudioW{
private:
    Tracker *t;
    AudioBuffer *b;
    SDL_PixelFormat *fmt;
    Pallet *pallet;
    Uint32 tick_count; // counts ticks for effects commands

    void generate_scope(int index);

public:
    Uint32 sample_count; // used to determine timing
    Sint32 *wav_data = NULL; // used for export

    AudioW(Tracker *tracker, AudioBuffer *buffer, SDL_PixelFormat *f, Pallet *plt);

    ~AudioW();

    void play_note(SDL_Event *e); // plays note from keyboard

    void play_sample(SDL_Event *e, int sample, Uint32 range_start); // plays note from keyboard of specific sample only and always to channel 0

    void tick();

    void audio_works(); // run in program loop // generates buffer data

    Uint32 prepare_export(); // returns length of buffer

};
