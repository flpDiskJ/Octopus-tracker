#pragma once

#include "tracker.h"

class AudioW{
private:
    Tracker *t;
    AudioBuffer *b;
    Uint32 tick_count; // counts ticks for effects commands

public:
    Uint32 sample_count; // used to determine timing

    AudioW(Tracker *tracker, AudioBuffer *buffer);

    ~AudioW();

    void play_note(SDL_Event *e); // plays note from keyboard

    void play_sample(SDL_Event *e, int sample); // plays note from keyboard of specific sample only and always to channel 0

    void tick();

    void audio_works(); // run in program loop // generates buffer data

};
