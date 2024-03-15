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

    list_box.x = 20;
    list_box.y = 20;
    list_box.w = 460;
    list_box.h = 360;

    insert_btn.r.x = 40;
    insert_btn.r.y = 400;
    insert_btn.r.w = 168;
    insert_btn.r.h = 30;

    append_btn.r.x = 40;
    append_btn.r.y = 450;
    append_btn.r.w = 168;
    append_btn.r.h = 30;

    delete_btn.r.x = 292;
    delete_btn.r.y = 450;
    delete_btn.r.w = 168;
    delete_btn.r.h = 30;

    for (int index = 0; index < list_entries; index++)
    {
        list[index].r.w = list_box.w;
        list[index].r.h = 24;
        list[index].r.x = list_box.x;
        list[index].r.y = list_box.y + (index * list[index].r.h);
        list[index].t = NULL;
    }

    surf = TTF_RenderText_Solid(font, "Insert Block", pallet->green);
    insert_btn.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "Append Block", pallet->blue);
    append_btn.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "Delete Block", pallet->red);
    delete_btn.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    update_list();
}

Sequencer::~Sequencer()
{

}

bool Sequencer::set_seq_pos(int entry)
{
    int pos =  scroll_pos + (entry - (list_entries/2));
    if (pos >= 0 && pos < t->sequence_len)
    {
        t->sq_pos = pos;
        t->update_info();
        return true;
    }
    return false;
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

    SDL_RenderDrawRect(render, &list_box);
    SDL_RenderDrawRect(render, &insert_btn.r);
    SDL_RenderCopy(render, insert_btn.t, NULL, &insert_btn.r);
    SDL_RenderDrawRect(render, &append_btn.r);
    SDL_RenderCopy(render, append_btn.t, NULL, &append_btn.r);
    SDL_RenderDrawRect(render, &delete_btn.r);
    SDL_RenderCopy(render, delete_btn.t, NULL, &delete_btn.r);

    for (int i = 0; i < list_entries; i++)
    {
        SDL_RenderCopy(render, list[i].t, NULL, &list[i].r);
    }

    SDL_SetRenderDrawColor(render, 0, 0, 128, 0xFF); // Blue
    if (selected)
    {
        SDL_RenderDrawRect(render, &list[cursor].r);
    }

    SDL_RenderPresent(render); // Present image to screen
}

void Sequencer::update_list()
{
    string text;
    for (int index = 0, p = scroll_pos - (list_entries / 2); index < list_entries; index++, p++)
    {
        if (p >= 0 && p < t->sequence_len)
        {
            text += to_string(p);
            text += "> Blk: ";
            text += to_string(t->sequence[p]);
            text += " : ";
            text += t->block[t->sequence[p]].name;
            for (int fill = strlen(text.c_str()); fill < 30; fill++)
            {
                text += " ";
            }
        } else {
            text = "   ";
        }
        if (p == t->sq_pos)
        {
            surf = TTF_RenderText_Solid(font, text.c_str(), pallet->blue);
        } else{
            surf = TTF_RenderText_Solid(font, text.c_str(), pallet->black);
        }
        text.clear();
        if (list[index].t != NULL)
        {
            SDL_DestroyTexture(list[index].t);
        }
        list[index].t = SDL_CreateTextureFromSurface(render, surf);
        SDL_FreeSurface(surf);
    }
    refresh();
}

void Sequencer::open()
{
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_SetWindowInputFocus(window);
    cursor = list_entries/2;
    scroll_pos = t->sq_pos;
    selected = true;
    update_list();
}

void Sequencer::close()
{
    SDL_HideWindow(window);
}

Uint32 Sequencer::get_state()
{
    return SDL_GetWindowFlags(window);
}

bool Sequencer::checkButton(int mouseX, int mouseY, SDL_Rect *button)
{
    if (mouseX > button->x && mouseX < button->w+button->x && mouseY > button->y && mouseY < button->h+button->y)
    {
        return true;
    }
    return false;
}

void Sequencer::mouse(int x, int y)
{
    if (checkButton(x, y, &append_btn.r))
    {
        t->sequencer(APPEND_SEQ);
    } else if (checkButton(x, y, &insert_btn.r))
    {
        if (selected){
            t->sequencer(INSERT_SEQ);
        }
    } else if (checkButton(x, y, &delete_btn.r))
    {
        if (selected){
            t->sequencer(DELETE_SEQ);
        }
    } else if (checkButton(x, y, &list_box))
    {
        for (int i = 0; i < list_entries; i++)
        {
            if (checkButton(x, y, &list[i].r))
            {
                if (set_seq_pos(i))
                {
                    selected = true;
                    cursor = i;
                }
            }
        }
    }
    update_list();
}

void Sequencer::keyboard(SDL_Event *e)
{
    switch (e->key.keysym.sym)
    {
        case SDLK_UP:
            if (scroll_pos > 0)
            {
                scroll_pos--;
            } else {
                scroll_pos = t->sequence_len-1;
            }
            selected = false;
            update_list();
            break;
        case SDLK_DOWN:
            if (scroll_pos < t->sequence_len-1)
            {
                scroll_pos++;
            } else {
                scroll_pos = 0;
            }
            selected = false;
            update_list();
            break;
        case SDLK_RIGHT:
            if (selected)
            {
                t->sequencer(INC_SEQ);
                update_list();
            }
            break;
        case SDLK_LEFT:
            if (selected)
            {
                t->sequencer(DEC_SEQ);
                update_list();
            }
            break;
        default: break;
    }
}