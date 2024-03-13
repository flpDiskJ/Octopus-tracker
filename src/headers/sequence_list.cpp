#include "sequence_list.h"

Sequencer::Sequencer(Tracker *tracker, TTF_Font *f, Pallet *p)
{
    // get pointers from main func
    t = tracker;
    font = f;
    pallet = p;

    // initialize window and renderer
    window = SDL_CreateWindow("Track Sequence", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    500, 500, SDL_WINDOW_HIDDEN);
    if (window == NULL)
    {
        printf("Sequencer window creation failed!\n");
        return;
    }

    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (render == NULL)
    {
        printf("Failed to create renderer for sequence window!\n");
    }


    refresh();
}

Sequencer::~Sequencer()
{

}

void Sequencer::de_init()
{
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}

void Sequencer::refresh()
{
    SDL_SetRenderDrawColor(render, 128, 128, 128, 0xFF); // Background color
    SDL_RenderClear(render);
    SDL_SetRenderDrawColor(render, 0, 0, 0, 0xFF); // Black

    SDL_RenderPresent(render); // Present image to screen
}

void Sequencer::open()
{
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_SetWindowInputFocus(window);
}

void Sequencer::close()
{
    SDL_HideWindow(window);
}

Uint32 Sequencer::get_state()
{
    return SDL_GetWindowFlags(window);
}

void Sequencer::keyboard(SDL_Event *e)
{

}
