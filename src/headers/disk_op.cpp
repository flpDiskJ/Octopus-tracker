#include "disk_op.h"

DiskOp::DiskOp(Tracker *tracker, TTF_Font *f, Pallet *p)
{
    // get pointers from main func
    t = tracker;
    font = f;
    pallet = p;

    // initialize window and renderer
    window = SDL_CreateWindow("Disk Op", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    750, 500, SDL_WINDOW_HIDDEN);
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

    // button intialization
    load_inst.r.x = 0;
    load_inst.r.y = 0;
    load_inst.r.w = 150;
    load_inst.r.h = 50;

    save_file.r.x = 175;
    save_file.r.y = 0;
    save_file.r.w = 150;
    save_file.r.h = 50;

    load_file.r.x = 350;
    load_file.r.y = 0;
    load_file.r.w = 150;
    load_file.r.h = 50;

    export_audio.r.x = 525;
    export_audio.r.y = 0;
    export_audio.r.w = 150;
    export_audio.r.h = 50;

    surf = TTF_RenderText_Solid(font, "Load Instrument", pallet->black);
    load_inst.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "Save File", pallet->black);
    save_file.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "Load File", pallet->black);
    load_file.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "Export Audio", pallet->black);
    export_audio.t = SDL_CreateTextureFromSurface(render, surf);
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

    // render buttons
    SDL_RenderDrawRect(render, &load_inst.r);
    SDL_RenderCopy(render, load_inst.t, NULL, &load_inst.r);

    SDL_RenderDrawRect(render, &save_file.r);
    SDL_RenderCopy(render, save_file.t, NULL, &save_file.r);

    SDL_RenderDrawRect(render, &load_file.r);
    SDL_RenderCopy(render, load_file.t, NULL, &load_file.r);

    SDL_RenderDrawRect(render, &export_audio.r);
    SDL_RenderCopy(render, export_audio.t, NULL, &export_audio.r);

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
    if (checkButton(x, y, &load_inst.r))
    {
        // do something
    }
    refresh();
}

void DiskOp::keyboard(SDL_Event *e)
{

}
