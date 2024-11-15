#include "audioworks.h"

AudioW::AudioW(Tracker *tracker, AudioBuffer *buffer, SDL_PixelFormat *f, Pallet *plt)
{
    t = tracker;
    b = buffer;
    pallet = plt;
    sample_count = 0;
    tick_count = 0;
    fmt = f;

    for (int c = 0; c < CHANNELS; c++)
    {
        lp_output[c] = 0.0;
    }
}

AudioW::~AudioW()
{

}

void AudioW::generate_scope(int index)
{
    void *pixels;
    int pitch;
    int scale = (SAMPLE_PEAK * 1.8) / ((t->scope[index].r.h / 2) - 1);
    double pos = 0;
    double pos_adv = (double)t->scope[index].data_size / (double)(t->scope[index].r.w/2);
    int actual_pos = 0;
    SDL_LockTexture(t->scope[index].t, NULL, &pixels, &pitch);
    for (int x = 0; x < t->scope[index].r.w / 2; x++)
    {
        for (int y = 0; y < t->scope[index].r.h / 2; y++)
        {
            if (y == (t->scope[index].data[actual_pos] / scale) + (t->scope[index].r.h / 4))
            {
                ((Uint32*)pixels)[x+(y*(t->scope[index].r.w/2))] = SDL_MapRGB(fmt, pallet->black.b, pallet->black.g, pallet->black.r);
            } else {
                ((Uint32*)pixels)[x+(y*(t->scope[index].r.w/2))] = SDL_MapRGB(fmt, pallet->bgd.b, pallet->bgd.g, pallet->bgd.r);
            }
        }
        pos += pos_adv;
        actual_pos = (int)pos;
    }
    SDL_UnlockTexture(t->scope[index].t);
    t->scope[index].texture_ready = true;
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
        t->channel[t->cursor_channel].pos_adv = (double)t->getFreq(note, key, oct, t->s_pos) / (double)SAMPLE_RATE;
        t->channel[t->cursor_channel].amplifier = (double)t->sample[t->s_pos].level / 100.0;
        t->channel[t->cursor_channel].play = true;
        t->channel[t->cursor_channel].reverse = false;
    }
}

void AudioW::play_sample(SDL_Event *e, int sample, Uint32 range_start)
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
        if (range_start >= t->sample[sample].len)
        {
            range_start = 0;
        }
        t->channel[CHANNELS-1].pos = range_start;
        t->channel[CHANNELS-1].sample = sample;
        t->channel[CHANNELS-1].pos_adv = (double)t->getFreq(note, key, oct, sample) / (double)SAMPLE_RATE;
        t->channel[CHANNELS-1].amplifier = (double)t->sample[sample].level / 100.0;
        t->channel[CHANNELS-1].play = true;
        t->channel[CHANNELS-1].reverse = false;
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

Sint32 AudioW::low_pass(Sint32 input, int c)
{
    Sint32 out;
    lp_output[c] = t->filter_vals[c] * input + t->filter_vals[c] * lp_output[c] - (t->filter_vals[c]-1) * lp_output[c];
    lp_output[c] /= (t->filter_vals[c]+1);
    out = (Sint32)lp_output[c];
    return out;
}

void AudioW::audio_works() // fills audio buffer
{
    long unsigned int actual_pos;
    Sint32 val_1 = 0, val_2 = 0;
    Sint16 out;
    long int sig_max[CHANNELS];
    Sint32 temp;
    Sint32 mix_1 = 0, mix_2 = 0, filter_1, filter_2;
    int mix_div = 0;
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
        // timer
        if (t->tracker_running)
        {
            timer_count++;
            if (timer_count >= SAMPLE_RATE)
            {
                timer_count = 0;
                if (t->timer_second < 59)
                {
                    t->timer_second++;
                } else {
                    t->timer_second = 0;
                    if (t->timer_minute < 99)
                    {
                        t->timer_minute++;
                    } else {
                        t->timer_minute = 0;
                    }
                }
                t->update_timer();
            }
        }

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
                t->scope[c].active = true;
                actual_pos = (int)t->channel[c].pos;
                if (t->sample[t->channel[c].sample].len != 0 && actual_pos < t->sample[t->channel[c].sample].len && actual_pos >= 0)
                {
                    mix_div = 0;
                    mix_1 = 0;
                    mix_2 = 0;
                    for (int scan_p = actual_pos; scan_p <= (int)(t->channel[c].pos+t->channel[c].pos_adv); scan_p++)
                    {
                        filter_1 = t->sample[t->channel[c].sample].data[actual_pos] * t->channel[c].amplifier * t->channel[c].left_level;
                        filter_2 = t->sample[t->channel[c].sample].data[actual_pos] * t->channel[c].amplifier * t->channel[c].right_level;

                        // LP filtering
                        if (t->low_pass_cutoff[c] != 0)
                        {
                            filter_1 = low_pass(filter_1, c);
                            filter_2 = low_pass(filter_2, c);
                        }

                        mix_1 += filter_1;
                        mix_2 += filter_2;
                        mix_div++;
                    }

                    mix_1 = mix_1 / mix_div;
                    mix_2 = mix_2 / mix_div;
                    temp = (mix_1 + mix_2) / 2;

                    t->scope[c].data[t->scope[c].data_pos] = (int)temp;
                    t->scope[c].data_pos++;
                    if (t->scope[c].data_pos >= t->scope[c].data_size - 1)
                    {
                        t->scope[c].data_pos = 0;
                        generate_scope(c);
                    }

                    if (sig_max[c] < temp * BIT_REDUCT)
                    {sig_max[c] = temp * BIT_REDUCT;}

                    mix_1 *= AMP_LEV;
                    mix_1 *= BIT_REDUCT;
                    mix_2 *= AMP_LEV;
                    mix_2 *= BIT_REDUCT;
                    val_1 += mix_1;
                    val_2 += mix_2;
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
                if (t->scope[c].active)
                {
                    t->scope[c].active = false;
                }
            }
        }
        val_1 = val_1 / CHANNELS;
        val_1 = val_1 * t->mix_level_adjust;
        if (val_1 > AUDIO_PEAK){val_1 = AUDIO_PEAK;}
        else if (val_1 < AUDIO_PEAK_LOW){val_1 = AUDIO_PEAK_LOW;}
        val_2 = val_2 / CHANNELS;
        val_2 = val_2 * t->mix_level_adjust;
        if (val_2 > AUDIO_PEAK){val_2 = AUDIO_PEAK;}
        else if (val_2 < AUDIO_PEAK_LOW){val_2 = AUDIO_PEAK_LOW;}
        out = (Sint16)val_1;
        b->data[b->write_pos] = out & 0xFF;
        b->data[b->write_pos+1] = out >> 8 & 0xFF;
        out = (Sint16)val_2;
        b->data[b->write_pos+2] = out & 0xFF;
        b->data[b->write_pos+3] = out >> 8 & 0xFF;
        if (b->write_pos >= b->len)
        {
            b->write_pos = 0;
        } else {
            b->write_pos += BYTES_IN_SAMPLE;
        }
        val_1 = 0;
        val_2 = 0;
    }
    for (int c = 0; c < CHANNELS; c++)
    {
        t->set_trigger_bar(c, sig_max[c]);
    }
}

Uint32 AudioW::prepare_export()
{
    long unsigned int actual_pos = 0;
    Sint32 val_1 = 0, val_2 = 0;
    Sint32 temp = 0;
    sample_count = 0;
    tick_count = 0;
    Uint32 buffer_len = 0;
    Uint32 buffer_size = SAMPLE_RATE * 30;
    Sint32 mix_1 = 0, mix_2 = 0, filter_1, filter_2;
    int mix_div = 0;
    wav_data = (Sint32*)malloc(sizeof(Sint32)*(buffer_size + 2));

    for (int c = 0; c < CHANNELS; c++)
    {
        t->channel[c].play = false;
        t->channel[c].command_type = COM_NONE;
    }

    t->sq_pos = 0;
    t->b_pos = t->sequence[t->sq_pos];
    t->pos = 0;
    t->tracker_running = true;
    t->run_sequence = true;
    t->sequence_end = false;
    t->set_timing_delay();
    t->note_trigger();

    while (!t->sequence_end)
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
                actual_pos = (int)t->channel[c].pos;
                if (t->sample[t->channel[c].sample].len != 0 && actual_pos < t->sample[t->channel[c].sample].len && actual_pos >= 0)
                {
                    mix_div = 0;
                    mix_1 = 0;
                    mix_2 = 0;
                    for (int scan_p = actual_pos; scan_p <= (int)(t->channel[c].pos+t->channel[c].pos_adv); scan_p++)
                    {
                        filter_1 = t->sample[t->channel[c].sample].data[actual_pos] * t->channel[c].amplifier * t->channel[c].left_level;
                        filter_2 = t->sample[t->channel[c].sample].data[actual_pos] * t->channel[c].amplifier * t->channel[c].right_level;

                        // LP filtering
                        if (t->low_pass_cutoff[c] != 0)
                        {
                            filter_1 = low_pass(filter_1, c);
                            filter_2 = low_pass(filter_2, c);
                        }

                        mix_1 += filter_1;
                        mix_2 += filter_2;
                        mix_div++;
                    }

                    mix_1 = mix_1 / mix_div;
                    mix_2 = mix_2 / mix_div;
                    mix_1 *= AMP_LEV;
                    mix_1 *= BIT_REDUCT;
                    mix_2 *= AMP_LEV;
                    mix_2 *= BIT_REDUCT;
                    val_1 += mix_1;
                    val_2 += mix_2;
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
            }
        }
        val_1 = val_1 / CHANNELS;
        val_1 = val_1 * t->mix_level_adjust;
        if (val_1 > AUDIO_PEAK){val_1 = AUDIO_PEAK;}
        else if (val_1 < AUDIO_PEAK_LOW){val_1 = AUDIO_PEAK_LOW;}
        val_2 = val_2 / CHANNELS;
        val_2 = val_2 * t->mix_level_adjust;
        if (val_2 > AUDIO_PEAK){val_2 = AUDIO_PEAK;}
        else if (val_2 < AUDIO_PEAK_LOW){val_2 = AUDIO_PEAK_LOW;}
        wav_data[buffer_len] = val_1 * 200;
        wav_data[buffer_len+1] = val_2 * 200;
        buffer_len += 2;
        if (buffer_len >= buffer_size)
        {
            buffer_size += SAMPLE_RATE * 30;
            wav_data = (Sint32*)realloc(wav_data, sizeof(Sint32)*(buffer_size + 2));
        }
    }

    t->sq_pos = 0;
    t->b_pos = t->sequence[t->sq_pos];
    t->tracker_running = false;
    t->run_sequence = false;

    for (int c = 0; c < CHANNELS; c++)
    {
        t->channel[c].play = false;
        t->channel[c].command_type = COM_NONE;
    }

    return buffer_len / 2;
}
