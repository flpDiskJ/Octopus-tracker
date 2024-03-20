#include "instrument_properties.h"

Instrument_properties::Instrument_properties(Tracker *tracker, TTF_Font *f, Pallet *p)
{
    // get pointers from main func
    t = tracker;
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

    samplename_box.x = 250; //this will need to be better implemented to show text of samplenames inside this box
    samplename_box.y = 20;
    samplename_box.w = 224;
    samplename_box.h = 30;

    inst_name_label.r.x = 20;
    inst_name_label.r.y = 20;
    inst_name_label.r.w = 224;
    inst_name_label.r.h = 30;
//transpose
    transpose_label.r.x = 243;
    transpose_label.r.y = 60;
    transpose_label.r.w = 126;
    transpose_label.r.h = 30;

    transpose_sliderbox.x = 250;
    transpose_sliderbox.y = 20;
    transpose_sliderbox.w = 224;
    transpose_sliderbox.h = 30;
//finetune
    finetune_label.r.x = 250;
    finetune_label.r.y = 120;
    finetune_label.r.w = 112;
    finetune_label.r.h = 30;

    finetune_sliderbox.x = 250;
    finetune_sliderbox.y = 20;
    finetune_sliderbox.w = 224;
    finetune_sliderbox.h = 30;
//volume
    volume_label.r.x = 264;
    volume_label.r.y = 180;
    volume_label.r.w = 84;
    volume_label.r.h = 30;

    volume_sliderbox.x = 250;
    volume_sliderbox.y = 20;
    volume_sliderbox.w = 224;
    volume_sliderbox.h = 30;

    surf = TTF_RenderText_Solid(font, "Instrument Name:", pallet->black);
    inst_name_label.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

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

void Instrument_properties::de_init()
{
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}

void Instrument_properties::update()
{

    refresh();
}

void Instrument_properties::refresh()
{
    SDL_SetRenderDrawColor(render, 128, 128, 128, 0xFF); // Background color
    SDL_RenderClear(render);
    SDL_SetRenderDrawColor(render, 0, 0, 0, 0xFF); // Black
//sample naming
    SDL_RenderDrawRect(render, &samplename_box);
    SDL_RenderCopy(render, inst_name_label.t, NULL, &inst_name_label.r);
//sample params
    SDL_RenderCopy(render, transpose_label.t, NULL, &transpose_label.r);
    SDL_RenderDrawRect(render, &transpose_sliderbox);
    SDL_RenderCopy(render, finetune_label.t,  NULL, &finetune_label.r);
    SDL_RenderDrawRect(render, &finetune_sliderbox);
    SDL_RenderCopy(render, volume_label.t,    NULL, &volume_label.r);
    SDL_RenderDrawRect(render, &volume_sliderbox);

    SDL_RenderPresent(render); // Present image to screen
}

void Instrument_properties::open()
{
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_SetWindowInputFocus(window);
    update();
}

void Instrument_properties::close()
{
    SDL_HideWindow(window);
}

void Instrument_properties::mouse(int x, int y)
{
    refresh();
}

Uint32 Instrument_properties::get_state()
{
    return SDL_GetWindowFlags(window);
}
