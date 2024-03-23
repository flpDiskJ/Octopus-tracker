#include "disk_opp.h"

DiskOp::DiskOp(tracker *tracker, Util *util, TTF_Font *f) {
    // get pointers from main func
    t = tracker;
    u = util;
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
        printf("Failed to create renderer for diskop window!\n");
    }
}

DiskOp::~DiskOp() {}

void DiskOp::de_init() {
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}

void DiskOp::refresh() {}

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

void DiskOp::mouse(int x, int y) {}

void DiskOp::keyboard(SDL_Event *e) {}