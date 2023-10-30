#ifndef AUDIOWORKS_H
#define AUDIOWORKS_H

#include "tracker.h"

void audio_callback(void* userdata, Uint8* stream, int len);

class AudioW{
private:
    const int sRate = 48000;
    SDL_AudioSpec mainFormat;
    Tracker *t;
    Uint8 *buffer;

public:

    AudioW(Tracker *tracker);

    ~AudioW();

    void close_audio();

};

#endif
