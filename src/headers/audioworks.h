#ifndef AUDIOWORKS_H
#define AUDIOWORKS_H

#include "tracker.h"

class AudioW{
private:
    Tracker *t;
    AudioBuffer *b;

    Uint32 sample_count; // used to determine timing

public:

    AudioW(Tracker *tracker, AudioBuffer *buffer);

    ~AudioW();

    void play_note(SDL_Event *e); // plays note from keyboard

    void audio_works(); // run in program loop // generates buffer data

};

#endif
