#include "sample_edit.h"

Sample_edit::Sample_edit(Tracker *tracker, TTF_Font *f, Pallet *p)
{
    // get pointers from main func
    t = tracker;
    font = f;
    pallet = p;

    // initialize window and renderer
    window = SDL_CreateWindow("Sample_edit", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    800, 200, SDL_WINDOW_HIDDEN);
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

    update();
}

Sample_edit::~Sample_edit()
{

}

void Sample_edit::de_init()
{
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}

void Sample_edit::update()
{

}

void Sample_edit::refresh()
{
    SDL_SetRenderDrawColor(render, pallet->bgd.r, pallet->bgd.g, pallet->bgd.b, 0xFF); // Background color
    SDL_RenderClear(render);
    SDL_SetRenderDrawColor(render, pallet->black.r, pallet->black.g, pallet->black.b, 0xFF); // Black

    //sample naming

    //sample params

    SDL_RenderPresent(render); // Present image to screen
}

void Sample_edit::open()
{
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_SetWindowInputFocus(window);
    update();
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
