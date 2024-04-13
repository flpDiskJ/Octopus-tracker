#include "instrument_properties.h"

Instrument_properties::Instrument_properties(Tracker *tracker, AudioW *a, TTF_Font *f, Pallet *p)
{
    // get pointers from main func
    t = tracker;
    audioworks = a;
    font = f;
    pallet = p;

    // initialize window and renderer
    window = SDL_CreateWindow("Instrument_properties", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    600, 500, SDL_WINDOW_HIDDEN);
    if (window == NULL)
    {
        printf("Instrument window creation failed!\n");
        return;
    }

    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (render == NULL)
    {
        printf("Failed to create renderer for instrument window!\n");
    }

    samplename_entry.r.x = 250;
    samplename_entry.r.y = 20;
    samplename_entry.r.w = 15 * 18;
    samplename_entry.r.h = 30;

    inst_name_label.r.x = 20;
    inst_name_label.r.y = 21;
    inst_name_label.r.w = 16 * 15;
    inst_name_label.r.h = 28;

    transpose_label.r.x = 243;
    transpose_label.r.y = 60;
    transpose_label.r.w = 126;
    transpose_label.r.h = 30;

    transpose_sliderbox.slide.x = 50;
    transpose_sliderbox.slide.y = 90;
    transpose_sliderbox.slide.w = 500;
    transpose_sliderbox.slide.h = 30;

    finetune_label.r.x = 250;
    finetune_label.r.y = 120;
    finetune_label.r.w = 112;
    finetune_label.r.h = 30;

    finetune_sliderbox.slide.x = 50;
    finetune_sliderbox.slide.y = 150;
    finetune_sliderbox.slide.w = 500;
    finetune_sliderbox.slide.h = 30;

    volume_label.r.x = 264;
    volume_label.r.y = 180;
    volume_label.r.w = 84;
    volume_label.r.h = 30;

    volume_sliderbox.slide.x = 47;
    volume_sliderbox.slide.y = 210;
    volume_sliderbox.slide.w = 506;
    volume_sliderbox.slide.h = 30;

    volume_sliderbox.pos.w = 5;
    volume_sliderbox.pos.h = 28;
    volume_sliderbox.pos.x = volume_sliderbox.slide.x + (t->sample[t->s_pos].level * volume_sliderbox.pos.w);
    volume_sliderbox.pos.y = 211;

    surf = TTF_RenderText_Solid(font, "Transpose", pallet->blue);
    transpose_label.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "Finetune", pallet->blue);
    finetune_label.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "Volume", pallet->blue);
    volume_label.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    update();
}

Instrument_properties::~Instrument_properties()
{

}

void Instrument_properties::update_instname()
{
    if (samplename_entry.t != NULL)
    {
        SDL_DestroyTexture(samplename_entry.t);
    }
    string temp = t->sample[t->s_pos].name;
    for (int f = strlen(temp.c_str()); f < 15; f++)
    {
        temp += ' ';
    }
    surf = TTF_RenderText_Solid(font, temp.c_str(), pallet->black);
    samplename_entry.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    if (inst_name_label.t != NULL)
    {
        SDL_DestroyTexture(inst_name_label.t);
    }
    temp = "Instrument ";
    temp += blank_fill(to_string(t->s_pos), 2, ' ');
    temp += ": ";
    surf = TTF_RenderText_Solid(font, temp.c_str(), pallet->black);
    inst_name_label.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    refresh();
}

string Instrument_properties::blank_fill(string input, int len, char fill_char)
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

void Instrument_properties::de_init()
{
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}

void Instrument_properties::update()
{
    volume_sliderbox.pos.x = volume_sliderbox.slide.x + (t->sample[t->s_pos].level * volume_sliderbox.pos.w);
    refresh();
}

void Instrument_properties::refresh()
{
    SDL_SetRenderDrawColor(render, pallet->bgd.r, pallet->bgd.g, pallet->bgd.b, 0xFF); // Background color
    SDL_RenderClear(render);
    SDL_SetRenderDrawColor(render, pallet->black.r, pallet->black.g, pallet->black.b, 0xFF); // Black

    //sample naming
    SDL_RenderDrawRect(render, &samplename_entry.r);
    SDL_RenderCopy(render, samplename_entry.t, NULL, &samplename_entry.r);
    SDL_RenderCopy(render, inst_name_label.t, NULL, &inst_name_label.r);

    //sample params
    SDL_SetRenderDrawColor(render, pallet->blue.r, pallet->blue.g, pallet->blue.b, 0xFF); // Blue
    SDL_RenderCopy(render, transpose_label.t, NULL, &transpose_label.r);
    SDL_RenderDrawRect(render, &transpose_sliderbox.slide);
    SDL_RenderCopy(render, finetune_label.t,  NULL, &finetune_label.r);
    SDL_RenderDrawRect(render, &finetune_sliderbox.slide);
    SDL_RenderCopy(render, volume_label.t,    NULL, &volume_label.r);
    SDL_RenderDrawRect(render, &volume_sliderbox.slide);
    SDL_RenderFillRect(render, &volume_sliderbox.pos);

    SDL_RenderPresent(render); // Present image to screen
}

void Instrument_properties::open()
{
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_SetWindowInputFocus(window);
    update();
    update_instname();
}

void Instrument_properties::close()
{
    SDL_HideWindow(window);
}

Uint32 Instrument_properties::get_state()
{
    return SDL_GetWindowFlags(window);
}

bool Instrument_properties::checkButton(int mouseX, int mouseY, SDL_Rect *button)
{
    if (mouseX > button->x && mouseX < button->w+button->x && mouseY > button->y && mouseY < button->h+button->y)
    {
        return true;
    }
    return false;
}

void Instrument_properties::mouse(int x, int y)
{
    if (checkButton(x, y, &volume_sliderbox.slide))
    {
        if (x < volume_sliderbox.slide.w + volume_sliderbox.slide.x - volume_sliderbox.pos.w)
        {
            t->sample[t->s_pos].level = (x - volume_sliderbox.slide.x) / volume_sliderbox.pos.w;
        } else {
            t->sample[t->s_pos].level = 100;
        }
    }
    update();
}

void Instrument_properties::keyboard(SDL_Event *e)
{
    switch (e->key.keysym.sym)
    {
        case SDLK_RIGHT:
            if (SDL_GetModState() & KMOD_SHIFT)
            {
                t->sample_inc();
                update_instname();
                update();
            }
            break;
        case SDLK_LEFT:
            if (SDL_GetModState() & KMOD_SHIFT)
            {
                t->sample_dec();
                update_instname();
                update();
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
