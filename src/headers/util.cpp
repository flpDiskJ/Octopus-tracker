#include "util.h"

Util::Util(Tracker *t, TTF_Font *gfont, Pallet *pallet)
{
    tracker = t;
    font = gfont; // pass in font from main
    p = pallet; // pass in pallet from main
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

    name_rec.x = 100;
    name_rec.y = 0;
    name_rec.w = 100;
    name_rec.h = 20;

    block_list.x = 20;
    block_list.y = 20;
    block_list.w = 460;
    block_list.h = 200;
    block_name.x = 200;
    block_name.y = 220;
    block_name.w = 100;
    block_name.h = 20;
    cursor.x = 20;
    cursor.y = 20;
    cursor.w = 460;
    cursor.h = 20;

    surf = TTF_RenderText_Solid(font, "Yes", p->green);
    button.yes_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    surf = TTF_RenderText_Solid(font, "No", p->red);
    button.no_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    surf = TTF_RenderText_Solid(font, "Save", p->green);
    button.save_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    button.yes.x = 120;
    button.yes.y = 150;
    button.yes.w = 60;
    button.yes.h = 20;
    button.no.x = 320;
    button.no.y = 150;
    button.no.w = 40;
    button.no.h = 20;
    button.save.x = 226;
    button.save.y = 275;
    button.save.w = 48;
    button.save.h = 20;

    for (int i = 0; i < 10; i++)
    {
        list_index[i].x = 20;
        list_index[i].y = 20 + (i * 20);
        list_index[i].w = 460;
        list_index[i].h = 20;
        list_index_tex[i] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2); // throwaway
    }
}

Util::~Util()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void Util::open(string title, int m)
{
    if (windowopen)
    {
        close();
    }
    mode = m;
    surf = TTF_RenderText_Solid(font, title.c_str(), p->black);
    name_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    name_rec.w = strlen(title.c_str()) * 12;
    name_rec.x = (500 / 2) - (name_rec.w / 2);
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_SetWindowInputFocus(window);
    if (mode > 0)
    {
        update();
    }
    render();
    windowopen = true;
}

void Util::close()
{
    windowopen = false;
    SDL_DestroyTexture(name_tex);
    SDL_HideWindow(window);
}

void Util::update()
{
    string text;
    switch (mode) {
        case 1:
            for (int i = 0; i < 10; i++)
            {
                if (pos + i < tracker->total_blocks)
                {
                    text.clear();
                    text = "Block ";
                    text += to_string(tracker->b_pos + pos + i);
                    text += ": ";
                    text += tracker->block[tracker->b_pos].name;
                    for (int f = strlen(text.c_str()); f < 25; f++)
                    {
                        text += ' ';
                    }
                    surf = TTF_RenderText_Solid(font, text.c_str(), p->black);
                } else{
                    surf = TTF_RenderText_Solid(font, " ", p->black);
                }
                SDL_DestroyTexture(list_index_tex[i]);
                list_index_tex[i] = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_FreeSurface(surf);
            }
            break;
        default:
            break;
    }
}

void Util::render()
{
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 0xFF); // Background color
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF); // Black

    SDL_RenderCopy(renderer, name_tex, NULL, &name_rec);

    switch (mode)
    {
        case 0:
            SDL_RenderCopy(renderer, button.yes_tex, NULL, &button.yes);
            SDL_RenderCopy(renderer, button.no_tex, NULL, &button.no);
            SDL_RenderDrawRect(renderer, &button.yes);
            SDL_RenderDrawRect(renderer, &button.no);
            break;
        case 1:
            SDL_RenderCopy(renderer, button.save_tex, NULL, &button.save);
            SDL_RenderDrawRect(renderer, &button.save);
            SDL_RenderDrawRect(renderer, &block_list);
            for (int i = 0; i < 10; i++)
            {
                SDL_RenderCopy(renderer, list_index_tex[i], NULL, &list_index[i]);
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 180, 0xFF); // Blue
            SDL_RenderDrawRect(renderer, &cursor);
            break;
        default:
            break;
    }

    SDL_RenderPresent(renderer); // Present image to screen
}

bool Util::checkButton(int mouseX, int mouseY, SDL_Rect *button)
{
    if (mouseX > button->x && mouseX < button->w+button->x && mouseY > button->y && mouseY < button->h+button->y)
    {
        return true;
    }
    return false;
}

void Util::mouse(int x, int y)
{
    switch (mode)
    {
        case 0:
            if (checkButton(x, y, &button.yes))
            {
                command = 'Q';
            } else if (checkButton(x, y, &button.no))
            {
                command = 'q';
            }
            break;
        case 1:
            if (checkButton(x, y, &button.save))
            {
                command = 'q';
            }
            break;
        default:
            break;
    }
}

void Util::input(SDL_Event *e)
{
    switch (mode)
    {
        case 0:
            break;
        case 1:
            break;
        default:
            break;
    }
}
