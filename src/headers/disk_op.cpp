#include "disk_op.h"

DiskOp::DiskOp(Tracker *tracker, TTF_Font *f, Pallet *p)
{
    // get pointers from main func
    t = tracker;
    font = f;
    pallet = p;

    // initialize window and renderer
    window = SDL_CreateWindow("Disk Op", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    500, 500, SDL_WINDOW_HIDDEN);
    if (window == NULL)
    {
        printf("Disk Op window creation failed!\n");
        return;
    }

    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (render == NULL)
    {
        printf("Failed to create renderer for disk op window!\n");
    }

    test.r.x = 20;
    test.r.y = 20;
    test.r.w = 100;
    test.r.h = 30;

    string title = "test_button";
    surf = TTF_RenderText_Solid(font, title.c_str(), pallet->green);
    test.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

}

DiskOp::~DiskOp()
{

}

void DiskOp::de_init() {
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}

void DiskOp::refresh()
{
    SDL_SetRenderDrawColor(render, pallet->bgd.r, pallet->bgd.g, pallet->bgd.b, 0xFF); // Background color
    SDL_RenderClear(render);
    SDL_SetRenderDrawColor(render, pallet->black.r, pallet->black.g, pallet->black.b, 0xFF); // Black

    // render stuff
    SDL_RenderDrawRect(render, &test.r);
    SDL_RenderCopy(render, test.t, NULL, &test.r);

    SDL_RenderPresent(render); // Present image to screen
}

void DiskOp::open() {
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_SetWindowInputFocus(window);
}

void DiskOp::close() {
    SDL_HideWindow(window);
}

Uint32 DiskOp::get_state() {
    return SDL_GetWindowFlags(window);
}

bool DiskOp::checkButton(int mouseX, int mouseY, SDL_Rect *button) {
    if (mouseX > button->x && mouseX < button->w+button->x && mouseY > button->y && mouseY < button->h+button->y)
    {
        return true;
    }
    return false;
}

void DiskOp::mouse(int x, int y)
{
    if (checkButton(x, y, &test.r))
    {
        // do something
    }
    refresh();
}

void DiskOp::keyboard(SDL_Event *e)
{

}
