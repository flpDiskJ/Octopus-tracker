#include "sample_edit.h"

Sample_edit::Sample_edit(AudioW *audio, Tracker *tracker, TTF_Font *f, Pallet *p)
{
    // get pointers from main func
    audioworks = audio;
    t = tracker;
    font = f;
    pallet = p;

    // initialize window and renderer
    window = SDL_CreateWindow("Sample Editor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    1000, 400, SDL_WINDOW_HIDDEN);
    if (window == NULL)
    {
        printf("Sample Editor window creation failed!\n");
        return;
    }

    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (render == NULL)
    {
        printf("Failed to create renderer for sample editor window!\n");
    }

    fmt = SDL_AllocFormat(SDL_GetWindowPixelFormat(window));

    waveform.r.x = 20;
    waveform.r.y = 20;
    waveform.r.w = 960;
    waveform.r.h = 200;
    wave_scale = (SAMPLE_PEAK * 2) / waveform.r.h;
    waveform.t = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, waveform.r.w, waveform.r.h);

    selection_front_entry.r.x = 20;
    selection_front_entry.r.y = 20 + waveform.r.h + 4;
    selection_front_entry.r.w = 12 * 6;
    selection_front_entry.r.h = 20;

    selection_back_entry.r.y = 20 + waveform.r.h + 4;
    selection_back_entry.r.w = 12 * 6;
    selection_back_entry.r.h = 20;
    selection_back_entry.r.x = 20 + selection_front_entry.r.w + 8;

    sample_len_display.r.x = 20 + selection_back_entry.r.w + selection_front_entry.r.w + 16;
    sample_len_display.r.y = 20 + waveform.r.h + 4;
    sample_len_display.r.w = 12 * 6;
    sample_len_display.r.h = 20;

    zoom_to_start.r.w = 14 * 5;
    zoom_to_start.r.h = 20;
    zoom_to_start.r.x = sample_len_display.r.x + sample_len_display.r.w + 8;
    zoom_to_start.r.y = 20 + waveform.r.h + 4;

    zoom_to_end.r.w = 14 * 5;
    zoom_to_end.r.h = 20;
    zoom_to_end.r.x = zoom_to_start.r.x + zoom_to_start.r.w + 8;
    zoom_to_end.r.y = 20 + waveform.r.h + 4;

    zoom_to_start.active = true;

    surf = TTF_RenderText_Solid(font, " <-- ", pallet->blue);
    zoom_to_start.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, " --> ", pallet->blue);
    zoom_to_end.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    zoom_offset = 0;

    range_all.r.w = (14 * 9) - 2;
    range_all.r.h = 20;
    range_all.r.x = zoom_to_end.r.x + zoom_to_end.r.w + 8;
    range_all.r.y = zoom_to_end.r.y;
    tex_gen(&range_all, "Range All");

    cut_b.r.w = 14 * 3;
    cut_b.r.h = 20;
    cut_b.r.x = range_all.r.x + range_all.r.w + 8;
    cut_b.r.y = range_all.r.y;
    tex_gen(&cut_b, "Cut");

    copy_b.r.w = 14 * 4;
    copy_b.r.h = 20;
    copy_b.r.x = cut_b.r.x + cut_b.r.w + 8;
    copy_b.r.y = cut_b.r.y;
    tex_gen(&copy_b, "Copy");

    paste_b.r.w = 14 * 5;
    paste_b.r.h = 20;
    paste_b.r.x = copy_b.r.x + copy_b.r.w + 8;
    paste_b.r.y = copy_b.r.y;
    tex_gen(&paste_b, "Paste");

    copy_buffer = (Sint16*)malloc(sizeof(Sint16)*1000);
    copy_buffer_size = 1000;

    slice_b.r.w = 14 * 5;
    slice_b.r.h = 20;
    slice_b.r.x = paste_b.r.x + paste_b.r.w + 8;
    slice_b.r.y = paste_b.r.y;
    tex_gen(&slice_b, "Slice");

    fade_in_b.r.w = 14 * 5;
    fade_in_b.r.h = 20;
    fade_in_b.r.x = slice_b.r.x + slice_b.r.w + 8;
    fade_in_b.r.y = slice_b.r.y;
    tex_gen(&fade_in_b, "Fade In");

    fade_out_b.r.w = 14 * 6;
    fade_out_b.r.h = 20;
    fade_out_b.r.x = fade_in_b.r.x + fade_in_b.r.w + 8;
    fade_out_b.r.y = fade_in_b.r.y;
    tex_gen(&fade_out_b, "Fade Out");

    vol_e.r.x = zoom_to_end.r.x;
    vol_e.r.y = 20 + waveform.r.h + selection_front_entry.r.h + 8;
    vol_e.r.w = zoom_to_end.r.w;
    vol_e.r.h = 20;
    vol_e.text = "100";

    vol_b.r.x = range_all.r.x;
    vol_b.r.y = vol_e.r.y;
    vol_b.r.w = range_all.r.w;
    vol_b.r.h = 20;
    tex_gen(&vol_b, "Change Vol");

    sample_rate_d.r.x = selection_front_entry.r.x;
    sample_rate_d.r.y = vol_b.r.y;
    sample_rate_d.r.w = selection_front_entry.r.w;
    sample_rate_d.r.h = 20;

    half_rate_b.r.x = selection_back_entry.r.x;
    half_rate_b.r.y = vol_e.r.y;
    half_rate_b.r.w = selection_back_entry.r.w;
    half_rate_b.r.h = 20;
    tex_gen(&half_rate_b, " 1/2 ");

    third_rate_b.r.x = sample_len_display.r.x;
    third_rate_b.r.y = vol_e.r.y;
    third_rate_b.r.w = sample_len_display.r.w;
    third_rate_b.r.h = 20;
    tex_gen(&third_rate_b, " 1/3 ");

    quarter_rate_b.r.x = zoom_to_start.r.x;
    quarter_rate_b.r.y = vol_e.r.y;
    quarter_rate_b.r.w = zoom_to_start.r.w;
    quarter_rate_b.r.h = 20;
    tex_gen(&quarter_rate_b, " 1/4 ");

    rev_b.r.x = cut_b.r.x;
    rev_b.r.y = vol_e.r.y;
    rev_b.r.w = cut_b.r.w;
    rev_b.r.h = 20;
    tex_gen(&rev_b, "Rev");

    loop_on.r.x = copy_b.r.x;
    loop_on.r.y = vol_e.r.y;
    loop_on.r.w = copy_b.r.w;
    loop_on.r.h = 20;
    tex_gen(&loop_on, "Loop");

    loop_mode.r.x = paste_b.r.x;
    loop_mode.r.y = vol_e.r.y;
    loop_mode.r.w = paste_b.r.w;
    loop_mode.r.h = 20;

    setup_new_sample();
    update_vol_entry();
}

Sample_edit::~Sample_edit()
{
    free(copy_buffer);
}

void Sample_edit::setup_new_sample()
{
    wave_zoom = (double)t->sample[t->s_pos].len / (double)waveform.r.w;
    wave_offset = 0;
    if (t->sample[t->s_pos].loop == 1)
    {
        tex_gen(&loop_mode, " --> ");
    } else if (t->sample[t->s_pos].loop == 2)
    {
        tex_gen(&loop_mode, " <-> ");
    } else {
        tex_gen(&loop_mode, " --- ");
    }
    select_all();
    update_selection_index();
}

void Sample_edit::draw_wave()
{
    void *pixels;
    int pitch;
    SDL_LockTexture(waveform.t, NULL, &pixels, &pitch);
    int amp_high = waveform.r.h / 2;
    int amp_low = waveform.r.h / 2;
    int amp = waveform.r.h / 2;
    double pos = wave_offset;
    int actual_pos = wave_offset;
    int previous_pos = actual_pos;

    for (int x = 0; x < waveform.r.w; x++)
    {
        // keep position within sample bounds
        if (actual_pos < t->sample[t->s_pos].len && actual_pos >= 0 && previous_pos >= 0 && previous_pos < t->sample[t->s_pos].len)
        {
            amp_low = amp_high = 0;
            for (int skim = previous_pos; skim < actual_pos; skim++) // find max value in sample range
            {
                if (amp_high < t->sample[t->s_pos].data[skim])
                {
                    amp_high = t->sample[t->s_pos].data[skim];
                }
                if (amp_low > t->sample[t->s_pos].data[skim])
                {
                    amp_low = t->sample[t->s_pos].data[skim];
                }
            }
            amp = ((double)(t->sample[t->s_pos].data[actual_pos] + SAMPLE_PEAK)) / wave_scale;
            amp_low = ((double)(amp_low + SAMPLE_PEAK)) / wave_scale;
            amp_high = ((double)(amp_high + SAMPLE_PEAK)) / wave_scale;
            if (amp_low < 0){amp_low = 0;}
            else if (amp_low >= waveform.r.h){amp_low = waveform.r.h - 1;}
            if (amp_high < 0){amp_high = 0;}
            else if (amp_high >= waveform.r.h){amp_high = waveform.r.h - 1;}
        } else {
            amp_low = 0;
            amp_high = 0;
            amp = 0;
        }

        // increment sample data postion
        pos += wave_zoom;
        previous_pos = actual_pos;
        actual_pos = (int)pos;

        for (int y = 0; y < waveform.r.h; y++)
        {
            // Draw waveform
            if (wave_zoom < 2)
            {
                if (y == amp)
                {
                    ((Uint32*)pixels)[x+(y*waveform.r.w)] = SDL_MapRGB(fmt, pallet->black.b, pallet->black.g, pallet->black.r);
                } else if (y == 101) {
                    ((Uint32*)pixels)[x+(y*waveform.r.w)] = SDL_MapRGB(fmt, pallet->red.b, pallet->red.g, pallet->red.r);
                } else {
                    if (actual_pos >= selection.sample_front && actual_pos <= selection.sample_back)
                    {
                        ((Uint32*)pixels)[x+(y*waveform.r.w)] = SDL_MapRGB(fmt, pallet->white.b, pallet->white.g, pallet->white.r);
                    } else {
                        ((Uint32*)pixels)[x+(y*waveform.r.w)] = SDL_MapRGB(fmt, pallet->bgd.b, pallet->bgd.g, pallet->bgd.r);
                    }
                }
            } else {
                if (y >= amp_low && y <= amp_high)
                {
                    ((Uint32*)pixels)[x+(y*waveform.r.w)] = SDL_MapRGB(fmt, pallet->black.b, pallet->black.g, pallet->black.r);
                } else if (y == 101) {
                    ((Uint32*)pixels)[x+(y*waveform.r.w)] = SDL_MapRGB(fmt, pallet->red.b, pallet->red.g, pallet->red.r);
                } else {
                    if (actual_pos >= selection.sample_front && actual_pos <= selection.sample_back)
                    {
                        ((Uint32*)pixels)[x+(y*waveform.r.w)] = SDL_MapRGB(fmt, pallet->white.b, pallet->white.g, pallet->white.r);
                    } else {
                        ((Uint32*)pixels)[x+(y*waveform.r.w)] = SDL_MapRGB(fmt, pallet->bgd.b, pallet->bgd.g, pallet->bgd.r);
                    }
                }
            }
        }
    }
    SDL_UnlockTexture(waveform.t);
}

void Sample_edit::get_sample_postions(int x, bool front)
{
    x = x - waveform.r.x;
    if (front)
    {
        selection.sample_front = ((double)x * wave_zoom) + wave_offset;
        if (selection.sample_front > t->sample[t->s_pos].len)
        {
            selection.sample_front = t->sample[t->s_pos].len;
        }
    } else {

        selection.sample_back = ((double)x * wave_zoom) + wave_offset;
        if (selection.sample_back > t->sample[t->s_pos].len)
        {
            selection.sample_back = t->sample[t->s_pos].len;
        }
    }
    update_selection_index();
}

string Sample_edit::blank_fill(string input, int len, char fill_char)
{
    string out;
    int fill = len - input.length();
    for (int i = 0; i < fill; i++)
    {
        out += fill_char;
    }
    out += input;
    return out;
}

void Sample_edit::bound_offset()
{
    if (wave_offset < 0)
    {
        wave_offset = 0;
    } else if (wave_offset >= t->sample[t->s_pos].len)
    {
        wave_offset = t->sample[t->s_pos].len - 1;
    }
}

void Sample_edit::get_zoom_offset()
{
    if (zoom_to_start.active)
    {
        zoom_offset = selection.sample_front;
    } else {
        zoom_offset = selection.sample_back;
    }
}

void Sample_edit::select_all()
{
    selection.sample_front = 0;
    if (t->sample[t->s_pos].len == 0)
    {
        selection.sample_back = 0;
        return;
    }
    selection.sample_back = t->sample[t->s_pos].len - 1;
}

void Sample_edit::render_struct(SDL_Renderer *r, Button *b, Entry *e)
{
    if (b != NULL)
    {
        SDL_RenderDrawRect(r, &b->r);
        SDL_RenderCopy(r, b->t, NULL, &b->r);
    }
    if (e != NULL)
    {
        if (e->active)
        {
            SDL_SetRenderDrawColor(r, pallet->red.r, pallet->red.g, pallet->red.b, 0xFF);
        }
        SDL_RenderDrawRect(r, &e->r);
        SDL_RenderCopy(r, e->t, NULL, &e->r);
        SDL_SetRenderDrawColor(r, pallet->black.r, pallet->black.g, pallet->black.b, 0xFF);
    }
}

void Sample_edit::update_selection_index()
{
    string length = to_string(t->sample[t->s_pos].len);
    length = blank_fill(length, 6, '0');
    surf = TTF_RenderText_Solid(font, length.c_str(), pallet->black);
    if (sample_len_display.t != NULL)
    {
        SDL_DestroyTexture(sample_len_display.t);
    }
    sample_len_display.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    string data;
    if (selection_front_entry.t != NULL)
    {
        SDL_DestroyTexture(selection_front_entry.t);
    }
    data = to_string(selection.sample_front);
    data = blank_fill(data, 6, '0');
    surf = TTF_RenderText_Solid(font, data.c_str(), pallet->black);
    selection_front_entry.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);
    selection_front_entry.text = data;

    if (selection_back_entry.t != NULL)
    {
        SDL_DestroyTexture(selection_back_entry.t);
    }
    data = to_string(selection.sample_back);
    data = blank_fill(data, 6, '0');
    surf = TTF_RenderText_Solid(font, data.c_str(), pallet->black);
    selection_back_entry.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);
    selection_back_entry.text = data;

    if (sample_rate_d.t != NULL)
    {
        SDL_DestroyTexture(sample_rate_d.t);
    }
    data = to_string(t->sample[t->s_pos].sample_rate);
    data = blank_fill(data, 5, '0');
    data += "Hz";
    surf = TTF_RenderText_Solid(font, data.c_str(), pallet->black);
    sample_rate_d.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    draw_wave();
}

void Sample_edit::update_entries()
{
    if (selection_front_entry.text.length() > 0 && selection_front_entry.text.length() < 7)
    {
        selection.sample_front = stoi(selection_front_entry.text);
    }
    if (selection_back_entry.text.length() > 0 && selection_back_entry.text.length() < 7)
    {
        selection.sample_back = stoi(selection_back_entry.text);
    }
    if (selection.sample_back >= t->sample[t->s_pos].len)
    {
        selection.sample_back = t->sample[t->s_pos].len - 1;
    }
    if (selection.sample_front >= t->sample[t->s_pos].len)
    {
        selection.sample_front = t->sample[t->s_pos].len - 1;
    }
    update_selection_index();
}

void Sample_edit::update_vol_entry()
{
    if (vol_e.t != NULL)
    {
        SDL_DestroyTexture(vol_e.t);
    }
    if (vol_e.text.length() == 0)
    {
        vol_e.text = "0";
    }
    string temp = blank_fill(vol_e.text, 4, ' ');
    temp += "% ";
    surf = TTF_RenderText_Solid(font, temp.c_str(), pallet->black);
    vol_e.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);
}

void Sample_edit::cut_selection()
{
    if (selection.sample_back >= t->sample[t->s_pos].len)
    {
        selection.sample_back = t->sample[t->s_pos].len - 1;
    }
    if (selection.sample_front >= t->sample[t->s_pos].len)
    {
        selection.sample_front = t->sample[t->s_pos].len - 1;
    }
    long int size = t->sample[t->s_pos].len - (selection.sample_back - selection.sample_front);
    if (size < 2)
    {
        selection.sample_front = 0;
        selection.sample_back = 0;
        t->sample[t->s_pos].len = 0;
        draw_wave();
        return;
    }
    Sint16 *buffer = (Sint16*)malloc(sizeof(Sint16)*size);

    for (int p = 0, new_p = 0; p < t->sample[t->s_pos].len; p++)
    {
        if (p < selection.sample_front || p > selection.sample_back)
        {
            buffer[new_p] = t->sample[t->s_pos].data[p];
            new_p++;
        }
    }

    if (size > t->sample[t->s_pos].len)
    {
        t->sample[t->s_pos].data = (Sint16*)realloc(t->sample[t->s_pos].data, sizeof(Sint16)*size);
    }
    t->sample[t->s_pos].len = size;

    memcpy(t->sample[t->s_pos].data, buffer, size*sizeof(Sint16));
    free(buffer);
    if (selection.sample_back >= t->sample[t->s_pos].len)
    {
        selection.sample_back = t->sample[t->s_pos].len - 1;
    }
    if (selection.sample_front >= t->sample[t->s_pos].len)
    {
        selection.sample_front = t->sample[t->s_pos].len - 1;
    }
    update_selection_index();
}

void Sample_edit::copy_selection()
{
    if (selection.sample_back >= t->sample[t->s_pos].len)
    {
        selection.sample_back = t->sample[t->s_pos].len - 1;
    }
    if (selection.sample_front >= t->sample[t->s_pos].len)
    {
        selection.sample_front = t->sample[t->s_pos].len - 1;
    }
    if (copy_buffer_size < selection.sample_back - selection.sample_front)
    {
        copy_buffer_size = (selection.sample_back - selection.sample_front) + 1;
        copy_buffer = (Sint16*)realloc(copy_buffer, sizeof(Sint16)*copy_buffer_size);
    }
    long int size = 0;
    for (int p = selection.sample_front; p < selection.sample_back; p++)
    {
        copy_buffer[size] = t->sample[t->s_pos].data[p];
        size++;
    }
    paste_size = size;
}

void Sample_edit::paste_buffer()
{
    long int size = t->sample[t->s_pos].len + paste_size;
    Sint16 *buffer = (Sint16*)malloc(sizeof(Sint16)*size);

    for (int p = 0, old_p = 0, new_p = 0; p < size; p++)
    {
        if (p < selection.sample_front || p > selection.sample_front + (paste_size - 1))
        {
            buffer[p] = t->sample[t->s_pos].data[old_p];
            old_p++;
        } else {
            buffer[p] = copy_buffer[new_p];
            new_p++;
        }
    }

    if (t->sample[t->s_pos].data == NULL)
    {
        t->sample[t->s_pos].data = (Sint16*)malloc(sizeof(Sint16)*size);
        t->sample[t->s_pos].level = 100;
    } else if (t->sample[t->s_pos].len < size)
    {
        t->sample[t->s_pos].data = (Sint16*)realloc(t->sample[t->s_pos].data, sizeof(Sint16)*size);
    }
    t->sample[t->s_pos].len = size;
    memcpy(t->sample[t->s_pos].data, buffer, size*sizeof(Sint16));
    free(buffer);
    setup_new_sample();
}

void Sample_edit::slice_selection()
{
    if (selection.sample_back >= t->sample[t->s_pos].len)
    {
        selection.sample_back = t->sample[t->s_pos].len - 1;
    }
    if (selection.sample_front >= t->sample[t->s_pos].len)
    {
        selection.sample_front = t->sample[t->s_pos].len - 1;
    }
    long int size = selection.sample_back - selection.sample_front;
    if (size < 2)
    {
        selection.sample_front = 0;
        selection.sample_back = 0;
        t->sample[t->s_pos].len = 0;
        draw_wave();
        return;
    }
    Sint16 *buffer = (Sint16*)malloc(sizeof(Sint16)*size);

    for (int p = 0, new_p = 0; p < t->sample[t->s_pos].len; p++)
    {
        if (p >= selection.sample_front && p < selection.sample_back)
        {
            buffer[new_p] = t->sample[t->s_pos].data[p];
            new_p++;
        }
    }

    if (size > t->sample[t->s_pos].len)
    {
        t->sample[t->s_pos].data = (Sint16*)realloc(t->sample[t->s_pos].data, sizeof(Sint16)*size);
    }
    t->sample[t->s_pos].len = size;

    memcpy(t->sample[t->s_pos].data, buffer, size*sizeof(Sint16));
    free(buffer);
    if (selection.sample_back >= t->sample[t->s_pos].len)
    {
        selection.sample_back = t->sample[t->s_pos].len - 1;
    }
    if (selection.sample_front >= t->sample[t->s_pos].len)
    {
        selection.sample_front = t->sample[t->s_pos].len - 1;
    }
    update_selection_index();
}

void Sample_edit::fade_in_selection()
{
    if (selection.sample_back >= t->sample[t->s_pos].len)
    {
        selection.sample_back = t->sample[t->s_pos].len - 1;
    }
    if (selection.sample_front >= t->sample[t->s_pos].len)
    {
        selection.sample_front = t->sample[t->s_pos].len - 1;
    }

    long int selection_len = selection.sample_back - selection.sample_front;
    if (selection_len < 2)
    {
        return;
    }

    double amplifier = 0.0;
    double increase = 1.0 / (double)selection_len;
    for (int p = 0; p < t->sample[t->s_pos].len; p++)
    {
        if (p >= selection.sample_front && p < selection.sample_back)
        {
            t->sample[t->s_pos].data[p] = t->sample[t->s_pos].data[p] * amplifier;
            amplifier += increase;
        }
    }
    draw_wave();
}

void Sample_edit::fade_out_selection()
{
    if (selection.sample_back >= t->sample[t->s_pos].len)
    {
        selection.sample_back = t->sample[t->s_pos].len - 1;
    }
    if (selection.sample_front >= t->sample[t->s_pos].len)
    {
        selection.sample_front = t->sample[t->s_pos].len - 1;
    }

    long int selection_len = selection.sample_back - selection.sample_front;
    if (selection_len < 2)
    {
        return;
    }

    double amplifier = 1.0;
    double decrease = 1.0 / (double)selection_len;
    for (int p = 0; p < t->sample[t->s_pos].len; p++)
    {
        if (p >= selection.sample_front && p < selection.sample_back)
        {
            t->sample[t->s_pos].data[p] = t->sample[t->s_pos].data[p] * amplifier;
            amplifier -= decrease;
        }
    }
    draw_wave();
}

void Sample_edit::text_entry(Entry *entry, SDL_Event *e)
{
    string keyname;
    switch (e->key.keysym.sym)
    {
        case SDLK_DELETE:
        case SDLK_BACKSPACE:
            if (entry->text.length() > 0)
            {
                entry->text.pop_back();
            }
            break;
        case SDLK_RETURN:
            entry->active = false;
            update_entries();
            update_vol_entry();
            return;
            break;
        case SDLK_0:
        case SDLK_1:
        case SDLK_2:
        case SDLK_3:
        case SDLK_4:
        case SDLK_5:
        case SDLK_6:
        case SDLK_7:
        case SDLK_8:
        case SDLK_9:
            if (entry->text.length() < 8)
            {
                keyname = SDL_GetKeyName(e->key.keysym.sym);
                entry->text += keymap.sdl_getText(keyname, (SDL_GetModState() & KMOD_SHIFT));
            }
            break;
        default:
            break;
    }
    if (entry->t != NULL)
    {
        SDL_DestroyTexture(entry->t);
    }
    surf = TTF_RenderText_Solid(font, entry->text.c_str(), pallet->black);
    entry->t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);
}

void Sample_edit::change_vol()
{
    double amp = (double)stoi(vol_e.text) / 100.0;
    for (int p = selection.sample_front; p < selection.sample_back; p++)
    {
        t->sample[t->s_pos].data[p] = (double)t->sample[t->s_pos].data[p] * amp;
        if (t->sample[t->s_pos].data[p] > SAMPLE_PEAK)
        {
            t->sample[t->s_pos].data[p] = SAMPLE_PEAK;
        } else if (t->sample[t->s_pos].data[p] < 0 - SAMPLE_PEAK)
        {
            t->sample[t->s_pos].data[p] = 0 - SAMPLE_PEAK;
        }
    }
    draw_wave();
}

void Sample_edit::tex_gen(Button *bt, const char *text)
{
    if (bt->t != NULL)
    {
        SDL_DestroyTexture(bt->t);
    }
    surf = TTF_RenderText_Solid(font, text, pallet->black);
    bt->t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);
}

void Sample_edit::reverse_selection()
{
    if (selection.sample_back >= t->sample[t->s_pos].len)
    {
        selection.sample_back = t->sample[t->s_pos].len - 1;
    }
    if (selection.sample_front >= t->sample[t->s_pos].len)
    {
        selection.sample_front = t->sample[t->s_pos].len - 1;
    }
    long int length = sizeof(Sint16)*t->sample[t->s_pos].len;
    Sint16 *buffer = (Sint16*)malloc(length);
    memcpy(buffer, t->sample[t->s_pos].data, length);
    int pos = selection.sample_front;
    for (int p = selection.sample_back; p > selection.sample_front; p--)
    {
        buffer[pos] = t->sample[t->s_pos].data[p];
        pos++;
    }
    memcpy(t->sample[t->s_pos].data, buffer, length);
    free(buffer);
    draw_wave();
}

void Sample_edit::de_init()
{
    SDL_FreeFormat(fmt);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}

void Sample_edit::refresh()
{
    SDL_SetRenderDrawColor(render, pallet->bgd.r, pallet->bgd.g, pallet->bgd.b, 0xFF); // Background color
    SDL_RenderClear(render);
    SDL_SetRenderDrawColor(render, pallet->black.r, pallet->black.g, pallet->black.b, 0xFF); // Black

    SDL_RenderCopy(render, waveform.t, NULL, &waveform.r);
    SDL_RenderDrawRect(render, &waveform.r);

    render_struct(render, NULL, &selection_front_entry);
    render_struct(render, NULL, &selection_back_entry);
    render_struct(render, &sample_len_display, NULL);

    if (zoom_to_start.active)
    {
        SDL_RenderFillRect(render, &zoom_to_start.r);
    } else {
        SDL_RenderDrawRect(render, &zoom_to_start.r);
    }
    if (zoom_to_end.active)
    {
        SDL_RenderFillRect(render, &zoom_to_end.r);
    } else {
        SDL_RenderDrawRect(render, &zoom_to_end.r);
    }
    SDL_RenderCopy(render, zoom_to_start.t, NULL, &zoom_to_start.r);
    SDL_RenderCopy(render, zoom_to_end.t, NULL, &zoom_to_end.r);

    render_struct(render, &loop_mode, NULL);

    render_struct(render, &range_all, NULL);
    render_struct(render, &cut_b, NULL);
    render_struct(render, &copy_b, NULL);
    render_struct(render, &paste_b, NULL);
    render_struct(render, &slice_b, NULL);
    render_struct(render, &fade_in_b, NULL);
    render_struct(render, &fade_out_b, NULL);
    render_struct(render, &vol_b, &vol_e);
    render_struct(render, &sample_rate_d, NULL);
    render_struct(render, &half_rate_b, NULL);
    render_struct(render, &third_rate_b, NULL);
    render_struct(render, &quarter_rate_b, NULL);
    render_struct(render, &rev_b, NULL);

    if (t->channel[0].play)
    {
        SDL_SetRenderDrawColor(render, pallet->green.r, pallet->green.g, pallet->green.b, 0xFF);
        int pos = (int)t->channel[0].pos;
        if (pos > wave_offset && pos < wave_offset + (waveform.r.w * wave_zoom))
        {
            int x = waveform.r.x + ((pos - wave_offset) / wave_zoom);
            SDL_RenderDrawLine(render, x, waveform.r.y + 1, x, waveform.r.y + waveform.r.h - 2);
        }
    }

    if (t->sample[t->s_pos].loop > 0)
    {
        SDL_SetRenderDrawColor(render, pallet->blue.r, pallet->blue.g, pallet->blue.b, 0xFF);
        SDL_RenderFillRect(render, &loop_on.r);
    } else {
        SDL_SetRenderDrawColor(render, pallet->black.r, pallet->black.g, pallet->black.b, 0xFF);
        SDL_RenderDrawRect(render, &loop_on.r);
    }
    SDL_RenderCopy(render, loop_on.t, NULL, &loop_on.r);

    SDL_RenderPresent(render); // Present image to screen
}

void Sample_edit::open()
{
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_SetWindowInputFocus(window);
    setup_new_sample();
}

void Sample_edit::close()
{
    SDL_HideWindow(window);
}

Uint32 Sample_edit::get_state()
{
    return SDL_GetWindowFlags(window);
}

bool Sample_edit::checkButton(int mouseX, int mouseY, SDL_Rect *button)
{
    if (mouseX > button->x && mouseX < button->w+button->x && mouseY > button->y && mouseY < button->h+button->y)
    {
        return true;
    }
    return false;
}

void Sample_edit::mouse(int x, int y, SDL_Event *e)
{
    if (checkButton(x, y, &zoom_to_start.r))
    {
        zoom_to_start.active = true;
        zoom_to_end.active = false;
        get_zoom_offset();
        wave_offset = zoom_offset-((waveform.r.w/2)*wave_zoom);
        bound_offset();
        draw_wave();
    } else if (checkButton(x, y, &zoom_to_end.r))
    {
        zoom_to_start.active = false;
        zoom_to_end.active = true;
        get_zoom_offset();
        wave_offset = zoom_offset-((waveform.r.w/2)*wave_zoom);
        bound_offset();
        draw_wave();
    } else if (checkButton(x, y, &range_all.r))
    {
        select_all();
        draw_wave();
        update_selection_index();
    } else if (checkButton(x, y, &cut_b.r)){
        cut_selection();
    } else if (checkButton(x, y, &copy_b.r)){
        copy_selection();
    } else if (checkButton(x, y, &paste_b.r)){
        paste_buffer();
    } else if (checkButton(x, y, &slice_b.r)){
        slice_selection();
    } else if (checkButton(x, y, &fade_in_b.r)){
        fade_in_selection();
    } else if (checkButton(x, y, &fade_out_b.r)){
        fade_out_selection();
    } else if (checkButton(x, y, &selection_front_entry.r)){
        selection_front_entry.active = true;
    } else if (checkButton(x, y, &selection_back_entry.r)){
        selection_back_entry.active = true;
    } else if (checkButton(x, y, &vol_e.r)){
        vol_e.active = true;
    } else if (checkButton(x, y, &vol_b.r)){
        change_vol();
    } else if (checkButton(x, y, &half_rate_b.r)){
        t->resample(t->s_pos, t->sample[t->s_pos].sample_rate, t->sample[t->s_pos].sample_rate / 2);
        update_selection_index();
    } else if (checkButton(x, y, &third_rate_b.r)){
        t->resample(t->s_pos, t->sample[t->s_pos].sample_rate,
            t->sample[t->s_pos].sample_rate - (t->sample[t->s_pos].sample_rate * 0.3333333333));
        update_selection_index();
    } else if (checkButton(x, y, &quarter_rate_b.r)){
        t->resample(t->s_pos, t->sample[t->s_pos].sample_rate,
            t->sample[t->s_pos].sample_rate - (t->sample[t->s_pos].sample_rate * 0.25));
        update_selection_index();
    } else if (checkButton(x, y, &rev_b.r)){
        reverse_selection();
    } else if (checkButton(x, y, &loop_on.r)){
        if (t->sample[t->s_pos].loop == 0)
        {
            t->sample[t->s_pos].loop = 1;
            tex_gen(&loop_mode, " --> ");
        } else {
            t->sample[t->s_pos].loop = 0;
            tex_gen(&loop_mode, " --- ");
        }
    } else if (checkButton(x, y, &loop_mode.r)){
        if (t->sample[t->s_pos].loop == 2)
        {
            t->sample[t->s_pos].loop = 1;
            tex_gen(&loop_mode, " --> ");
        } else if (t->sample[t->s_pos].loop == 1)
        {
            t->sample[t->s_pos].loop = 2;
            tex_gen(&loop_mode, " <-> ");
        }
    } else if (checkButton(x, y, &waveform.r))
    {
        switch (e->button.button)
        {
            case SDL_BUTTON_LEFT:
                get_sample_postions(x, true);
                draw_wave();
                break;
            case SDL_BUTTON_RIGHT:
                get_sample_postions(x, false);
                draw_wave();
                break;
            case SDL_BUTTON_MIDDLE:
                get_sample_postions(x, true);
                get_sample_postions(x, false);
                draw_wave();
                break;
            default:
                break;
        }
        get_zoom_offset();
    }
}

void Sample_edit::mouse_wheel(SDL_Event *e)
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    if (checkButton(x, y, &waveform.r))
    {
        if(e->wheel.y < 0) // scroll down
        {
             if (wave_zoom < (t->sample[t->s_pos].len + 10000) / waveform.r.w)
             {
                 wave_zoom *= 1.5;
                 wave_offset = zoom_offset-((waveform.r.w/2)*wave_zoom);
                 bound_offset();
             }
             draw_wave();
        }
        else if(e->wheel.y > 0) // scroll up
        {
             if (wave_zoom > 0.1)
             {
                 wave_zoom /= 1.5;
                 wave_offset = zoom_offset-((waveform.r.w/2)*wave_zoom);
                 bound_offset();
             }
             draw_wave();
        }
    }
}

void Sample_edit::keyboard(SDL_Event *e)
{
    if (selection_front_entry.active)
    {
        text_entry(&selection_front_entry, e);
    } else if (selection_back_entry.active)
    {
        text_entry(&selection_back_entry, e);
    } else if (vol_e.active)
    {
        text_entry(&vol_e, e);
    } else {
        switch (e->key.keysym.sym)
        {
            case SDLK_RIGHT:
                if (SDL_GetModState() & KMOD_SHIFT)
                {
                    t->sample_inc();
                    setup_new_sample();
                } else {
                    wave_offset += wave_zoom * (double)waveform_zoom_sensitivity;
                    bound_offset();
                    draw_wave();
                }
                break;
            case SDLK_LEFT:
                if (SDL_GetModState() & KMOD_SHIFT)
                {
                    t->sample_dec();
                    setup_new_sample();
                } else {
                    wave_offset -= wave_zoom * (double)waveform_zoom_sensitivity;
                    bound_offset();
                    draw_wave();
                }
                break;
            default:
                if (!(SDL_GetModState() & KMOD_CTRL) && !(SDL_GetModState() & KMOD_SHIFT))
                {
                    audioworks->play_sample(e, t->s_pos);
                }
                break;
        }
    }
}
