#include "util.h"

Util::Util(TTF_Font *gfont, Pallet *pallet)
{
    font = gfont;
    p = pallet;
    window == NULL;
    window = SDL_CreateWindow("Utility Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    500, 300, SDL_WINDOW_HIDDEN);
    if (window == NULL)
    {
        printf("Utility window creation failed!\n");
    }
    renderer = NULL;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        printf("Failed to create renderer for utility window!\n");
    }
}

Util::~Util()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void Util::open(string title)
{
    surf = TTF_RenderText_Solid(font, title.c_str(), p->black);
    name_tex = SDL_CreateTextureFromSurface(renderer, surf);
}
