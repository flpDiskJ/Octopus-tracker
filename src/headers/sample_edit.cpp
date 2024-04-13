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

    waveform.r.x = 20;
    waveform.r.y = 20;
    waveform.r.w = 960;
    waveform.r.h = 200;
    wave_scale = (SAMPLE_PEAK * 2) / waveform.r.h;
    waveform.t = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, waveform.r.w, waveform.r.h);;

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

    setup_new_sample();
}

Sample_edit::~Sample_edit()
{

}

void Sample_edit::setup_new_sample()
{
    wave_zoom = (double)t->sample[t->s_pos].len / (double)waveform.r.w;
    selection.front = 0;
    selection.back = 0;
    wave_offset = 0;
    draw_wave();

    // set sample length display
    if (sample_len_display.t != NULL)
    {
        SDL_DestroyTexture(sample_len_display.t);
    }
    string length = to_string(t->sample[t->s_pos].len);
    length = blank_fill(length, 6, '0');
    surf = TTF_RenderText_Solid(font, length.c_str(), pallet->black);
    sample_len_display.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);
    get_sample_postions();
}

void Sample_edit::draw_wave()
{
    void *pixels;
    int pitch;
    SDL_PixelFormat *fmt = SDL_AllocFormat(SDL_GetWindowPixelFormat(window));
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
    SDL_FreeFormat(fmt);
}

void Sample_edit::get_sample_postions()
{
    selection.sample_front = (double)(selection.front * wave_zoom) + wave_offset;
    selection.sample_back = (double)(selection.back * wave_zoom) + wave_offset;

    if (selection.sample_front > selection.sample_back)
    {
        selection.sample_front = selection.sample_back;
    }

    if (selection.sample_front > t->sample[t->s_pos].len)
    {
        selection.sample_front = t->sample[t->s_pos].len;
        get_positions();
    }
    if (selection.sample_back > t->sample[t->s_pos].len)
    {
        selection.sample_back = t->sample[t->s_pos].len;
        get_positions();
    }

    if (selection_front_entry.t != NULL)
    {
        SDL_DestroyTexture(selection_front_entry.t);
    }

    if (selection_back_entry.t != NULL)
    {
        SDL_DestroyTexture(selection_back_entry.t);
    }

    string data = to_string(selection.sample_front);
    data = blank_fill(data, 6, '0');
    surf = TTF_RenderText_Solid(font, data.c_str(), pallet->black);
    selection_front_entry.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    data = to_string(selection.sample_back);
    data = blank_fill(data, 6, '0');
    surf = TTF_RenderText_Solid(font, data.c_str(), pallet->black);
    selection_back_entry.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);
}

void Sample_edit::get_positions()
{
    selection.front = (selection.sample_front - wave_offset) / wave_zoom;
    selection.back = (selection.sample_back - wave_offset) / wave_zoom;
}

string Sample_edit::blank_fill(string input, int len, char fill_char)
{
    string out;
    int fill = len - strlen(input.c_str());
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

void Sample_edit::de_init()
{
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}

void Sample_edit::refresh()
{
    SDL_SetRenderDrawColor(render, pallet->bgd.b, pallet->bgd.g, pallet->bgd.r, 0xFF); // Background color
    SDL_RenderClear(render);
    SDL_SetRenderDrawColor(render, pallet->black.b, pallet->black.g, pallet->black.r, 0xFF); // Black

    SDL_RenderCopy(render, waveform.t, NULL, &waveform.r);
    SDL_RenderDrawRect(render, &waveform.r);

    SDL_RenderDrawRect(render, &selection_front_entry.r);
    SDL_RenderDrawRect(render, &selection_back_entry.r);
    SDL_RenderDrawRect(render, &sample_len_display.r);
    SDL_RenderCopy(render, sample_len_display.t, NULL, &sample_len_display.r);
    SDL_RenderCopy(render, selection_front_entry.t, NULL, &selection_front_entry.r);
    SDL_RenderCopy(render, selection_back_entry.t, NULL, &selection_back_entry.r);

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

    if (t->channel[0].play)
    {
        SDL_SetRenderDrawColor(render, pallet->green.b, pallet->green.g, pallet->green.r, 0xFF);
        int pos = (int)t->channel[0].pos;
        if (pos > wave_offset && pos < wave_offset + (waveform.r.w * wave_zoom))
        {
            int x = waveform.r.x + ((pos - wave_offset) / wave_zoom);
            SDL_RenderDrawLine(render, x, waveform.r.y + 1, x, waveform.r.y + waveform.r.h - 2);
        }
    }

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
        get_positions();
        draw_wave();
    } else if (checkButton(x, y, &zoom_to_end.r))
    {
        zoom_to_start.active = false;
        zoom_to_end.active = true;
        get_zoom_offset();
        wave_offset = zoom_offset-((waveform.r.w/2)*wave_zoom);
        bound_offset();
        get_positions();
        draw_wave();
    }
    else if (checkButton(x, y, &waveform.r))
    {
        switch (e->button.button)
        {
            case SDL_BUTTON_LEFT:
                selection.front = x - waveform.r.x;
                get_sample_postions();
                draw_wave();
                break;
            case SDL_BUTTON_RIGHT:
                selection.back = x - waveform.r.x;
                get_sample_postions();
                draw_wave();
                break;
            case SDL_BUTTON_MIDDLE:
                selection.front = x - waveform.r.x;
                selection.back = x - waveform.r.x;
                get_sample_postions();
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
             if (wave_zoom < t->sample[t->s_pos].len / waveform.r.w)
             {
                 wave_zoom *= 1.5;
                 wave_offset = zoom_offset-((waveform.r.w/2)*wave_zoom);
                 bound_offset();
                 get_positions();
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
                 get_positions();
             }
             draw_wave();
        }
    }
}

void Sample_edit::keyboard(SDL_Event *e)
{
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
                get_positions();
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
                get_positions();
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
