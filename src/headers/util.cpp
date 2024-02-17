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
    block_name.y = 222;
    block_name.w = 100;
    block_name.h = 20;
    block_speed.x = 200;
    block_speed.y = 244;
    block_speed.w = 100;
    block_speed.h = 20;
    block_len.x = 200;
    block_len.y = 266;
    block_len.w = 100;
    block_len.h = 20;
    cursor.x = 20;
    cursor.y = 20;
    cursor.w = 460;
    cursor.h = 20;

    track_name.x = 200;
    track_name.y = 40;
    track_name.w = 100;
    track_name.h = 30;
    track_artist.x = 200;
    track_artist.y = 80;
    track_artist.w = 100;
    track_artist.h = 30;
    track_date.x = 200;
    track_date.y = 120;
    track_date.w = 100;
    track_date.h = 30;
    bpm_display.x = 200;
    bpm_display.y = 160;
    bpm_display.w = 100;
    bpm_display.h = 30;

    surf = TTF_RenderText_Solid(font, "Yes", p->green);
    button.yes_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    surf = TTF_RenderText_Solid(font, "No", p->red);
    button.no_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    surf = TTF_RenderText_Solid(font, "Delete", p->red);
    button.del_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    button.yes.x = 120;
    button.yes.y = 150;
    button.yes.w = 60;
    button.yes.h = 20;
    button.no.x = 320;
    button.no.y = 150;
    button.no.w = 40;
    button.no.h = 20;
    button.del.x = 405;
    button.del.y = 250;
    button.del.w = 48;
    button.del.h = 20;

    block_name_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2); // throwaway
    block_speed_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2); // throwaway
    block_len_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2); // throwaway
    track_name_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2); // throwaway
    bpm_display_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2); // throwaway
    track_artist_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2); // throwaway
    track_date_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2); // throwaway

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

}

void Util::close_all()
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
    command = 'n';
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
        setEntry();
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
            block_name.w = 20 * strlen(tracker->block[tracker->b_pos].name.c_str());
            if (block_name.w < 20) {block_name.w = 20;}
            block_name.x = 250 - (block_name.w / 2);
            surf = TTF_RenderText_Solid(font, tracker->block[tracker->b_pos].name.c_str(), p->black);
            SDL_DestroyTexture(block_name_tex);
            block_name_tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);

            text = "Speed: ";
            text += speed_entry;
            block_speed.w = 20 * strlen(text.c_str());
            block_speed.x = 250 - (block_speed.w / 2);
            surf = TTF_RenderText_Solid(font, text.c_str(), p->black);
            SDL_DestroyTexture(block_speed_tex);
            block_speed_tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);

            text.clear();
            text = "Length: ";
            text += len_entry;
            block_len.w = 20 * strlen(text.c_str());
            block_len.x = 250 - (block_len.w / 2);
            surf = TTF_RenderText_Solid(font, text.c_str(), p->black);
            SDL_DestroyTexture(block_len_tex);
            block_len_tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);


            for (int i = 0; i < 10; i++)
            {
                if (pos + i == tracker->b_pos)
                {
                    cursor.y = 20 + (20 * i);
                }
                if (pos + i >= 0 && pos + i < tracker->total_blocks)
                {
                    text.clear();
                    text = "Block ";
                    text += to_string(pos + i);
                    text += ": ";
                    text += tracker->block[pos + i].name;
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
        case 2:
            text.clear();
            text = "Title: ";
            text += tracker->track_name;
            track_name.w = 16 * strlen(text.c_str());
            track_name.x = 250 - (track_name.w / 2);
            surf = TTF_RenderText_Solid(font, text.c_str(), p->black);
            SDL_DestroyTexture(track_name_tex);
            track_name_tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);

            text.clear();
            text = "Artist: ";
            text += tracker->track_artist;
            track_artist.w = 16 * strlen(text.c_str());
            track_artist.x = 250 - (track_artist.w / 2);
            surf = TTF_RenderText_Solid(font, text.c_str(), p->black);
            SDL_DestroyTexture(track_artist_tex);
            track_artist_tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);

            text.clear();
            text = "Date: ";
            text += tracker->track_date;
            track_date.w = 16 * strlen(text.c_str());
            track_date.x = 250 - (track_date.w / 2);
            surf = TTF_RenderText_Solid(font, text.c_str(), p->black);
            SDL_DestroyTexture(track_date_tex);
            track_date_tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);

            text.clear();
            text = "Master Tempo: ";
            text += bpm_entry;
            bpm_display.w = 16 * strlen(text.c_str());
            bpm_display.x = 250 - (bpm_display.w / 2);
            surf = TTF_RenderText_Solid(font, text.c_str(), p->black);
            SDL_DestroyTexture(bpm_display_tex);
            bpm_display_tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
            break;
        default:
            break;
    }
    render();
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
            SDL_RenderCopy(renderer, button.del_tex, NULL, &button.del);
            SDL_RenderCopy(renderer, block_name_tex, NULL, &block_name);
            SDL_RenderCopy(renderer, block_speed_tex, NULL, &block_speed);
            SDL_RenderCopy(renderer, block_len_tex, NULL, &block_len);
            SDL_RenderDrawRect(renderer, &button.del);
            SDL_RenderDrawRect(renderer, &block_list);
            SDL_RenderDrawRect(renderer, &block_speed);
            SDL_RenderDrawRect(renderer, &block_len);
            for (int i = 0; i < 10; i++)
            {
                SDL_RenderCopy(renderer, list_index_tex[i], NULL, &list_index[i]);
            }
            if (text_mode)
            {
                SDL_SetRenderDrawColor(renderer, 180, 0, 0, 0xFF); // Red
            }
            SDL_RenderDrawRect(renderer, &block_name);
            if (tracker->b_pos >= pos && tracker->b_pos < pos + 10)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 180, 0xFF); // Blue
                SDL_RenderDrawRect(renderer, &cursor);
            }
            if (speed_mode)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 180, 0xFF); // Blue
                SDL_RenderDrawRect(renderer, &block_speed);
            }
            if (len_mode)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 180, 0xFF); // Blue
                SDL_RenderDrawRect(renderer, &block_len);
            }
            break;
        case 2:
            SDL_RenderCopy(renderer, track_name_tex, NULL, &track_name);
            SDL_RenderCopy(renderer, track_artist_tex, NULL, &track_artist);
            SDL_RenderCopy(renderer, bpm_display_tex, NULL, &bpm_display);
            SDL_RenderCopy(renderer, track_date_tex, NULL, &track_date);
            if (bpm_mode)
            {
                SDL_RenderDrawRect(renderer, &bpm_display);
            }
            if (name_mode)
            {
                SDL_RenderDrawRect(renderer, &track_name);
            }
            if (artist_mode)
            {
                SDL_RenderDrawRect(renderer, &track_artist);
            }
            if (date_mode)
            {
                SDL_RenderDrawRect(renderer, &track_date);
            }
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

void Util::setEntry()
{
    speed_entry = to_string(tracker->block[tracker->b_pos].speed);
    len_entry = to_string(tracker->block[tracker->b_pos].length);
    bpm_entry = to_string(tracker->master_tempo);
    tracker->set_timing_delay();
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
            if (checkButton(x, y, &button.del))
            {
                tracker->delete_block(tracker->b_pos);
                setEntry();
                update();
            }
            if (checkButton(x, y, &block_name))
            {
                text_mode = true;
                update();
            }
            if (checkButton(x, y, &block_speed))
            {
                speed_mode = true;
                update();
            }
            if (checkButton(x, y, &block_len))
            {
                len_mode = true;
                update();
            }
            for (int i = 0; i < 10; i++)
            {
                if (checkButton(x, y, &list_index[i]))
                {
                    if (pos + i >= 0 && pos + i < tracker->total_blocks)
                    {
                        tracker->b_pos = pos + i;
                        tracker->update_info();
                        setEntry();
                        update();
                    }
                }
            }
            break;
        case 2:
            if (checkButton(x, y, &track_name))
            {
                name_mode = true;
            }
            if (checkButton(x, y, &track_artist))
            {
                artist_mode = true;
            }
            if (checkButton(x, y, &track_date))
            {
                date_mode = true;
            }
            if (checkButton(x, y, &bpm_display))
            {
                bpm_mode = true;
            }
            update();
            break;
        default:
            break;
    }
}

void Util::input(SDL_Event *e)
{
    string key;
    switch (mode)
    {
        case 0:
            break;
        case 1:
            if (e->key.keysym.sym == SDLK_DOWN)
            {
                if (SDL_GetModState() & KMOD_SHIFT) {tracker->block_inc();}
                else if (pos < tracker->total_blocks-5) {pos++;}
                update();
            }
            else if (e->key.keysym.sym == SDLK_UP)
            {
                if (SDL_GetModState() & KMOD_SHIFT) {tracker->block_dec();}
                else if (pos > -5) {pos--;}
                update();
            }
            if (text_mode || speed_mode || len_mode)
            {
                switch (e->key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                    case SDLK_RETURN:
                        text_mode = false;
                        if (speed_mode)
                        {
                            speed_mode = false;
                            if (speed_entry != "0" && strlen(speed_entry.c_str()) > 0)
                            {
                                tracker->block[tracker->b_pos].speed = stoi(speed_entry, 0, 10); // convert string to int
                            } else {
                                tracker->block[tracker->b_pos].speed = 1;
                            }
                            setEntry();
                        }
                        else if (len_mode)
                        {
                            len_mode = false;
                            if (len_entry != "0" && strlen(len_entry.c_str()) > 0)
                            {
                                tracker->realloc_block(stoi(len_entry, 0, 10));
                            } else {
                                tracker->realloc_block(1);
                            }
                            setEntry();
                        }
                        break;
                    case SDLK_DELETE:
                    case SDLK_BACKSPACE:
                        if (text_mode && strlen(tracker->block[tracker->b_pos].name.c_str()) > 0)
                        {
                            tracker->block[tracker->b_pos].name.pop_back();
                        } else if (speed_mode && strlen(speed_entry.c_str()) > 0)
                        {
                            speed_entry.pop_back();
                        } else if (len_mode && strlen(len_entry.c_str()) > 0)
                        {
                            len_entry.pop_back();
                        }
                        break;
                    default:
                        if (text_mode && strlen(tracker->block[tracker->b_pos].name.c_str()) < 16)
                        {
                            key = SDL_GetKeyName(e->key.keysym.sym);
                            if (strlen(key.c_str()) == 1)
                            {
                                tracker->block[tracker->b_pos].name += key;
                            } else {
                                if (key == "Space")
                                {
                                    tracker->block[tracker->b_pos].name += ' ';
                                }
                            }
                        } else if (speed_mode)
                        {
                            key = SDL_GetKeyName(e->key.keysym.sym);
                            if (strlen(speed_entry.c_str()) < 2)
                            {
                                if (key == "0" || key == "1" || key == "2" || key == "3" || key == "4" || key == "5"
                                || key == "6" || key == "7" || key == "8" || key == "9")
                                {
                                    speed_entry += key;
                                }
                            }
                        } else if (len_mode)
                        {
                            key = SDL_GetKeyName(e->key.keysym.sym);
                            if (strlen(len_entry.c_str()) < 4)
                            {
                                if (key == "0" || key == "1" || key == "2" || key == "3" || key == "4" || key == "5"
                                || key == "6" || key == "7" || key == "8" || key == "9")
                                {
                                    len_entry += key;
                                }
                            }
                        }
                        break;
                }
                update();
            }
            break;
        case 2:
            switch (e->key.keysym.sym)
            {
                case SDLK_ESCAPE:
                case SDLK_RETURN:
                    name_mode = false; artist_mode = false; date_mode = false;
                    if (bpm_mode)
                    {
                        bpm_mode = false;
                        if (bpm_entry != "0" && strlen(bpm_entry.c_str()) > 0)
                        {
                            tracker->master_tempo = stoi(bpm_entry, 0, 10); // convert string to int
                        } else {
                            tracker->master_tempo = 1;
                        }
                        setEntry();
                    }
                    break;
                case SDLK_DELETE:
                case SDLK_BACKSPACE:
                    if (name_mode && strlen(tracker->track_name.c_str()) > 0)
                    {
                        tracker->track_name.pop_back();
                    } else if (artist_mode && strlen(tracker->track_artist.c_str()) > 0)
                    {
                        tracker->track_artist.pop_back();
                    } else if (date_mode && strlen(tracker->track_date.c_str()) > 0)
                    {
                        tracker->track_date.pop_back();
                    } else if (bpm_mode && strlen(bpm_entry.c_str()) > 0)
                    {
                        bpm_entry.pop_back();
                    }
                    break;
                default:
                    if (name_mode && strlen(tracker->track_name.c_str()) < 20)
                    {
                        key = SDL_GetKeyName(e->key.keysym.sym);
                        if (strlen(key.c_str()) == 1)
                        {
                            tracker->track_name += key;
                        } else {
                            if (key == "Space")
                            {
                                tracker->track_name += ' ';
                            }
                        }
                    } else if (artist_mode && strlen(tracker->track_artist.c_str()) < 20)
                    {
                        key = SDL_GetKeyName(e->key.keysym.sym);
                        if (strlen(key.c_str()) == 1)
                        {
                            tracker->track_artist += key;
                        } else {
                            if (key == "Space")
                            {
                                tracker->track_artist += ' ';
                            }
                        }
                    } else if (date_mode && strlen(tracker->track_date.c_str()) < 20)
                    {
                        key = SDL_GetKeyName(e->key.keysym.sym);
                        if (strlen(key.c_str()) == 1)
                        {
                            tracker->track_date += key;
                        } else {
                            if (key == "Space")
                            {
                                tracker->track_date += ' ';
                            }
                        }
                    } else if (bpm_mode)
                    {
                        key = SDL_GetKeyName(e->key.keysym.sym);
                        if (strlen(bpm_entry.c_str()) < 3)
                        {
                            if (key == "0" || key == "1" || key == "2" || key == "3" || key == "4" || key == "5"
                            || key == "6" || key == "7" || key == "8" || key == "9")
                            {
                                bpm_entry += key;
                            }
                        }
                    }
                    break;
            }
            update();
            break;
        default:
            break;
    }
}
