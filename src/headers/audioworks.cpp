#include "audioworks.h"

AudioW::AudioW(Tracker *tracker, AudioBuffer *buffer)
{
    t = tracker;
    b = buffer;
    sample_count = 0;
    tick_count = 0;
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
        t->channel[t->cursor_channel].pos_adv = (double)t->getFreq(note, key, oct) / (double)SAMPLE_RATE;
        t->channel[t->cursor_channel].amplifier = (double)t->sample[t->s_pos].level / 100.0;
        t->channel[t->cursor_channel].pitch_mod = 1;
        t->channel[t->cursor_channel].play = true;
        t->channel[t->cursor_channel].reverse = false;
    }
}

void AudioW::tick()
{
    for (int c = 0; c < CHANNELS; c++)
    {
        switch (t->channel[c].command_type)
        {
            case COM_PITCH_UP:
            case COM_PITCH_DOWN:
                t->channel[c].pos_adv *= t->channel[c].pitch_mod;
                break;
            case COM_SLIDE:
                if (t->channel[c].slide_pos > t->channel[c].slide_target)
                {
                    t->channel[c].slide_pos -= t->channel[c].slide_speed;
                } else if (t->channel[c].slide_pos < t->channel[c].slide_target)
                {
                    t->channel[c].slide_pos += t->channel[c].slide_speed;
                }
                t->channel[c].pos_adv = (double)t->channel[c].slide_pos / (double)SAMPLE_RATE;
                break;
            case COM_ARPEGGIO:
                if (t->channel[c].arp_toggle < 2)
                {
                    t->channel[c].arp_toggle++;
                } else {
                    t->channel[c].arp_toggle = 0;
                }
                t->channel[c].pos_adv = (double)t->channel[c].arp_rates[t->channel[c].arp_toggle] / (double)SAMPLE_RATE;
                break;
            case COM_VIBRATO:
                if (t->channel[c].vib_up)
                {
                    if (t->channel[c].vib_pos < t->channel[c].vib_high)
                    {
                        t->channel[c].vib_pos += t->channel[c].vib_speed;
                    } else {
                        t->channel[c].vib_up = false;
                    }
                } else {
                    if (t->channel[c].vib_pos > t->channel[c].vib_low)
                    {
                        t->channel[c].vib_pos -= t->channel[c].vib_speed;
                    } else {
                        t->channel[c].vib_up = true;
                    }
                }
                t->channel[c].pos_adv = (double)t->channel[c].vib_pos / (double)SAMPLE_RATE;
                break;
            case COM_LEVEL_FADE:
                if (t->channel[c].command_param[0] > 0)
                {
                    if (t->channel[c].amplifier < 2.0)
                    {
                        t->channel[c].amplifier += (double)t->channel[c].command_param[0] * VOLUME_SLIDE_AMOUNT;
                    }
                } else if (t->channel[c].command_param[1] > 0)
                {
                    if (t->channel[c].amplifier > 0.0)
                    {
                        t->channel[c].amplifier -= (double)t->channel[c].command_param[1] * VOLUME_SLIDE_AMOUNT;
                    }
                }
                break;
            default:
                break;
        }
    }
}

void AudioW::audio_works() // fills audio buffer
{
    int actual_pos;
    int val;
    Sint16 out;
    int sig_max[CHANNELS];
    int temp;
    for (int c = 0; c < CHANNELS; c++)
    {
        sig_max[c] = 0;
    }
    if (b->stop)
    {
        for (int c = 0; c < CHANNELS; c++)
        {
            t->channel[c].play = false;
            t->channel[c].command_type = COM_NONE;
        }
        b->stop = false;
    }
    while (b->write_pos != b->read_pos)
    {
        // timing
        if (sample_count >= t->timing_delay)
        {
            t->move_step();
            sample_count = 0;
        } else {
            sample_count++;
        }

        // ticks
        if (tick_count >= TICK_LEN)
        {
            tick();
            tick_count = 0;
        } else {
            tick_count++;
        }

        val = 0;
        for (int c = 0; c < CHANNELS; c++) // c for channel
        {
            if (t->channel[c].play && t->mute[c] == false)
            {
                actual_pos = (int)t->channel[c].pos;
                if (t->sample[t->channel[c].sample].len != 0 && actual_pos < t->sample[t->channel[c].sample].len && actual_pos >= 0)
                {
                    temp = t->sample[t->channel[c].sample].data[actual_pos] * t->channel[c].amplifier;
                    val += temp;
                    if (t->channel[c].reverse)
                    {
                        t->channel[c].pos -= t->channel[c].pos_adv;
                    } else {
                        t->channel[c].pos += t->channel[c].pos_adv;
                    }
                    if (sig_max[c] < temp)
                    {sig_max[c] = temp;}
                } else {
                    t->channel[c].play = false; // stop channel playback if sample reaches end or sample is empty
                }
            }
        }
        temp = val / CHANNELS;
        temp = temp * AMP_LEV;
        if (temp > 32766){temp = 32766;}
        else if (temp < -32766){temp = -32766;}
        out = temp;
        b->data[b->write_pos] = out & 0xFF;
        b->data[b->write_pos+1] = out >> 8 & 0xFF;
        if (b->write_pos >= b->len)
        {
            b->write_pos = 0;
        } else {
            b->write_pos += BYTES_IN_SAMPLE;
        }
    }
    for (int c = 0; c < CHANNELS; c++)
    {
        t->set_trigger_bar(c, sig_max[c]);
    }
}
