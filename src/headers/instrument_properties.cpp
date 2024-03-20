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

    inst_name_label.r.x = 20;
    inst_name_label.r.y = 20;
    inst_name_label.r.w = 224;
    inst_name_label.r.h = 30;

    surf = TTF_RenderText_Solid(font, "Instrument Name:", pallet->black);
    inst_name_label.t = SDL_CreateTextureFromSurface(render, surf);
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

    SDL_RenderCopy(render, inst_name_label.t, NULL, &inst_name_label.r);
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
