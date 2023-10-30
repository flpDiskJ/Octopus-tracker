#include "audioworks.h"

void audio_callback(void* buffer, Uint8* stream, int len)
{
    memset(stream, 0, len);
    SDL_MixAudio(stream, (Uint8*)buffer, BUFF_SIZE, SDL_MIX_MAXVOLUME);
}

AudioW::AudioW(Tracker *tracker)
{
    t = tracker;
    mainFormat.format = AUDIO_S16; mainFormat.freq = sRate; mainFormat.channels = 1;
    mainFormat.callback = audio_callback; mainFormat.samples = BUFF_SIZE;
    mainFormat.userdata = buffer; // pass pointer to buffer data into callback

    buffer = (Uint8*)malloc(BUFF_SIZE);
    memset(buffer, 0, BUFF_SIZE);

    if (SDL_OpenAudio(&mainFormat, NULL) < 0)
    {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
    }

}

AudioW::~AudioW()
{
    free(buffer);
}

void AudioW::close_audio()
{
    SDL_CloseAudio();
}
