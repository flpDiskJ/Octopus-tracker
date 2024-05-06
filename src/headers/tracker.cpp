#include "tracker.h"

Tracker::Tracker(SDL_Renderer *tracker_renderer, TTF_Font *gFont, Pallet *pallet)
{ // default constructor with no arguments
    renderer = tracker_renderer;
    font = gFont;
    p = pallet;

    tracker_box.x = 10;
    tracker_box.y = 20;
    tracker_box.w = 1261;
    tracker_box.h = 680;

    sequence_display.x = 20;
    sequence_display.y = 0;
    sequence_display.w = 84;
    sequence_display.h = 20;

    block_display.x = 116;
    block_display.y = 0;
    block_display.w = 84;
    block_display.h = 20;

    blkname_displayrect.y = 0;
    blkname_displayrect.w = 120;
    blkname_displayrect.x = 212;
    blkname_displayrect.h = 20;

    sample_display.x = 344;
    sample_display.y = 0;
    sample_display.w = 84;
    sample_display.h = 20;

    sample_name.x = 440;
    sample_name.y = 0;
    sample_name.w = 180;
    sample_name.h = 20;

    octave_display.x = 632;
    octave_display.y = 0;
    octave_display.w = 96;
    octave_display.h = 20;

    skip_display.x = 740;
    skip_display.y = 0;
    skip_display.w = 84;
    skip_display.h = 20;

    cursor.x = 49;
    cursor.y = 360;
    cursor.w = 39;
    cursor.h = 17;

    block_highlight.r.x = skip_display.x + skip_display.w + 5;
    block_highlight.r.y = 0;
    block_highlight.r.w = 14 * 12;
    block_highlight.r.h = 20;

    default_pitch.note = 'C';
    default_pitch.key = '-';
    default_pitch.octave = 3;

    int xLoc = 60;
    for (int bar = 0; bar < CHANNELS; bar++)
    {
        trigger_bars[bar].w = 18;
        trigger_bars[bar].h = 200;
        trigger_bars[bar].x = xLoc;
        trigger_bars[bar].y = 21;
        xLoc += 156;
    }

    for (int s = 0; s < MAXSAMPLES; s++)
    {
        sample[s].level = 0;
        sample[s].len = 0;
        sample[s].tune = 0;
        sample[s].fine_tune = 0;
        sample[s].data = NULL;
    }

    for (int b = 0; b < MAXBLOCKS; b++)
    {
        for (int c = 0; c < CHANNELS; c++)
        {
            block[b].channel[c] = NULL;
        }
    }

    sequence_size = seq_size_chunk;
    sequence = (Uint8*)malloc(sizeof(Uint8)*sequence_size);
    sequence[0] = 0;
    sequence_len = 1;

    for (int x = 0; x < 10; x++) // clear note_buffer
    {
        note_buffer[x].note = '-';
        note_buffer[x].key = '-';
        note_buffer[x].octave = 0;
        note_buffer[x].sample = 0;
        note_buffer[x].command[0] = '0';
        note_buffer[x].command[1] = '0';
        note_buffer[x].command[2] = '\0';
        note_buffer[x].parameter[0] = '0';
        note_buffer[x].parameter[1] = '0';
        note_buffer[x].parameter[2] = '\0';
    }

    block_buffer.length = 0;
    channel_buffer.length = 0;

    total_blocks = 1;
    block[0].length = 64;
    block[0].speed = 4;
    for (int c = 0; c < CHANNELS; c++)
    {
        block[0].channel[c] = (Note*)malloc(64*sizeof(Note));
        mute[c] = false;
        channel[c].play = false;
        channel[c].command_type = COM_NONE;
        channel[c].vib_up = true;
        channel[c].reverse = false;
    }
    clear_block(0);

    for (int s = 0; s < DISPLAYRECTS; s++) // init step display rects
    {
        displayrects[s].x = 10;
        displayrects[s].y = (s * 17)+20;
        displayrects[s].w = 1261;
        displayrects[s].h = 17;
    }
    for (int s = 0; s < DISPLAYRECTS; s++) // initializes throwaway textures so that SDL_DestroyTexture() doesn't throw an error.
    {
        displaytextures[s] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2);
    }

    sequence_display_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2);
    block_display_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2);
    sample_display_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2);
    blkname_displaytex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2);
    octave_display_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2);
    sample_name_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2);
    skip_display_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2);

    update_steps();

    set_timing_delay();
}

Tracker::~Tracker()
{
    for (int b = 0; b < total_blocks; b++)
    {
        for (int c = 0; c < CHANNELS; c++)
        {
            if (block_buffer.length > 0 || block_buffer.channel[c] != NULL)
            {
                free(block_buffer.channel[c]);
            }
        }
    }
    if (channel_buffer.length > 0)
    {
        free(channel_buffer.data);
    }
    for (int i = 0; i < MAXSAMPLES; i++)
    {
        if (sample[i].len > 0 || sample[i].data != NULL)
        {
            free(sample[i].data);
        }
    }
    free(sequence);

    for (int c = 0; c < CHANNELS; c++)
    {
        free(scope[c].data);
    }
}

bool Tracker::check_command(int c, const char *command)
{
    if (command[0] == block[b_pos].channel[c][pos].command[0] && command[1] == block[b_pos].channel[c][pos].command[1])
    {
        return true;
    } else {
        return false;
    }
}

int Tracker::hex2dec(char hex)
{
    int v;
    switch (hex) {
        case '1': v = 1; break;
        case '2': v = 2; break;
        case '3': v = 3; break;
        case '4': v = 4; break;
        case '5': v = 5; break;
        case '6': v = 6; break;
        case '7': v = 7; break;
        case '8': v = 8; break;
        case '9': v = 9; break;
        case 'A': v = 10; break;
        case 'B': v = 11; break;
        case 'C': v = 12; break;
        case 'D': v = 13; break;
        case 'E': v = 14; break;
        case 'F': v = 15; break;
        default:
            v = 0;
            break;
    }
    return v;
}

Uint8 Tracker::get_command(int c)
{
    Uint8 type = COM_NONE;
    int cmd = (int)strtol(block[b_pos].channel[c][pos].command, NULL, 16); // convert hex command to decimal

    // convert hex params to decimal
    channel[c].command_param[0] = hex2dec(block[b_pos].channel[c][pos].parameter[0]);
    channel[c].command_param[1] = hex2dec(block[b_pos].channel[c][pos].parameter[1]);
    channel[c].command_param[2] = (int)strtol(block[b_pos].channel[c][pos].parameter, NULL, 16);

    switch (cmd)
    {
        case 0:
            if (channel[c].command_param[0] == 0 && channel[c].command_param[1] == 0)
            {
                type = COM_NONE;
            } else {
                type = COM_ARPEGGIO;
                channel[c].arp_rates[0] = channel[c].slide_pos; // original pitch
                int arp1 = channel[c].slide_pos;
                int arp2 = arp1;
                for (int x = 0; x < channel[c].command_param[0]; x++)
                {
                    arp1 = (double)arp1 * SEMITONE_MULTIPLIER;
                }
                for (int x = 0; x < channel[c].command_param[1]; x++)
                {
                    arp2 = (double)arp2 * SEMITONE_MULTIPLIER;
                }
                channel[c].arp_rates[1] = arp1;
                channel[c].arp_rates[2] = arp2;
                channel[c].arp_toggle = 0;
            }
            break;
        case 12: // 0C
            type = COM_SET_LEVEL;
            channel[c].amplifier = (double)channel[c].command_param[2] / 100.0;
            break;
        case 24: // 18
            type = COM_SET_LEVEL;
            channel[c].amplifier = (((double)channel[c].command_param[2] / 100.0) * (channel[c].amplifier * 100.0)) / 100.0;
            break;
        case 15: // 0F
            type = COM_KILL;
            if (channel[c].command_param[2] == 255)
            {
                channel[c].play = false;
                channel[c].pos_adv = 0;
            }
            break;
        case 1:
            type = COM_PITCH_UP;
            channel[c].slide_speed = channel[c].command_param[2] * PITCH_SLIDE_AMOUNT;
            break;
        case 2:
            type = COM_PITCH_DOWN;
            channel[c].slide_speed = channel[c].command_param[2] * PITCH_SLIDE_AMOUNT;
            break;
        case 9:
            type = COM_OFFSET;
            channel[c].pos = (sample[channel[c].sample].len / 255) * channel[c].command_param[2];
            break;
        case 25: // 19
            type = COM_OFFSET;
            channel[c].pos = channel[c].command_param[2] * 256;
            break;
        case 3:
            type = COM_SLIDE;
            if (channel[c].command_param[2] != 0)
            {
                channel[c].slide_speed = channel[c].command_param[2] * PITCH_SLIDE_AMOUNT;
            }
            break;
        case 4:
            type = COM_VIBRATO;
            channel[c].vib_speed = channel[c].command_param[0] * (400 * channel[c].octave);
            channel[c].vib_high = channel[c].slide_pos; // set to original pitch
            channel[c].vib_low = channel[c].slide_pos; // set to original pitch
            for (int x = 0; x < channel[c].command_param[1]; x++)
            {
                channel[c].vib_high = (double)channel[c].vib_high * HALF_SEMITONE_MULTIPLIER;
                channel[c].vib_low = (double)channel[c].vib_low / HALF_SEMITONE_MULTIPLIER;
            }
            break;
        case 13: // 0D
            type = COM_LEVEL_FADE;
            break;
        case 10: // 0A
            type = COM_REVERSE;
            channel[c].reverse = true;
            if (channel[c].command_param[2] > 0)
            {
                channel[c].pos = (sample[channel[c].sample].len / 255) * channel[c].command_param[2];
            }
            break;
        case 7:
            type = COM_TREMOLO;
            channel[c].trem_speed = channel[c].command_param[0];
            channel[c].trem_depth = 100 - ((double)channel[c].command_param[1] * 6.6);
            break;
        case 8:
            type = COM_HOLD_AND_DECAY;
            channel[c].hold = channel[c].command_param[1] * 4;
            channel[c].decay = channel[c].command_param[0];
            channel[c].hold_and_decay = true;
            break;
        case 11: // 0B
            type = COM_NONE;
            sq_pos = channel[c].command_param[2];
            if (sq_pos > sequence_len - 1)
            {
                sq_pos = sequence_len - 1;
            }
            b_pos = sequence[sq_pos];
            pos = 0;
            sequence_update = true;
            block_update = true;
            break;
        case 5:
            type = COM_DELAY;
            channel[c].retriggers = channel[c].command_param[0];
            channel[c].retrig_freq = channel[c].command_param[1];
            channel[c].retrig_count = 0;
            channel[c].total_triggers = channel[c].retriggers;
            channel[c].trigger_pos = (int)channel[c].pos;
            break;
        case 6:
            type = COM_RETRIG;
            channel[c].retrig_freq = 0;
            channel[c].retriggers = channel[c].command_param[2] + 1;
            if (channel[c].retriggers > 8)
            {
                channel[c].retriggers = 8;
            }
            channel[c].retrig_count = 0;
            channel[c].total_triggers = timing_delay / channel[c].retriggers;
            channel[c].trigger_pos = (int)channel[c].pos;
            break;
        default:
            break;
    }

    return type;
}

void Tracker::note_trigger()
{
    for (int c = 0; c < CHANNELS; c++)
    {
        if (block[b_pos].channel[c][pos].note != '-')
        {
            if (check_command(c, "03"))
            {
                channel[c].slide_target = getFreq(block[b_pos].channel[c][pos].note,
                block[b_pos].channel[c][pos].key,
                block[b_pos].channel[c][pos].octave,
                block[b_pos].channel[c][pos].sample);
            } else {
                channel[c].pos = 0;
                channel[c].sample = block[b_pos].channel[c][pos].sample;
                channel[c].pos_adv = (double)getFreq(block[b_pos].channel[c][pos].note,
                block[b_pos].channel[c][pos].key,
                block[b_pos].channel[c][pos].octave,
                block[b_pos].channel[c][pos].sample) / (double)SAMPLE_RATE;
                channel[c].amplifier = (double)sample[channel[c].sample].level / 100.0;
                channel[c].play = true;
                // note: slide_pos can be referenced as the original note's sample rate if the 03 command is not in use
                channel[c].slide_pos = getFreq(block[b_pos].channel[c][pos].note,
                block[b_pos].channel[c][pos].key,
                block[b_pos].channel[c][pos].octave,
                block[b_pos].channel[c][pos].sample);
                channel[c].vib_pos = channel[c].slide_pos;
                channel[c].octave = block[b_pos].channel[c][pos].octave;
                channel[c].reverse = false;
                channel[c].trem_start = sample[channel[c].sample].level;
                channel[c].trem_current = channel[c].trem_start;
                channel[c].trem_up = false;
                channel[c].hold_and_decay = false;
                channel[c].retriggers = 0;
            }
        }
        channel[c].command_type = get_command(c);
    }
}

void Tracker::set_timing_delay()
{
    double calc;
    calc = (double)master_tempo * (double)block[b_pos].speed;
    calc = calc / 60.0;
    calc = (double)SAMPLE_RATE / calc;
    timing_delay = (int)calc;
}

void Tracker::set_trigger_bar(int channel, int level)
{
    level = level / 100;
    if (level > 300){level = 300;}
    else if (level < 1){level = 1;}
    if (level > trigger_bars[channel].h)
    {
        trigger_bars[channel].h = level;
    }
}

void Tracker::dec_trigger_bars()
{
    for (int bar = 0; bar < CHANNELS; bar++)
    {
        if (trigger_bars[bar].h > 20)
        {
            trigger_bars[bar].h -= 20;
        } else {
            trigger_bars[bar].h = 1;
        }
    }
}

void Tracker::low_pass(int sample_slot, int cutoff, int sample_rate)
{
    long double x = tanf(M_PI * cutoff / sample_rate);
    long double output = 0;
    Sint16 out;
    Sint16 input;
    for (int p = 0; p < sample[sample_slot].len; p++)
    {
        input = sample[sample_slot].data[p];
        output = x * input + x * output - (x-1) * output;
        output /= (x+1);
        out = (Sint16)output;
        sample[sample_slot].data[p] = out;
    }
}

void Tracker::resample(int sample_slot, int old_rate, int new_rate, bool filter)
{
    if (new_rate < 2000 || new_rate > 96000)
    {
        return;
    }
    Sint16 *buffer = (Sint16*)malloc(sample[sample_slot].len*sizeof(Sint16));
    memset(buffer, 0, sample[sample_slot].len*sizeof(Sint16));
    if (filter)
    {
        low_pass(sample_slot, new_rate/2, old_rate);
    } else {
        low_pass(sample_slot, new_rate/1.2, old_rate);
    }
    double pos_adv = (double)old_rate / (double)new_rate;
    double pos = 0;
    int actual_pos = 0;
    int output_pos = 0;
    Sint32 mix = 0;
    int mix_div = 0;
    while (actual_pos < sample[sample_slot].len)
    {
        mix_div = 0;
        mix = 0;
        for (int scan_p = actual_pos; scan_p <= (int)(pos+pos_adv); scan_p++)
        {
            if (scan_p < sample[sample_slot].len)
            {
                mix += sample[sample_slot].data[scan_p];
                mix_div++;
            }
        }
        mix /= mix_div;
        if (mix > SAMPLE_PEAK)
        {
            mix = SAMPLE_PEAK;
        } else if (mix < 0 - SAMPLE_PEAK)
        {
            mix = 0 - SAMPLE_PEAK;
        }
        buffer[output_pos] = (Sint16)mix;
        output_pos++;
        pos += pos_adv;
        actual_pos = (int)pos;
    }
    sample[sample_slot].data = (Sint16*)realloc(sample[sample_slot].data, (output_pos+1)*sizeof(Sint16));
    sample[sample_slot].len = output_pos;
    memcpy(sample[sample_slot].data, buffer, output_pos*sizeof(Sint16));
    sample[sample_slot].sample_rate = new_rate;
    free(buffer);
}

bool Tracker::load_inst(string path, string name, int sample_slot, bool filter)
{
    SDL_AudioSpec inputSpec;
    Uint8 *data;
    Uint32 length;
    if (SDL_LoadWAV(path.c_str(), &inputSpec, &data, &length) == NULL)
    {
        fprintf(stderr, "Could not open wav: %s\n", SDL_GetError());
        return false;
    }

    int is_signed = inputSpec.format & (1 << 15);
    int big_endian = inputSpec.format & (1 << 12);
    int floating_point = inputSpec.format & (1 << 8);
    int bit_depth = inputSpec.format & 0x7F;
    string message = name + ": ";
    message += to_string(bit_depth);
    message += "bit";
    if (is_signed != 0)
    {
        message += "/Signed";
    } else {
        message += "/Unsigned";
    }
    if (big_endian != 0)
    {
        message += "/BigEndian";
    } else {
        message += "/LittleEndian";
    }
    if (floating_point != 0)
    {
        message += "/Float";
    } else {
        message += "/PCM";
    }
    printf("%s\n", message.c_str());
    message.clear();

    // convert input format
    if (inputSpec.format == AUDIO_S16)
    {
        SDL_LockAudio();
        if (sample[sample_slot].len != 0)
        {
            free(sample[sample_slot].data);
        }
        int new_length = length / 2;
        Sint16 val, val2;
        Sint32 mix;
        sample[sample_slot].data = (Sint16*)malloc(new_length*sizeof(Sint16));
        memset(sample[sample_slot].data, 0, new_length);
        sample[sample_slot].len = new_length;
        sample[sample_slot].tune = 0;
        sample[sample_slot].fine_tune = 0;
        sample[sample_slot].level = 100;
        sample[sample_slot].name = name;
        sample[sample_slot].sample_rate = getFreq(default_pitch.note, default_pitch.key, default_pitch.octave, -1);
        if (inputSpec.channels == 1)
        {
            for (int x = 0, p = 0; x < length; x += 2, p++)
            {
                val = ((data[x+1] & 0xFF) << 8) | (data[x] & 0xFF);
                sample[sample_slot].data[p] = val / BIT_REDUCT;
            }
        } else {
            for (int x = 0, p = 0; x < length; x += 4, p++)
            {
                val = ((data[x+1] & 0xFF) << 8) | (data[x] & 0xFF);
                val2 = ((data[x+3] & 0xFF) << 8) | (data[x+2] & 0xFF);
                mix = (val + val2) / 2;
                val = (Sint16)mix;
                sample[sample_slot].data[p] = val / BIT_REDUCT;
            }
            sample[sample_slot].len /= 2;
        }
        SDL_UnlockAudio();
        SDL_FreeWAV(data);
    } else if (inputSpec.format == AUDIO_U8)
    {
        SDL_LockAudio();
        if (sample[sample_slot].len != 0)
        {
            free(sample[sample_slot].data);
        }
        Uint8 val_8, val2_8;
        Sint16 temp;
        Sint32 mix_8;
        sample[sample_slot].data = (Sint16*)malloc(length*sizeof(Sint16));
        memset(sample[sample_slot].data, 0, length);
        sample[sample_slot].len = length;
        sample[sample_slot].tune = 0;
        sample[sample_slot].fine_tune = 0;
        sample[sample_slot].level = 100;
        sample[sample_slot].name = name;
        sample[sample_slot].sample_rate = getFreq(default_pitch.note, default_pitch.key, default_pitch.octave, -1);
        if (inputSpec.channels == 1)
        {
            for (int x = 0, p = 0; x < length; x++, p++)
            {
                val_8 = data[x];
                temp = (val_8 - 128) * (AUDIO_PEAK / 255);
                sample[sample_slot].data[p] = temp / BIT_REDUCT;
            }
        } else {
            for (int x = 0, p = 0; x < length; x += 2, p++)
            {
                val_8 = data[x];
                val2_8 = data[x+1];
                temp = (val_8 - 128) * (AUDIO_PEAK / 255);
                mix_8 = temp + ((val2_8 - 128) * (AUDIO_PEAK / 255));
                mix_8 /= 2;
                sample[sample_slot].data[p] = mix_8 / BIT_REDUCT;
            }
            sample[sample_slot].len /= 2;
        }
        SDL_UnlockAudio();
        SDL_FreeWAV(data);
    } else {
        printf("Invalid Audio Format!\n");
        SDL_FreeWAV(data);
        return false;
    }
    if (inputSpec.freq > sample[sample_slot].sample_rate)
    {
        resample(sample_slot, inputSpec.freq, sample[sample_slot].sample_rate, filter);
    } else {
        printf("Note: Desired pitch rate exceeds the original file's sample rate. No downsampling has been applied.\n");
        sample[sample_slot].sample_rate = inputSpec.freq;
    }
    update_info();
    return true;
}

void Tracker::copy_note(int buff)
{
    note_buffer[buff].note = block[b_pos].channel[cursor_channel][pos].note;
    note_buffer[buff].key = block[b_pos].channel[cursor_channel][pos].key;
    note_buffer[buff].octave = block[b_pos].channel[cursor_channel][pos].octave;
    note_buffer[buff].sample = block[b_pos].channel[cursor_channel][pos].sample;
    note_buffer[buff].command[0] = block[b_pos].channel[cursor_channel][pos].command[0];
    note_buffer[buff].command[1] = block[b_pos].channel[cursor_channel][pos].command[1];
    note_buffer[buff].parameter[0] = block[b_pos].channel[cursor_channel][pos].parameter[0];
    note_buffer[buff].parameter[1] = block[b_pos].channel[cursor_channel][pos].parameter[1];
}

void Tracker::paste_note(int buff)
{
    if (edit_mode)
    {
        block[b_pos].channel[cursor_channel][pos].note = note_buffer[buff].note;
        block[b_pos].channel[cursor_channel][pos].key = note_buffer[buff].key;
        block[b_pos].channel[cursor_channel][pos].octave = note_buffer[buff].octave;
        block[b_pos].channel[cursor_channel][pos].sample = note_buffer[buff].sample;
        block[b_pos].channel[cursor_channel][pos].command[0] = note_buffer[buff].command[0];
        block[b_pos].channel[cursor_channel][pos].command[1] = note_buffer[buff].command[1];
        block[b_pos].channel[cursor_channel][pos].parameter[0] = note_buffer[buff].parameter[0];
        block[b_pos].channel[cursor_channel][pos].parameter[1] = note_buffer[buff].parameter[1];
        incpos();
    }
}

void Tracker::copy_channel()
{
    if (channel_buffer.length == 0)
    {
        channel_buffer.data = (Note*)malloc(block[b_pos].length*sizeof(Note));
    } else if (channel_buffer.length < block[b_pos].length)
    {
        channel_buffer.data = (Note*)realloc(channel_buffer.data, block[b_pos].length*sizeof(Note));
    }
    channel_buffer.length = block[b_pos].length;
    for (int s = 0; s < channel_buffer.length; s++)
    {
        channel_buffer.data[s].note = block[b_pos].channel[cursor_channel][s].note;
        channel_buffer.data[s].key = block[b_pos].channel[cursor_channel][s].key;
        channel_buffer.data[s].octave = block[b_pos].channel[cursor_channel][s].octave;
        channel_buffer.data[s].sample = block[b_pos].channel[cursor_channel][s].sample;
        channel_buffer.data[s].command[0] = block[b_pos].channel[cursor_channel][s].command[0];
        channel_buffer.data[s].command[1] = block[b_pos].channel[cursor_channel][s].command[1];
        channel_buffer.data[s].parameter[0] = block[b_pos].channel[cursor_channel][s].parameter[0];
        channel_buffer.data[s].parameter[1] = block[b_pos].channel[cursor_channel][s].parameter[1];
    }
}

void Tracker::paste_channel()
{
    int length;
    if (channel_buffer.length > block[b_pos].length)
    {
        length = block[b_pos].length;
    } else {
        length = channel_buffer.length;
    }
    for (int s = 0; s < length; s++)
    {
        block[b_pos].channel[cursor_channel][s].note = channel_buffer.data[s].note;
        block[b_pos].channel[cursor_channel][s].key = channel_buffer.data[s].key;
        block[b_pos].channel[cursor_channel][s].octave = channel_buffer.data[s].octave;
        block[b_pos].channel[cursor_channel][s].sample = channel_buffer.data[s].sample;
        block[b_pos].channel[cursor_channel][s].command[0] = channel_buffer.data[s].command[0];
        block[b_pos].channel[cursor_channel][s].command[1] = channel_buffer.data[s].command[1];
        block[b_pos].channel[cursor_channel][s].parameter[0] = channel_buffer.data[s].parameter[0];
        block[b_pos].channel[cursor_channel][s].parameter[1] = channel_buffer.data[s].parameter[1];
    }
}

void Tracker::sequencer(Uint8 flag)
{
    switch (flag)
    {
        case APPEND_SEQ:
            sequence_len++;
            if (sequence_len >= sequence_size)
            {
                sequence_size += seq_size_chunk;
                sequence = (Uint8*)realloc(sequence, sequence_size*sizeof(Uint8));
            }
            sequence[sequence_len-1] = b_pos;
            break;
        case INSERT_SEQ:
            sequence_len++;
            if (sequence_len >= sequence_size)
            {
                sequence_size += seq_size_chunk;
                sequence = (Uint8*)realloc(sequence, sequence_size*sizeof(Uint8));
            }
            for (int i = sequence_len-1; i > sq_pos; i--)
            {
                sequence[i] = sequence[i-1];
            }
            sequence[sq_pos] = b_pos;
            break;
        case DELETE_SEQ:
            if (sequence_len > 1)
            {
                for (int i = sq_pos; i < sequence_len-1; i++)
                {
                    sequence[i] = sequence[i+1];
                }
                sequence_len--;
            } else {
                sequence[0] = 0;
            }
            break;
        case INC_SEQ:
            if (sequence[sq_pos] < total_blocks-1)
            {
                sequence[sq_pos]++;
            } else {
                sequence[sq_pos] = 0;
            }
            break;
        case DEC_SEQ:
            if (sequence[sq_pos] > 0)
            {
                sequence[sq_pos]--;
            } else {
                sequence[sq_pos] = total_blocks - 1;
            }
            break;
        default: break;
    }
}

void Tracker::realloc_block(int size)
{
    if (block[b_pos].length < size)
    {
        for (int c = 0; c < CHANNELS; c++)
        {
            block[b_pos].channel[c] = (Note*)realloc(block[b_pos].channel[c], size*sizeof(Note));
            for (int s = block[b_pos].length; s < size; s++)
            {
                block[b_pos].channel[c][s].note = '-';
                block[b_pos].channel[c][s].key = '-';
                block[b_pos].channel[c][s].octave = 0;
                block[b_pos].channel[c][s].sample = 0;
                block[b_pos].channel[c][s].command[0] = '0';
                block[b_pos].channel[c][s].command[1] = '0';
                block[b_pos].channel[c][s].command[2] = '\0';
                block[b_pos].channel[c][s].parameter[0] = '0';
                block[b_pos].channel[c][s].parameter[1] = '0';
                block[b_pos].channel[c][s].parameter[2] = '\0';
            }
        }
    }
    block[b_pos].length = size;
    if (pos >= size)
    {
        pos = size - 1;
    }
}

void Tracker::allocate_block(int blk, Uint16 size)
{
    for (int c = 0; c < CHANNELS; c++)
    {
        if (block[blk].channel[c] != NULL)
        {
            free(block[blk].channel[c]);
        }
        block[blk].channel[c] = (Note*)malloc(sizeof(Note)*size);
    }
    block[blk].length = size;
}

void Tracker::copy_block(int blk)
{
    if (block_buffer.length == 0)
    {
        for (int c = 0; c < CHANNELS; c++)
        {
            block_buffer.channel[c] = (Note*)malloc(block[blk].length*sizeof(Note));
        }
    } else if (block_buffer.length < block[blk].length)
    {
        for (int c = 0; c < CHANNELS; c++)
        {
            block_buffer.channel[c] = (Note*)realloc(block_buffer.channel[c], block[blk].length*sizeof(Note));
        }
    }
    block_buffer.length = block[blk].length;
    block_buffer.speed = block[blk].speed;
    block_buffer.name = block[blk].name;
    for (int c = 0; c < CHANNELS; c++)
    {
        for (int s = 0; s < block[blk].length; s++)
        {
            block_buffer.channel[c][s].note = block[blk].channel[c][s].note;
            block_buffer.channel[c][s].key = block[blk].channel[c][s].key;
            block_buffer.channel[c][s].octave = block[blk].channel[c][s].octave;
            block_buffer.channel[c][s].sample = block[blk].channel[c][s].sample;
            block_buffer.channel[c][s].command[0] = block[blk].channel[c][s].command[0];
            block_buffer.channel[c][s].command[1] = block[blk].channel[c][s].command[1];
            block_buffer.channel[c][s].parameter[0] = block[blk].channel[c][s].parameter[0];
            block_buffer.channel[c][s].parameter[1] = block[blk].channel[c][s].parameter[1];
        }
    }
}

void Tracker::paste_block(int blk)
{
    if (block[blk].length == 0)
    {
        for (int c = 0; c < CHANNELS; c++)
        {
            block[blk].channel[c] = (Note*)malloc(block_buffer.length*sizeof(Note));
        }
    } else if (block[blk].length < block_buffer.length)
    {
        for (int c = 0; c < CHANNELS; c++)
        {
            block[blk].channel[c] = (Note*)realloc(block[blk].channel[c], block_buffer.length*sizeof(Note));
        }
    }
    block[blk].length = block_buffer.length;
    block[blk].speed = block_buffer.speed;
    block[blk].name = block_buffer.name;
    for (int c = 0; c < CHANNELS; c++)
    {
        for (int s = 0; s < block_buffer.length; s++)
        {
            block[blk].channel[c][s].note = block_buffer.channel[c][s].note;
            block[blk].channel[c][s].key = block_buffer.channel[c][s].key;
            block[blk].channel[c][s].octave = block_buffer.channel[c][s].octave;
            block[blk].channel[c][s].sample = block_buffer.channel[c][s].sample;
            block[blk].channel[c][s].command[0] = block_buffer.channel[c][s].command[0];
            block[blk].channel[c][s].command[1] = block_buffer.channel[c][s].command[1];
            block[blk].channel[c][s].parameter[0] = block_buffer.channel[c][s].parameter[0];
            block[blk].channel[c][s].parameter[1] = block_buffer.channel[c][s].parameter[1];
        }
    }
}

void Tracker::create_block(bool insert)
{
    block[total_blocks].length = block[total_blocks-1].length;
    block[total_blocks].speed = block[total_blocks-1].speed;
    for (int c = 0; c < CHANNELS; c++)
    {
        if (block[total_blocks].channel[c] != NULL)
        {
            free(block[total_blocks].channel[c]);
            block[total_blocks].channel[c] = NULL;
        }
        block[total_blocks].channel[c] = (Note*)malloc(block[total_blocks].length*sizeof(Note));
    }
    clear_block(total_blocks);
    if (insert) // copy all blocks ahead of b_pos down and clear current block
    {
        for (int s = 0; s < sequence_len; s++)
        {
            if (sequence[s] >= b_pos)
            {
                sequence[s]++;
            }
        }
        sequence_update = true;
        for (int b = total_blocks; b > b_pos; b--)
        {
            copy_block(b-1);
            paste_block(b);
        }
        clear_block(b_pos);
    }
    total_blocks++;
    block_update = true;
}

void Tracker::delete_block(int blk)
{
    clear_block(blk);
    if (total_blocks == 1)
    {
        return;
    }
    for (int b = blk; b < total_blocks; b++)
    {
        copy_block(b+1);
        paste_block(b);
    }
    for (int s = 0; s < sequence_len; s++)
    {
        if (sequence[s] >= blk)
        {
            sequence[s]--;
        }
    }
    total_blocks--;
    if (b_pos >= total_blocks)
    {
        b_pos = total_blocks - 1;
    }
    sequence_update = true;
    block_update = true;
}

void Tracker::update_info()
{
    string text;
    text = block[b_pos].name;
    int length = strlen(text.c_str());
    if ( length < 10)
    {
        for (int a = 0; a < 10 - length; a++)
        {
            text += " ";
        }
    }
    surf = TTF_RenderText_Solid(font, text.c_str(), p->black);
    SDL_DestroyTexture(blkname_displaytex);
    blkname_displaytex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    text.clear();
    text += "SEQ:";
    if (sq_pos < 100){text += "0";}
    if (sq_pos < 10){text += "0";}
    text += to_string(sq_pos);
    surf = TTF_RenderText_Solid(font, text.c_str(), p->black);
    SDL_DestroyTexture(sequence_display_tex);
    sequence_display_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    text.clear();
    text += "BLK:";
    if (b_pos < 100){text += "0";}
    if (b_pos < 10){text += "0";}
    text += to_string(b_pos);
    surf = TTF_RenderText_Solid(font, text.c_str(), p->black);
    SDL_DestroyTexture(block_display_tex);
    block_display_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    text.clear();
    text += "INST:";
    if (s_pos < 10){text += "0";}
    text += to_string(s_pos);
    surf = TTF_RenderText_Solid(font, text.c_str(), p->black);
    SDL_DestroyTexture(sample_display_tex);
    sample_display_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    text.clear();
    text = sample[s_pos].name;
    length = strlen(text.c_str());
    if (length < 15)
    {
        for (int a = 0; a < 15 - length; a++)
        {
            text += " ";
        }
    }
    surf = TTF_RenderText_Solid(font, text.c_str(), p->black);
    SDL_DestroyTexture(sample_name_tex);
    sample_name_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    text.clear();
    text += "OCTAVE:";
    text += to_string(octave);
    surf = TTF_RenderText_Solid(font, text.c_str(), p->black);
    SDL_DestroyTexture(octave_display_tex);
    octave_display_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    text.clear();
    text = "SKIP:";
    text += to_string(skip);
    length = strlen(text.c_str());
    if (length < 7)
    {
        for (int a = 0; a < 7 - length; a++)
        {
            text += " ";
        }
    }
    surf = TTF_RenderText_Solid(font, text.c_str(), p->black);
    SDL_DestroyTexture(skip_display_tex);
    skip_display_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    text.clear();
    text = "Highlight:";
    text += to_string(highlight_iteration);
    length = text.length();
    if (length < 12)
    {
        for (int a = 0; a < 12 - length; a++)
        {
            text += " ";
        }
    }
    surf = TTF_RenderText_Solid(font, text.c_str(), p->black);
    if (block_highlight.t != NULL)
    {
        SDL_DestroyTexture(block_highlight.t);
    }
    block_highlight.t = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
}

void Tracker::move_cursor(int position, int chn, int direction)
{
    // set cursor_pos and cursor_channel
    if (position == 1)
    {
        if (direction == 1)
        {
            if (cursor_pos < 6)
            {cursor_pos++;}
            else
            {cursor_pos = 0;
            if (cursor_channel < CHANNELS-1){cursor_channel++;}else{cursor_channel = 0;}
            }
        } else {
            if (cursor_pos > 0)
            {cursor_pos--;}
            else
            {cursor_pos = 6;
            if (cursor_channel > 0){cursor_channel--;}else{cursor_channel = CHANNELS-1;}
            }
        }
    } else if (chn > 0)
    {
        if (direction == 1)
        {
            if (cursor_channel < CHANNELS-1){cursor_channel++;}else{cursor_channel = 0;}
        } else {
            if (cursor_channel > 0){cursor_channel--;}else{cursor_channel = CHANNELS-1;}
        }
    }

    // set cursor rect positions
    // Note each symbol is 13x17 WxH
    int width;
    switch (cursor_channel) {
        case 0: width = 36; break;
        case 1: width = 192; break;
        case 2: width = 348; break;
        case 3: width = 504; break;
        case 4: width = 660; break;
        case 5: width = 816; break;
        case 6: width = 972; break;
        case 7: width = 1128; break;
        default: width = 0; break;
    }
    width += 13;
    switch (cursor_pos) {
        case 0: cursor.x = width; cursor.w = 39; break;
        case 1: cursor.x = 39 + width; cursor.w = 13; break;
        case 2: cursor.x = 52 + width; cursor.w = 13; break;
        case 3: cursor.x = 65 + width; cursor.w = 13; break;
        case 4: cursor.x = 78 + width; cursor.w = 13; break;
        case 5: cursor.x = 91 + width; cursor.w = 13; break;
        case 6: cursor.x = 104 + width; cursor.w = 13; break;
        default: break;
    }
}

void Tracker::block_dec()
{
    if ((b_pos - 1) >= 0)
    {
        b_pos--;
    } else {
        b_pos = total_blocks - 1;
    }
    pos = 0;
    set_timing_delay();
    update_info();
}

void Tracker::block_inc()
{
    if ((b_pos + 1) < total_blocks)
    {
        b_pos++;
    } else {
        b_pos = 0;
    }
    pos = 0;
    set_timing_delay();
    update_info();
}

void Tracker::sample_inc()
{
    if (s_pos + 1 < MAXSAMPLES)
    {
        s_pos++;
    } else {
        s_pos = 0;
    }
    update_info();
}

void Tracker::sample_dec()
{
    if (s_pos - 1 >= 0)
    {
        s_pos--;
    } else {
        s_pos = MAXSAMPLES - 1;
    }
    update_info();
}

void Tracker::incpos()
{
    if (skip == 1)
    {
        if (pos < block[b_pos].length - 1)
        {
            pos++;
        } else {
            pos = 0;
        }
    } else if (!round_skip)
    {
        if ((pos + skip) < block[b_pos].length)
        {
            pos += skip;
        } else {
            pos = skip - (block[b_pos].length - pos);
        }
    } else {
        pos = (((pos / skip)+1) * skip);
        if (pos >= block[b_pos].length - 1)
        {
            pos = 0;
        }
        // johnny code
    }
}

void Tracker::move_step() // used by timer to run tracker
{
    if (tracker_running)
    {
        if (pos < block[b_pos].length - 1)
        {
            pos++;
        } else {
            pos = 0;
            if (run_sequence) // if true step to next block in the sequence else loops
            {
                if (sq_pos < sequence_len - 1)
                {
                    sq_pos++;
                } else {
                    sq_pos = 0;
                    sequence_end = true;
                }
                b_pos = sequence[sq_pos];
                sequence_update = true;
                update_info();
                set_timing_delay();
            }
        }
        note_trigger();
    }
}

void Tracker::decpos()
{
    if (skip == 1)
    {
        if (pos > 0)
        {
            pos--;
        } else {
            pos = block[b_pos].length - 1;
        }
    } else if (!round_skip)
    {
        if ((pos - skip) >= 0)
        {
            pos -= skip;
        } else {
            pos = block[b_pos].length - (skip - pos);
        }
    } else {
        if (pos > skip && pos < (skip*2))
            {
              pos = skip;
        } else {
              pos = (((pos / skip)-1) * skip);
              if (pos < 0)
              {
                  pos = block[b_pos].length - skip;
              }
          }
        // johnny code
    }
}

void Tracker::clear_channel()
{
    for (int s = 0; s < block[b_pos].length; s++)
    {
        block[b_pos].channel[cursor_channel][s].note = '-';
        block[b_pos].channel[cursor_channel][s].key = '-';
        block[b_pos].channel[cursor_channel][s].octave = 0;
        block[b_pos].channel[cursor_channel][s].sample = 0;
        block[b_pos].channel[cursor_channel][s].command[0] = '0';
        block[b_pos].channel[cursor_channel][s].command[1] = '0';
        block[b_pos].channel[cursor_channel][s].command[2] = '\0';
        block[b_pos].channel[cursor_channel][s].parameter[0] = '0';
        block[b_pos].channel[cursor_channel][s].parameter[1] = '0';
        block[b_pos].channel[cursor_channel][s].parameter[2] = '\0';
    }
}

void Tracker::clear_block(int blk) // Clears indicated block
{
    block[blk].name.clear();
    for (int c = 0; c < CHANNELS; c++)
    {
        for (int s = 0; s < block[blk].length; s++)
        {
            block[blk].channel[c][s].note = '-';
            block[blk].channel[c][s].key = '-';
            block[blk].channel[c][s].octave = 0;
            block[blk].channel[c][s].sample = 0;
            block[blk].channel[c][s].command[0] = '0';
            block[blk].channel[c][s].command[1] = '0';
            block[blk].channel[c][s].command[2] = '\0';
            block[blk].channel[c][s].parameter[0] = '0';
            block[blk].channel[c][s].parameter[1] = '0';
            block[blk].channel[c][s].parameter[2] = '\0';
        }
    }
}

void Tracker::render_info()
{
    SDL_RenderCopy(renderer, blkname_displaytex, NULL, &blkname_displayrect);
    SDL_RenderDrawRect(renderer, &tracker_box);
    SDL_RenderCopy(renderer, sequence_display_tex, NULL, &sequence_display);
    SDL_RenderCopy(renderer, block_display_tex, NULL, &block_display);
    SDL_RenderCopy(renderer, sample_display_tex, NULL, &sample_display);
    SDL_RenderCopy(renderer, octave_display_tex, NULL, &octave_display);
    SDL_RenderCopy(renderer, sample_name_tex, NULL, &sample_name);
    SDL_RenderCopy(renderer, skip_display_tex, NULL, &skip_display);
    if (round_skip)
    {
        SDL_SetRenderDrawColor(renderer, 128, 0, 0, 0xFF); // Red
        SDL_RenderDrawRect(renderer, &skip_display);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF); // Black
    }
    SDL_RenderCopy(renderer, block_highlight.t, NULL, &block_highlight.r);
    SDL_SetRenderDrawColor(renderer, 128, 0, 0, 0xFF); // Red
    SDL_RenderDrawRect(renderer, &cursor);
}

void Tracker::update_steps()
{
    string step_data;
    int step_pos = 0;
    for (int step = 0; step < DISPLAYRECTS; step++)
    {
        step_pos = pos + (step - (DISPLAYRECTS / 2)); // calculates position offset
        step_data.clear();
        if (step_pos < block[b_pos].length && step_pos >= 0) // Checks if position is in bounds of the block
        {
            // combines all elements into one string
            if (step_pos < 100){step_data += '>';}
            if (step_pos < 10){step_data += '0';}
            step_data += to_string(step_pos);
            for (int chan = 0; chan < CHANNELS; chan++)
            {
                step_data += block[b_pos].channel[chan][step_pos].note;
                step_data += block[b_pos].channel[chan][step_pos].key;
                if (block[b_pos].channel[chan][step_pos].octave == 0){step_data += '-';} else
                {step_data += to_string(block[b_pos].channel[chan][step_pos].octave);}
                if (block[b_pos].channel[chan][step_pos].sample < 10) {step_data += '0';}
                step_data += to_string(block[b_pos].channel[chan][step_pos].sample);
                step_data += block[b_pos].channel[chan][step_pos].command[0];
                step_data += block[b_pos].channel[chan][step_pos].command[1];
                step_data += block[b_pos].channel[chan][step_pos].parameter[0];
                step_data += block[b_pos].channel[chan][step_pos].parameter[1];
                if (chan < CHANNELS-1) {step_data += " | ";}
            }
            step_data += "<";
        } else {
            step_data = " "; // renders empty step if position is out of bounds
        }

        if (highlight_iteration != 0 && (step_pos % highlight_iteration) == 0 && step_data.length() > 1)
        {
            if (edit_mode && step_pos == pos)
            {
                surf = TTF_RenderText_Shaded(font, step_data.c_str(), p->red, p->highlight);
            } else if (step_pos == pos)
            {
                surf = TTF_RenderText_Shaded(font, step_data.c_str(), p->blue, p->highlight);
            } else {
                surf = TTF_RenderText_Shaded(font, step_data.c_str(), p->black, p->highlight);
            }
        } else if (step_pos == pos) // Renders the cursor positon red instead of black
        {
            if (edit_mode)
            {
                surf = TTF_RenderText_Solid(font, step_data.c_str(), p->red); // Makes surface with text
            } else {
                surf = TTF_RenderText_Solid(font, step_data.c_str(), p->blue); // Makes surface with text
            }
        } else {
            surf = TTF_RenderText_Solid(font, step_data.c_str(), p->black);
        }
        SDL_DestroyTexture(displaytextures[step]); // destroys old texture. If not destroyed there will be a memory leak.
        displaytextures[step] = SDL_CreateTextureFromSurface(renderer, surf); // Coverts surface to texture
        SDL_FreeSurface(surf); // frees surface otherwise there will be a memory leak
    }
}

void Tracker::render_bars()
{
    SDL_SetRenderDrawColor(renderer, 0, 128, 0, 0xFF);
    for (int b = 0; b < CHANNELS; b++)
    {
        SDL_RenderFillRect(renderer, &trigger_bars[b]);
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
}

void Tracker::render_steps() // Renders block data to screen
{
    update_steps();
    for (int step = 0; step < DISPLAYRECTS; step++)
    {
        SDL_RenderCopy(renderer, displaytextures[step], NULL, &displayrects[step]); // Renderes texture to screen
    }
}

int Tracker::getFreq(char note, char key, int oct, int sample_id)
{
    int rate;
    switch (note)
    {
        case 'C': if (key == '#') {rate = 4238;} else {rate = 4000;} break;
        case 'D': if (key == '#') {rate = 4757;} else {rate = 4490;} break;
        case 'E': rate = 5040; break;
        case 'F': if (key == '#') {rate = 5658;} else {rate = 5340;} break;
        case 'G': if (key == '#') {rate = 6350;} else {rate = 5994;} break;
        case 'A': if (key == '#') {rate = 7128;} else {rate = 6728;} break;
        case 'B': rate = 7552; break;
        default: rate =  0; break;
    }
    for (int a = 1; a < oct; a++)
    {
        rate = rate * 2;
    }

    if (sample_id == -1)
    {
        return rate;
    }

    if (sample[sample_id].tune > 0)
    {
        for (int x = 0; x < sample[sample_id].tune; x++)
        {
            rate *= SEMITONE_MULTIPLIER;
        }
    } else if (sample[sample_id].tune < 0) {
        for (int x = 0; x > sample[sample_id].tune; x--)
        {
            rate /= SEMITONE_MULTIPLIER;
        }
    }

    if (sample[sample_id].fine_tune > 0)
    {
        for (int x = 0; x < sample[sample_id].fine_tune; x++)
        {
            rate *= FINETUNE_MULTIPLIER;
        }
    } else if (sample[sample_id].fine_tune < 0) {
        for (int x = 0; x > sample[sample_id].fine_tune; x--)
        {
            rate /= FINETUNE_MULTIPLIER;
        }
    }

    return rate;
}

void Tracker::get_note(SDL_Event *e)
{
    if (!edit_mode)
    {
        return;
    }
    if (cursor_pos == 0)
    {
        // figure out note data
        char note = '-';
        char key = '-';
        int oct = octave;
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

        if (oct != 0)
        {
            // add note
            block[b_pos].channel[cursor_channel][pos].note = note;
            block[b_pos].channel[cursor_channel][pos].key = key;
            block[b_pos].channel[cursor_channel][pos].octave = oct;
            block[b_pos].channel[cursor_channel][pos].sample = s_pos;
            if (!tracker_running)
            {incpos();}
        }
    } else
    {
        char k; int i = 11;
        bool vkey = true;
        switch (e->key.keysym.sym) {
            case SDLK_a: k = 'A'; break;
            case SDLK_b: k = 'B'; break;
            case SDLK_c: k = 'C'; break;
            case SDLK_d: k = 'D'; break;
            case SDLK_e: k = 'E'; break;
            case SDLK_f: k = 'F'; break;
            case SDLK_0: k = '0'; i = 0; break;
            case SDLK_1: k = '1'; i = 1; break;
            case SDLK_2: k = '2'; i = 2; break;
            case SDLK_3: k = '3'; i = 3; break;
            case SDLK_4: k = '4'; i = 4; break;
            case SDLK_5: k = '5'; i = 5; break;
            case SDLK_6: k = '6'; i = 6; break;
            case SDLK_7: k = '7'; i = 7; break;
            case SDLK_8: k = '8'; i = 8; break;
            case SDLK_9: k = '9'; i = 9; break;
            default:
                vkey = false;
                break;
        }
        if (vkey)
        {
            if (cursor_pos == 1 && i != 11)
            {
                block[b_pos].channel[cursor_channel][pos].sample = i * 10; incpos();
            } else if (cursor_pos == 2 && i != 11)
            {
                if (block[b_pos].channel[cursor_channel][pos].sample >= 10){
                    block[b_pos].channel[cursor_channel][pos].sample = block[b_pos].channel[cursor_channel][pos].sample / 10;
                    block[b_pos].channel[cursor_channel][pos].sample = (block[b_pos].channel[cursor_channel][pos].sample * 10) + i;}
                else {
                    block[b_pos].channel[cursor_channel][pos].sample = i;
                }
                if (!tracker_running)
                {incpos();}
            } else if (cursor_pos == 3)
            {
                block[b_pos].channel[cursor_channel][pos].command[0] = k;
                if (!tracker_running)
                {incpos();}
            } else if (cursor_pos == 4)
            {
                block[b_pos].channel[cursor_channel][pos].command[1] = k;
                if (!tracker_running)
                {incpos();}
            } else if (cursor_pos == 5)
            {
                block[b_pos].channel[cursor_channel][pos].parameter[0] = k;
                if (!tracker_running)
                {incpos();}
            } else if (cursor_pos == 6)
            {
                block[b_pos].channel[cursor_channel][pos].parameter[1] = k;
                if (!tracker_running)
                {incpos();}
            }
        }
    }
    update_steps();
}

void Tracker::clear_step()
{
    block[b_pos].channel[cursor_channel][pos].note = '-';
    block[b_pos].channel[cursor_channel][pos].key = '-';
    block[b_pos].channel[cursor_channel][pos].octave = 0;
    block[b_pos].channel[cursor_channel][pos].sample = 0;
    block[b_pos].channel[cursor_channel][pos].command[0] = '0';
    block[b_pos].channel[cursor_channel][pos].command[1] = '0';
    block[b_pos].channel[cursor_channel][pos].parameter[0] = '0';
    block[b_pos].channel[cursor_channel][pos].parameter[1] = '0';
    if (!tracker_running)
    {incpos();}
    update_steps();
}

void Tracker::clear_index()
{
    switch (cursor_pos) {
        case 3:
            block[b_pos].channel[cursor_channel][pos].command[0] = '0';
            if (!tracker_running)
            {incpos();}
            break;
        case 4:
            block[b_pos].channel[cursor_channel][pos].command[1] = '0';
            if (!tracker_running)
            {incpos();}
            break;
        case 5:
            block[b_pos].channel[cursor_channel][pos].parameter[0] = '0';
            if (!tracker_running)
            {incpos();}
            break;
        case 6:
            block[b_pos].channel[cursor_channel][pos].parameter[1] = '0';
            if (!tracker_running)
            {incpos();}
            break;
        default:
            break;
    }
    update_steps();
}

bool Tracker::checkButton(SDL_Rect *button, int mouseX, int mouseY)
{
    if (mouseX > button->x && mouseX < button->w+button->x && mouseY > button->y && mouseY < button->h+button->y)
    {
        return true;
    }
    return false;
}

void Tracker::mouse(int x, int y)
{
    if (checkButton(&skip_display, x, y))
    {
        round_skip = !round_skip;
    }
}

void Tracker::keyboard(SDL_Event *e)
{
    switch (e->key.keysym.sym) // Keyboard input
    {
        case SDLK_BACKQUOTE:
            if (skip == 1)
            {
                skip = skip_mem;
            } else {
                skip_mem = skip;
                skip = 1;
            }
            update_info();
            break;
        case SDLK_UP:
            if (SDL_GetModState() & KMOD_SHIFT)
            {
                block_dec();
            } else {
                decpos();
            }
            break;
        case SDLK_DOWN:
            if (SDL_GetModState() & KMOD_SHIFT)
            {
                block_inc();
            } else {
                incpos();
            }
            break;
        case SDLK_RIGHT:
            if (SDL_GetModState() & KMOD_CTRL && SDL_GetModState() & KMOD_SHIFT)
            {
                if (highlight_iteration < 32)
                {
                    highlight_iteration++;
                } else {
                    highlight_iteration = 0;
                }
                update_info();
                update_steps();
            } else if (SDL_GetModState() & KMOD_SHIFT)
            {
                sample_inc();
            } else if (SDL_GetModState() & KMOD_ALT){
                move_cursor(0, 1, 1);
            } else if (SDL_GetModState() & KMOD_CTRL){
                if (skip < 32){skip++;}else{skip = 1;}
                update_info();
            } else {
                move_cursor(1, 0, 1);
            }
            break;
        case SDLK_LEFT:
            if (SDL_GetModState() & KMOD_CTRL && SDL_GetModState() & KMOD_SHIFT)
            {
                if (highlight_iteration > 0)
                {
                    highlight_iteration--;
                } else {
                    highlight_iteration = 32;
                }
                update_info();
                update_steps();
            } else if (SDL_GetModState() & KMOD_SHIFT)
            {
                sample_dec();
            } else if (SDL_GetModState() & KMOD_ALT){
                move_cursor(0, 1, 0);
            } else if (SDL_GetModState() & KMOD_CTRL){
                if (skip > 1){skip--;}else{skip = 32;}
                update_info();
            } else {
                move_cursor(1, 0, 0);
            }
            break;
        case SDLK_ESCAPE:
            if (edit_mode){edit_mode = false;} else {edit_mode = true;}
            break;
        case SDLK_TAB:
            if (SDL_GetModState() & KMOD_SHIFT)
            {
                move_cursor(0, 1, 0);
            } else {
                move_cursor(0, 1, 1);
            }
            break;
        case SDLK_DELETE:
        case SDLK_BACKSPACE:
            if (edit_mode)
            {
                if (cursor_pos == 0)
                {
                    clear_step();
                } else {
                    clear_index();
                }
            }
            break;

        // keyboard keys. if there's no modstate run get_note(e)
        case SDLK_x:
            if ((SDL_GetModState() & KMOD_CTRL) && (SDL_GetModState() & KMOD_SHIFT))
            {
                clear_block(b_pos);
                pos = 0;
                break;
            } else if (SDL_GetModState() & KMOD_CTRL)
            {
                clear_channel();
                break;
            }
            get_note(e);
            break;
        case SDLK_c:
            if ((SDL_GetModState() & KMOD_CTRL) && (SDL_GetModState() & KMOD_SHIFT))
            {
                copy_block(b_pos);
                break;
            } else if (SDL_GetModState() & KMOD_CTRL)
            {
                copy_channel();
                break;
            }
            get_note(e);
            break;
        case SDLK_v:
            if ((SDL_GetModState() & KMOD_CTRL) && (SDL_GetModState() & KMOD_SHIFT))
            {
                paste_block(b_pos);
                break;
            } else if (SDL_GetModState() & KMOD_CTRL)
            {
                paste_channel();
                break;
            }
            get_note(e);
            break;
        case SDLK_n:
            if ((SDL_GetModState() & KMOD_CTRL) && (SDL_GetModState() & KMOD_SHIFT))
            {
                create_block(true);
                break;
            } else if (SDL_GetModState() & KMOD_CTRL)
            {
                create_block(false);
                break;
            }
            get_note(e);
            break;
        case SDLK_F1:
            octave = 1; update_info();
            break;
        case SDLK_F2:
            octave = 2; update_info();
            break;
        case SDLK_F3:
            octave = 3; update_info();
            break;
        case SDLK_F4:
            octave = 4; update_info();
            break;
        case SDLK_F5:
            octave = 5; update_info();
            break;
        case SDLK_F6:
            pos = 0;
            break;
        case SDLK_F7:
            pos = block[b_pos].length / 4;
            break;
        case SDLK_F8:
            pos = block[b_pos].length / 2;
            break;
        case SDLK_F9:
            pos = block[b_pos].length - (block[b_pos].length / 4);
            break;
        case SDLK_F10:
            pos = block[b_pos].length - 1;
            break;
        case SDLK_1:
            if (SDL_GetModState() & KMOD_CTRL){pos = 0;}
            else if (SDL_GetModState() & KMOD_SHIFT && SDL_GetModState() & KMOD_ALT){copy_note(0);}
            else if (SDL_GetModState() & KMOD_SHIFT) {paste_note(0);}else
            {get_note(e);}
            break;
        case SDLK_2:
            if (SDL_GetModState() & KMOD_CTRL){pos = block[b_pos].length / 4;}
            else if (SDL_GetModState() & KMOD_SHIFT && SDL_GetModState() & KMOD_ALT){copy_note(1);}
            else if (SDL_GetModState() & KMOD_SHIFT) {paste_note(1);}else
            {get_note(e);}
            break;
        case SDLK_3:
            if (SDL_GetModState() & KMOD_CTRL){pos = block[b_pos].length / 2;}
            else if (SDL_GetModState() & KMOD_SHIFT && SDL_GetModState() & KMOD_ALT){copy_note(2);}
            else if (SDL_GetModState() & KMOD_SHIFT) {paste_note(2);}else
            {get_note(e);}
            break;
        case SDLK_4:
            if (SDL_GetModState() & KMOD_CTRL){pos = block[b_pos].length - (block[b_pos].length / 4);}
            else if (SDL_GetModState() & KMOD_SHIFT && SDL_GetModState() & KMOD_ALT){copy_note(3);}
            else if (SDL_GetModState() & KMOD_SHIFT) {paste_note(3);}else
            {get_note(e);}
            break;
        case SDLK_5:
            if (SDL_GetModState() & KMOD_CTRL){pos = block[b_pos].length - 1;}
            else if (SDL_GetModState() & KMOD_SHIFT && SDL_GetModState() & KMOD_ALT){copy_note(4);}
            else if (SDL_GetModState() & KMOD_SHIFT) {paste_note(4);}else
            {get_note(e);}
            break;
        case SDLK_6:
            if (SDL_GetModState() & KMOD_SHIFT && SDL_GetModState() & KMOD_ALT){copy_note(5);}
            else if (SDL_GetModState() & KMOD_SHIFT) {paste_note(5);}else
            {get_note(e);}
            break;
        case SDLK_7:
            if (SDL_GetModState() & KMOD_SHIFT && SDL_GetModState() & KMOD_ALT){copy_note(6);}
            else if (SDL_GetModState() & KMOD_SHIFT) {paste_note(6);}else
            {get_note(e);}
            break;
        case SDLK_8:
            if (SDL_GetModState() & KMOD_SHIFT && SDL_GetModState() & KMOD_ALT){copy_note(7);}
            else if (SDL_GetModState() & KMOD_SHIFT) {paste_note(7);}else
            {get_note(e);}
            break;
        case SDLK_9:
            if (SDL_GetModState() & KMOD_SHIFT && SDL_GetModState() & KMOD_ALT){copy_note(8);}
            else if (SDL_GetModState() & KMOD_SHIFT) {paste_note(8);}else
            {get_note(e);}
            break;
        case SDLK_0:
            if (SDL_GetModState() & KMOD_SHIFT && SDL_GetModState() & KMOD_ALT){copy_note(9);}
            else if (SDL_GetModState() & KMOD_SHIFT) {paste_note(9);}else
            {get_note(e);}
            break;
        default:
            get_note(e);
            break;
    }
}
