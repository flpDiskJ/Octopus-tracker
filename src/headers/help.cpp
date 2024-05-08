#include "help.h"

HelpWindow::HelpWindow(TTF_Font *f, Pallet *p)
{
    font = f;
    pallet = p;

    window = SDL_CreateWindow("Help", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    800, 600, SDL_WINDOW_HIDDEN);
    if (window == NULL)
    {
        printf("Help window creation failed!\n");
        return;
    }

    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (render == NULL)
    {
        printf("Failed to create renderer for help window!\n");
    }

}

HelpWindow::~HelpWindow()
{

}

void HelpWindow::de_init()
{
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}

void HelpWindow::update()
{

}

void HelpWindow::refresh()
{
    SDL_SetRenderDrawColor(render, pallet->bgd.r, pallet->bgd.g, pallet->bgd.b, 0xFF); // Background color
    SDL_RenderClear(render);



    SDL_RenderPresent(render); // Present image to screen
}

void HelpWindow::open()
{
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_SetWindowInputFocus(window);
    refresh();
}

void HelpWindow::close()
{
    SDL_HideWindow(window);
}

Uint32 HelpWindow::get_state()
{
    return SDL_GetWindowFlags(window);
}

bool HelpWindow::checkButton(int mouseX, int mouseY, SDL_Rect *button)
{
    if (mouseX > button->x && mouseX < button->w+button->x && mouseY > button->y && mouseY < button->h+button->y)
    {
        return true;
    }
    return false;
}

void HelpWindow::mouse(int x, int y)
{

}

void HelpWindow::keyboard(SDL_Event *e)
{

}
