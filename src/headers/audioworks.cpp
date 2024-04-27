#include "audioworks.h"

AudioW::AudioW(Tracker *tracker, AudioBuffer *buffer, SDL_Renderer *ren, SDL_Window *window, Pallet *plt)
{
    t = tracker;
    b = buffer;
    render = ren;
    pallet = plt;
    sample_count = 0;
    tick_count = 0;

    fmt = SDL_AllocFormat(SDL_GetWindowPixelFormat(window));

    for (int c = 0; c < CHANNELS; c++)
    {
        scope[c].r.w = 120;
        scope[c].r.h = 80;
        scope[c].r.x = 48 + (155 * c);
        scope[c].r.y = 710;
        scope[c].t = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, scope[c].r.w / 2, scope[c].r.h / 2);
        scope[c].data_size = SAMPLE_RATE / REFRESH_RATE;
        scope[c].data = (int*)malloc(sizeof(int)*scope[c].data_size);
    }
    scope_idle_t = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, scope[0].r.w / 2, scope[0].r.h / 2);
    void *pixels;
    int pitch;
    SDL_LockTexture(scope_idle_t, NULL, &pixels, &pitch);
    for (int x = 0; x < scope[0].r.w / 2; x++)
    {
        for (int y = 0; y < scope[0].r.h / 2; y++)
        {
            if (y == scope[0].r.h / 4)
            {
                ((Uint32*)pixels)[x+(y*(scope[0].r.w/2))] = SDL_MapRGB(fmt, pallet->white.b, pallet->white.g, pallet->white.r);
            } else {
                ((Uint32*)pixels)[x+(y*(scope[0].r.w/2))] = SDL_MapRGB(fmt, pallet->bgd.b, pallet->bgd.g, pallet->bgd.r);
            }
        }
    }
    SDL_UnlockTexture(scope_idle_t);
}

AudioW::~AudioW()
{
    for (int c = 0; c < CHANNELS; c++)
    {
        free(scope[c].data);
    }
}

void AudioW::generate_scope(int index)
{
    void *pixels;
    int pitch;
    int scale = SAMPLE_PEAK / ((scope[index].r.h / 2) - 1);
    double pos = 0;
    double pos_adv = (double)scope[index].data_size / (double)(scope[index].r.w/2);
    int actual_pos = 0;
    SDL_LockTexture(scope[index].t, NULL, &pixels, &pitch);
    for (int x = 0; x < scope[index].r.w / 2; x++)
    {
        for (int y = 0; y < scope[index].r.h / 2; y++)
        {
            if (y == (scope[index].data[actual_pos] / scale) + (scope[index].r.h / 4))
            {
                ((Uint32*)pixels)[x+(y*(scope[index].r.w/2))] = SDL_MapRGB(fmt, pallet->black.b, pallet->black.g, pallet->black.r);
            } else {
                ((Uint32*)pixels)[x+(y*(scope[index].r.w/2))] = SDL_MapRGB(fmt, pallet->bgd.b, pallet->bgd.g, pallet->bgd.r);
            }
        }
        pos += pos_adv;
        actual_pos = (int)pos;
    }
    SDL_UnlockTexture(scope[index].t);
}

void AudioW::render_scopes()
{
    for (int c = 0; c < CHANNELS; c++)
    {
        if (scope[c].active)
        {
            SDL_RenderCopy(render, scope[c].t, NULL, &scope[c].r);
        } else {
            SDL_RenderCopy(render, scope_idle_t, NULL, &scope[c].r);
        }
        SDL_RenderDrawRect(render, &scope[c].r);
    }
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
        t->channel[t->cursor_channel].play = true;
        t->channel[t->cursor_channel].reverse = false;
    }
}

void AudioW::play_sample(SDL_Event *e, int sample)
{
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
    if (note != '-' && t->sample[sample].len != 0)
    {
        t->channel[0].pos = 0;
        t->channel[0].sample = sample;
        t->channel[0].pos_adv = (double)t->getFreq(note, key, oct) / (double)SAMPLE_RATE;
        t->channel[0].amplifier = (double)t->sample[sample].level / 100.0;
        t->channel[0].play = true;
        t->channel[0].reverse = false;
    }
}

void AudioW::tick()
{
    for (int c = 0; c < CHANNELS; c++)
    {
        if (t->channel[c].hold_and_decay) // 08
        {
            if (t->channel[c].hold > 0)
            {
                t->channel[c].hold--;
            } else if (t->channel[c].trem_current > t->channel[c].decay) {
                t->channel[c].trem_current -= t->channel[c].decay;
                t->channel[c].amplifier = (double)t->channel[c].trem_current / 100.0;
            } else {
                t->channel[c].play = false;
            }
        }
        if (t->channel[c].retriggers > 0 && t->channel[c].retrig_freq > 0)
        {
            if (t->channel[c].retrig_count >= t->channel[c].retrig_freq)
            {
                t->channel[c].retrig_count = 0;
                t->channel[c].retriggers--;
                t->channel[c].pos = (double)t->channel[c].trigger_pos;
                t->channel[c].amplifier -= 1.0 / (double)t->channel[c].total_triggers;
            } else {
                t->channel[c].retrig_count++;
            }
        }
        switch (t->channel[c].command_type)
        {
            case COM_PITCH_UP:
                t->channel[c].slide_pos += t->channel[c].slide_speed;
                t->channel[c].pos_adv = (double)t->channel[c].slide_pos / (double)SAMPLE_RATE;
                break;
            case COM_PITCH_DOWN:
                if (t->channel[c].slide_pos > 0)
                {
                    t->channel[c].slide_pos -= t->channel[c].slide_speed;
                }
                t->channel[c].pos_adv = (double)t->channel[c].slide_pos / (double)SAMPLE_RATE;
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
            case COM_TREMOLO:
                if (t->channel[c].trem_up)
                {
                    if (t->channel[c].trem_current < t->channel[c].trem_start - t->channel[c].trem_speed)
                    {
                        t->channel[c].trem_current += t->channel[c].trem_speed;
                    } else {
                        t->channel[c].trem_up = false;
                    }
                } else {
                    if (t->channel[c].trem_current > t->channel[c].trem_depth + t->channel[c].trem_speed)
                    {
                        t->channel[c].trem_current -= t->channel[c].trem_speed;
                    } else {
                        t->channel[c].trem_up = true;
                    }
                }
                t->channel[c].amplifier = (double)t->channel[c].trem_current / 100.0;
                break;
            default:
                break;
        }
    }
}

void AudioW::audio_works() // fills audio buffer
{
    long unsigned int actual_pos;
    long signed int val;
    Sint16 out;
    long int sig_max[CHANNELS];
    long signed int temp;
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
            // retrigger
            if (t->channel[c].retriggers > 0 && t->channel[c].retrig_freq == 0)
            {
                if (t->channel[c].retrig_count >= t->channel[c].total_triggers)
                {
                    t->channel[c].retrig_count = 0;
                    t->channel[c].retriggers--;
                    t->channel[c].pos = (double)t->channel[c].trigger_pos;
                } else {
                    t->channel[c].retrig_count++;
                }
            }

            if (t->channel[c].play && t->mute[c] == false)
            {
                scope[c].active = true;
                actual_pos = (int)t->channel[c].pos;
                if (t->sample[t->channel[c].sample].len != 0 && actual_pos < t->sample[t->channel[c].sample].len && actual_pos >= 0)
                {
                    temp = t->sample[t->channel[c].sample].data[actual_pos] * t->channel[c].amplifier;

                    scope[c].data[scope[c].data_pos] = temp;
                    scope[c].data_pos++;
                    if (scope[c].data_pos >= scope[c].data_size)
                    {
                        scope[c].data_pos = 0;
                        generate_scope(c);
                    }

                    val += temp;
                    if (t->channel[c].reverse)
                    {
                        // handles ping pong loop
                        if (t->sample[t->channel[c].sample].loop == 2 && actual_pos <= t->sample[t->channel[c].sample].loop_point)
                        {
                            t->channel[c].reverse = false;
                        }
                        t->channel[c].pos -= t->channel[c].pos_adv;
                    } else {
                        t->channel[c].pos += t->channel[c].pos_adv;
                    }
                    if (sig_max[c] < temp * BIT_REDUCT)
                    {sig_max[c] = temp * BIT_REDUCT;}
                } else {
                    // handle looping
                    if (t->sample[t->channel[c].sample].loop == 1)
                    {
                        t->channel[c].pos = t->sample[t->channel[c].sample].loop_point;
                    } else if (t->sample[t->channel[c].sample].loop == 2)
                    {
                        t->channel[c].reverse = true;
                        t->channel[c].pos -= 1;
                    } else {
                        t->channel[c].play = false; // stop channel playback if sample reaches end or sample is empty
                    }
                }
            } else {
                scope[c].active = false;
            }
        }
        temp = val / CHANNELS;
        temp = temp * AMP_LEV * BIT_REDUCT;
        if (temp > AUDIO_PEAK){temp = AUDIO_PEAK;}
        else if (temp < AUDIO_PEAK_LOW){temp = AUDIO_PEAK_LOW;}
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

void AudioW::deinit()
{
    SDL_FreeFormat(fmt);
}
