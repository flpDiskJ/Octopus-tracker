#include "audioworks.h"

AudioW::AudioW(Tracker *tracker, AudioBuffer *buffer)
{
    t = tracker;
    b = buffer;
}

AudioW::~AudioW()
{

}

void AudioW::play_note(SDL_Event *e)
{
    if (t->cursor_pos != 0)
    {
        return;
    }
    // figure out note data
    char note = '-';
    char key = '-';
    int oct = t->octave;
    switch (e->key.keysym.sym)
    {
        case SDLK_z: note = 'C'; break;
        case SDLK_s: note = 'C'; key = '#'; break;
        case SDLK_l: note = 'C'; key = '#'; oct++; break;
        case SDLK_q:
        case SDLK_COMMA: note = 'C'; oct++; break;
        case SDLK_2: note = 'C'; key = '#'; oct++; break;
        case SDLK_i: note = 'C'; oct += 2; break;
        case SDLK_9: note = 'C'; key = '#'; oct += 2; break;
        case SDLK_x: note = 'D'; break;
        case SDLK_d: note = 'D'; break;
        case SDLK_PERIOD: note = 'D'; oct++; break;
        case SDLK_SEMICOLON: note = 'D'; key = '#'; oct++; break;
        case SDLK_w: note = 'D'; oct++; break;
        case SDLK_3: note = 'D'; key = '#'; oct++; break;
        case SDLK_o: note = 'D'; oct += 2; break;
        case SDLK_0: note = 'D'; key = '#'; oct += 2; break;
        case SDLK_c: note = 'E'; break;
        case SDLK_SLASH: note = 'E'; oct++; break;
        case SDLK_e: note = 'E'; oct++; break;
        case SDLK_p: note = 'E'; oct += 2; break;
        case SDLK_v: note = 'F'; break;
        case SDLK_g: note = 'F'; key = '#'; break;
        case SDLK_r: note = 'F'; oct++; break;
        case SDLK_5: note = 'F'; key = '#'; oct++; break;
        case SDLK_LEFTBRACKET: note = 'F'; oct += 2; break;
        case SDLK_EQUALS: note = 'F'; key = '#'; oct += 2; break;
        case SDLK_b: note = 'G'; break;
        case SDLK_h: note = 'G'; key = '#'; break;
        case SDLK_t: note = 'G'; oct++; break;
        case SDLK_6: note = 'G'; key = '#'; oct++; break;
        case SDLK_n: note = 'A'; break;
        case SDLK_j: note = 'A'; key = '#'; break;
        case SDLK_y: note = 'A'; oct++; break;
        case SDLK_7: note = 'A'; key = '#'; oct++; break;
        case SDLK_m: note = 'B'; break;
        case SDLK_u: note = 'B'; oct++; break;
        default:
            oct = 0;
            break;
    }
    if (oct != 0 && t->sample[t->s_pos].len != 0)
    {
        t->channel[t->cursor_channel].pos = 0;
        t->channel[t->cursor_channel].sample = t->s_pos;
        t->channel[t->cursor_channel].pos_adv = (double)t->getFreq(note, key, oct) / SAMPLE_RATE * 1.0;
        t->channel[t->cursor_channel].amplifier = 128.0 / (double)t->sample[t->s_pos].level;
        t->channel[t->cursor_channel].pitch_mod = 1;
        t->channel[t->cursor_channel].play = true;
    }
}

void AudioW::audio_works() // fills audio buffer
{
    int actual_pos;
    int val;
    Sint16 out;
    if (b->stop)
    {
        for (int c = 0; c < CHANNELS; c++)
        {
            t->channel[c].play = false;
        }
        b->stop = false;
    }
    while (b->write_pos != b->read_pos)
    {
        if (b->write_pos >= b->len)
        {
            b->write_pos = 0;
        } else {
            b->write_pos += BYTES_IN_SAMPLE;
        }
        val = 0;
        for (int c = 0; c < 8; c++) // c for channel
        {
            if (t->channel[c].play && t->mute[c] == false)
            {
                actual_pos = (int)t->channel[c].pos;
                if (t->sample[t->channel[c].sample].len != 0 && actual_pos < t->sample[t->channel[c].sample].len)
                {
                    val += t->sample[t->channel[c].sample].data[actual_pos] * t->channel[c].amplifier;
                    t->channel[c].pos += t->channel[c].pos_adv;
                    t->channel[c].pos_adv *= t->channel[c].pitch_mod;
                } else{
                    t->channel[c].play = false; // stop channel playback if sample reaches end or sample is empty
                }
            }
        }
        out = val / CHANNELS;
        b->data[b->write_pos] = out & 0xFF;
        b->data[b->write_pos+1] = out >> 8 & 0xFF;
        //b->pos = 0;
    }
}
