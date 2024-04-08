#include "sample_edit.h"

Sample_edit::Sample_edit(Tracker *tracker, TTF_Font *f, Pallet *p)
{
    // get pointers from main func
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

    setup_new_sample();
    update();
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
            if (actual_pos - previous_pos > 0)
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
            }
        } else {
            amp_low = waveform.r.h / 2;
            amp_high = waveform.r.h / 2;
            amp = waveform.r.h / 2;
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
                    ((Uint32*)pixels)[x+(y*waveform.r.w)] = SDL_MapRGB(fmt, pallet->black.r, pallet->black.g, pallet->black.b);
                } else {
                    ((Uint32*)pixels)[x+(y*waveform.r.w)] = SDL_MapRGB(fmt, pallet->bgd.r, pallet->bgd.g, pallet->bgd.b);
                }
            } else {
                if (y >= amp_low && y <= amp_high)
                {
                    ((Uint32*)pixels)[x+(y*waveform.r.w)] = SDL_MapRGB(fmt, pallet->black.r, pallet->black.g, pallet->black.b);
                } else {
                    ((Uint32*)pixels)[x+(y*waveform.r.w)] = SDL_MapRGB(fmt, pallet->bgd.r, pallet->bgd.g, pallet->bgd.b);
                }
            }
        }
    }

    SDL_UnlockTexture(waveform.t);
    SDL_FreeFormat(fmt);
    refresh();
}

void Sample_edit::de_init()
{
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}

void Sample_edit::update()
{
    refresh();
}

void Sample_edit::refresh()
{
    SDL_SetRenderDrawColor(render, pallet->bgd.r, pallet->bgd.g, pallet->bgd.b, 0xFF); // Background color
    SDL_RenderClear(render);
    SDL_SetRenderDrawColor(render, pallet->black.r, pallet->black.g, pallet->black.b, 0xFF); // Black

    SDL_RenderCopy(render, waveform.t, NULL, &waveform.r);
    SDL_RenderDrawRect(render, &waveform.r);

    SDL_RenderPresent(render); // Present image to screen
}

void Sample_edit::open()
{
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_SetWindowInputFocus(window);
    setup_new_sample();
    refresh();
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

void Sample_edit::mouse(int x, int y)
{

    update();
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
            }
            break;
        case SDLK_LEFT:
            if (SDL_GetModState() & KMOD_SHIFT)
            {
                t->sample_dec();
                setup_new_sample();
            }
            break;
        default:
            break;
    }
    update();
}
