#include "disk_op.h"

DiskOp::DiskOp(Tracker *tracker, AudioW *a, ModuleFormat *m, TTF_Font *f, Pallet *p)
{
    // get pointers from main function
    t = tracker;
    audioworks = a;
    module = m;
    font = f;
    pallet = p;

    // read from config eventually
    parent[MOD_PATH] = "/home/";
    parent[SAMPLE_PATH] = "/home/";
    parent[EXPORT_PATH] = "/home/";

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

    // button intialization
    save_file.r.x = 20;
    save_file.r.y = 20;
    save_file.r.w = 14 * 4;
    save_file.r.h = 28;

    load_file.r.x = 20;
    load_file.r.y = 20 + save_file.r.h + 10;
    load_file.r.w = 14 * 4;
    load_file.r.h = 28;

    inst_b.r.x = 20;
    inst_b.r.y = load_file.r.y + load_file.r.h + 10;
    inst_b.r.w = 14 * 4;
    inst_b.r.h = 28;

    octo_b.r.x = 20;
    octo_b.r.y = inst_b.r.y + inst_b.r.h + 10;
    octo_b.r.w = 14 * 4;
    octo_b.r.h = 28;

    wave_b.r.x = 20;
    wave_b.r.y = octo_b.r.y + octo_b.r.h + 10;
    wave_b.r.w = 14 * 4;
    wave_b.r.h = 28;

    back_b.r.x = 20;
    back_b.r.y = wave_b.r.y + wave_b.r.h + 10;
    back_b.r.w = 14 * 4;
    back_b.r.h = 28;

    surf = TTF_RenderText_Solid(font, "Save", pallet->black);
    save_file.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "Load", pallet->black);
    load_file.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "Inst", pallet->black);
    inst_b.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "Octo", pallet->black);
    octo_b.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "Wave", pallet->black);
    wave_b.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "BACK", pallet->black);
    back_b.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    // file border init
    file_border.x = 20 + save_file.r.w + 10;
    file_border.y = 20;
    file_border.w = 400;
    file_border.h = 400;

    // init rectangles for file paths to be displayed
    for (int i = 0; i < FILE_LIST_SIZE; i++) {
        path_list_buttons[i].r.x = file_border.x;
        path_list_buttons[i].r.w = file_border.w;
        path_list_buttons[i].r.h = 23;
        path_list_buttons[i].r.y = file_border.y + 1 + (i * 25);
    }

    parent_path_display.r.x = 20 + wave_b.r.w + 10;
    parent_path_display.r.y = file_border.y + file_border.h + 5;
    parent_path_display.r.w = file_border.w;
    parent_path_display.r.h = 24;

    file_name_entry.r.x = parent_path_display.r.x;
    file_name_entry.r.y = parent_path_display.r.y + parent_path_display.r.h + 5;
    file_name_entry.r.w = 400;
    file_name_entry.r.h = 28;

    default_note_b.r.x = back_b.r.x + 6;
    default_note_b.r.y = back_b.r.y + back_b.r.h + 10;
    default_note_b.r.w = 3 * 14;
    default_note_b.r.h = 28;

    note_cursor.x = default_note_b.r.x;
    note_cursor.y = default_note_b.r.y+1;
    note_cursor.w = 14;
    note_cursor.h = default_note_b.r.h-2;

    filter_b.r.x = back_b.r.x;
    filter_b.r.y = default_note_b.r.y + default_note_b.r.h + 10;
    filter_b.r.w = 4 * 14;
    filter_b.r.h = 28;

    surf = TTF_RenderText_Solid(font, "Filter", pallet->blue);
    filter_b.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);
    update_default_note();
}

DiskOp::~DiskOp()
{

}

void DiskOp::update_default_note()
{
    string temp;
    temp += t->default_pitch.note;
    temp += t->default_pitch.key;
    temp += to_string(t->default_pitch.octave);
    if (default_note_b.t != NULL)
    {
        SDL_DestroyTexture(default_note_b.t);
    }
    surf = TTF_RenderText_Solid(font, temp.c_str(), pallet->black);
    default_note_b.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);
    temp.clear();
}

void DiskOp::set_default_note(const char* k, int index)
{
    char key = k[0];
    string temp;
    int octave_num;
    switch (index) {
        case 0:
            if (key == 'A' || key == 'B' || key == 'C' || key == 'D'
                || key == 'E' || key == 'F' || key == 'G')
                {
                    t->default_pitch.note = key;
                    update_default_note();
                }
            break;
        case 2:
            temp.clear();
            temp += key;
            octave_num = stoi(temp, 0, 10);
            if (octave_num >= 0 && octave_num < 6)
            {
                t->default_pitch.octave = octave_num;
                update_default_note();
            }
            break;
        default:
            break;
    }
}

bool DiskOp::check_file_extension(const char* ext, char* file)
{
    string file_name;
    string extension;
    file_name.clear();
    extension.clear();
    file_name += file;
    extension += ext;
    for (int x = file_name.length() - 1, y = extension.length() - 1; x > 0, y > 0; x--, y--)
    {
        if (file_name[x] != extension[y])
        {
            return false;
        }
    }
    return true;
}

void DiskOp::fill_path_list() // for now pass "/", when this is used later in the program it will be different
{
    // clean up path vector
    path_list_strings.clear();
    path_list_types.clear();
    selected = -1;
    file_name_entry.text.clear();
    list_offset = 0;

    d_op_path = opendir(parent[parent_index].c_str());

    while (dp = readdir(d_op_path))// if dp is null no more content to list
    {
        if (dp->d_name[0] != '.')
        {
            if (parent_index == SAMPLE_PATH && check_file_extension(".wav\0", dp->d_name))
            {
                path_list_strings.push_back(dp->d_name); // append path string to path list
                path_list_types.push_back(dp->d_type);
            } else if (parent_index == MOD_PATH && check_file_extension(".octo\0", dp->d_name))
            {
                path_list_strings.push_back(dp->d_name); // append path string to path list
                path_list_types.push_back(dp->d_type);
            } else if (parent_index == EXPORT_PATH && check_file_extension(".wav\0", dp->d_name))
            {
                path_list_strings.push_back(dp->d_name); // append path string to path list
                path_list_types.push_back(dp->d_type);
            } else {
                if (dp->d_type == DT_DIR)
                {
                    path_list_strings.push_back(dp->d_name); // append path string to path list
                    path_list_types.push_back(dp->d_type);
                }
            }
        }
    }

    closedir(d_op_path);
    set_parent_display();
    set_file_name();
    update_list_textures();
}

void DiskOp::set_parent_display()
{
    string path = parent[parent_index];
    for (int f = path.length(); f < 28; f++)
    {
        path += ' ';
    }
    if (parent_path_display.t != NULL)
    {
        SDL_DestroyTexture(parent_path_display.t);
    }
    surf = TTF_RenderText_Solid(font, path.c_str(), pallet->black);
    parent_path_display.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);
}

void DiskOp::set_file_name()
{
    if (selected >= 0 && selected < path_list_strings.size() && path_list_types[selected] == DT_REG)
    {
        file_name_entry.text = path_list_strings[selected];
    } else {
        file_name_entry.text.clear();
        if (path_list_types[selected] == DT_DIR)
        {
            parent[parent_index] += path_list_strings[selected];
            parent[parent_index] += os_slash;
            fill_path_list();
        }
    }
    if (file_name_entry.t != NULL)
    {
        SDL_DestroyTexture(file_name_entry.t);
    }
    string temp;
    for (int f = file_name_entry.text.length(); f < 25; f++)
    {
        temp += ' ';
    }
    temp += file_name_entry.text;
    temp += ' ';
    surf = TTF_RenderText_Solid(font, temp.c_str(), pallet->black);
    file_name_entry.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);
}

void DiskOp::update_file_name()
{
    if (file_name_entry.t != NULL)
    {
        SDL_DestroyTexture(file_name_entry.t);
    }
    string temp;
    for (int f = file_name_entry.text.length(); f < 25; f++)
    {
        temp += ' ';
    }
    temp += file_name_entry.text;
    temp += ' ';
    surf = TTF_RenderText_Solid(font, temp.c_str(), pallet->black);
    file_name_entry.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);
}

void DiskOp::update_list_textures()
{
    string item;
    for (int index = list_offset, list_p = 0; list_p < FILE_LIST_SIZE; index++, list_p++)
    {
        item.clear();
        if (index >= 0 && index < path_list_strings.size())
        {
            for (int f = path_list_strings[index].length(); f < 29; f++)
            {
                item += ' ';
            }
            item += path_list_strings[index];
            item += ' ';
        } else {
            item = "   ";
        }
        if (path_list_buttons[list_p].t != NULL)
        {
            SDL_DestroyTexture(path_list_buttons[list_p].t);
        }
        if (selected != -1 && selected == index)
        {
            surf = TTF_RenderText_Shaded(font, item.c_str(), pallet->black, pallet->white);
        } else if (path_list_types[index] == DT_DIR)
        {
            surf = TTF_RenderText_Solid(font, item.c_str(), pallet->red);
        } else {
            surf = TTF_RenderText_Solid(font, item.c_str(), pallet->black);
        }
        path_list_buttons[list_p].t = SDL_CreateTextureFromSurface(render, surf);
        SDL_FreeSurface(surf);
    }
}

void DiskOp::revert_dir()
{
    if (parent[parent_index].length() == 1)
    {
        return;
    }
    parent[parent_index].pop_back();
    while (parent[parent_index].back() != os_slash)
    {
        parent[parent_index].pop_back();
    }
    fill_path_list();
}

void DiskOp::save_button()
{
    string path = parent[parent_index] + file_name_entry.text;
    switch (parent_index)
    {
        case MOD_PATH:
            module->save_module(path);
            break;
        case EXPORT_PATH:
            module->export_wav(path);
            break;
        default:
            break;
    }
}

void DiskOp::load_button()
{
    string path = parent[parent_index] + file_name_entry.text;
    switch (parent_index)
    {
        case MOD_PATH:
            module->load_module(path);
            break;
        case SAMPLE_PATH:
            t->load_inst(path, file_name_entry.text, t->s_pos, filter);
            break;
        default:
            break;
    }
}

void DiskOp::de_init()
{
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}

void DiskOp::refresh()
{
    SDL_SetRenderDrawColor(render, pallet->bgd.r, pallet->bgd.g, pallet->bgd.b, 0xFF); // Background color
    SDL_RenderClear(render);
    SDL_SetRenderDrawColor(render, pallet->black.r, pallet->black.g, pallet->black.b, 0xFF); // Black

    // render buttons
    SDL_RenderDrawRect(render, &save_file.r);
    SDL_RenderCopy(render, save_file.t, NULL, &save_file.r);

    SDL_RenderDrawRect(render, &load_file.r);
    SDL_RenderCopy(render, load_file.t, NULL, &load_file.r);

    SDL_SetRenderDrawColor(render, pallet->white.r, pallet->white.g, pallet->white.b, 0xFF);
    switch (parent_index)
    {
        case SAMPLE_PATH:
            SDL_RenderFillRect(render, &inst_b.r);
            break;
        case MOD_PATH:
            SDL_RenderFillRect(render, &octo_b.r);
            break;
        case EXPORT_PATH:
            SDL_RenderFillRect(render, &wave_b.r);
            break;
        default:
            break;
    }
    SDL_SetRenderDrawColor(render, pallet->black.r, pallet->black.g, pallet->black.b, 0xFF);

    SDL_RenderDrawRect(render, &inst_b.r);
    SDL_RenderCopy(render, inst_b.t, NULL, &inst_b.r);

    SDL_RenderDrawRect(render, &octo_b.r);
    SDL_RenderCopy(render, octo_b.t, NULL, &octo_b.r);

    SDL_RenderDrawRect(render, &wave_b.r);
    SDL_RenderCopy(render, wave_b.t, NULL, &wave_b.r);

    SDL_RenderDrawRect(render, &back_b.r);
    SDL_RenderCopy(render, back_b.t, NULL, &back_b.r);

    SDL_RenderDrawRect(render, &file_border);

    if (parent_index == SAMPLE_PATH)
    {
        SDL_RenderDrawRect(render, &default_note_b.r);
        SDL_RenderCopy(render, default_note_b.t, NULL, &default_note_b.r);

        SDL_SetRenderDrawColor(render, pallet->red.r, pallet->red.g, pallet->red.b, 0xFF);

        if (default_note_b.active)
        {
            SDL_RenderDrawRect(render, &note_cursor);
        }

        if (filter)
        {
            SDL_RenderFillRect(render, &filter_b.r);
            SDL_SetRenderDrawColor(render, pallet->black.r, pallet->black.g, pallet->black.b, 0xFF);
        } else {
            SDL_SetRenderDrawColor(render, pallet->black.r, pallet->black.g, pallet->black.b, 0xFF);
            SDL_RenderDrawRect(render, &filter_b.r);
        }
        SDL_RenderCopy(render, filter_b.t, NULL, &filter_b.r);
    }

    // render subpaths
    for (int i = 0; i < FILE_LIST_SIZE; i++)
    {
        SDL_RenderCopy(render, path_list_buttons[i].t, NULL, &path_list_buttons[i].r);
    }

    SDL_RenderCopy(render, parent_path_display.t, NULL, &parent_path_display.r);

    if (file_name_entry.active)
    {
        SDL_SetRenderDrawColor(render, pallet->red.r, pallet->red.g, pallet->red.b, 0xFF);
    }
    SDL_RenderDrawRect(render, &file_name_entry.r);
    SDL_RenderCopy(render, file_name_entry.t, NULL, &file_name_entry.r);

    SDL_RenderPresent(render); // Present image to screen
}

void DiskOp::open()
{
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_SetWindowInputFocus(window);
    fill_path_list();
    update_list_textures();
    refresh();
}

void DiskOp::close()
{
    SDL_HideWindow(window);
}

Uint32 DiskOp::get_state()
{
    return SDL_GetWindowFlags(window);
}

bool DiskOp::checkButton(int mouseX, int mouseY, SDL_Rect *button)
{
    if (mouseX > button->x && mouseX < button->w+button->x && mouseY > button->y && mouseY < button->h+button->y)
    {
        return true;
    }
    return false;
}

void DiskOp::mouse_wheel(SDL_Event *e)
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    if (checkButton(x, y, &file_border))
    {
        if(e->wheel.y > 0) // scroll down
        {
            if (list_offset > 0)
            {
                list_offset--;
                update_list_textures();
                refresh();
            }
        }
        else if(e->wheel.y < 0) // scroll up
        {
            if (list_offset < path_list_strings.size() - 5)
            {
                list_offset++;
                update_list_textures();
                refresh();
            }
        }
    }
}

void DiskOp::mouse(int x, int y)
{
    if (checkButton(x, y, &inst_b.r))
    {
        parent_index = SAMPLE_PATH;
        list_offset = 0;
        fill_path_list();
    } else if (checkButton(x, y, &octo_b.r))
    {
        parent_index = MOD_PATH;
        default_note_b.active = false;
        list_offset = 0;
        fill_path_list();
    } else if (checkButton(x, y, &wave_b.r))
    {
        parent_index = EXPORT_PATH;
        default_note_b.active = false;
        list_offset = 0;
        fill_path_list();
    } else if (checkButton(x, y, &file_name_entry.r))
    {
        file_name_entry.active = !file_name_entry.active;
    } else if (checkButton(x, y, &back_b.r))
    {
        revert_dir();
    } else if (checkButton(x, y, &save_file.r))
    {
        save_button();
    } else if (checkButton(x, y, &load_file.r))
    {
        load_button();
    } else if (checkButton(x, y, &filter_b.r) && parent_index == SAMPLE_PATH)
    {
        filter = !filter;
    } else if (checkButton(x, y, &default_note_b.r) && parent_index == SAMPLE_PATH)
    {
        default_note_b.active = !default_note_b.active;
    } else
    {
        for (int i = 0; i < FILE_LIST_SIZE; i++)
        {
            if (checkButton(x, y, &path_list_buttons[i].r))
            {
                selected = i + list_offset;
                if (selected >= path_list_strings.size() || selected < 0)
                {
                    selected = -1;
                } else {
                    set_file_name();
                }
                update_list_textures();
            }
        }
    }
    refresh();
}

void DiskOp::keyboard(SDL_Event *e)
{
    switch (e->key.keysym.sym)
    {
        case SDLK_UP:
            if (default_note_b.active && default_pitch_index == 1)
            {
                t->default_pitch.key = '#';
                update_default_note();
            }
            break;
        case SDLK_DOWN:
            if (default_note_b.active && default_pitch_index == 1)
            {
                t->default_pitch.key = '-';
                update_default_note();
            }
            break;
        case SDLK_RIGHT:
            if (SDL_GetModState() & KMOD_SHIFT)
            {
                t->sample_inc();
            } else if (default_note_b.active)
            {
                if (default_pitch_index < 2)
                {
                    default_pitch_index++;
                    note_cursor.x = default_note_b.r.x + (14 * default_pitch_index);
                }
            }
            break;
        case SDLK_LEFT:
            if (SDL_GetModState() & KMOD_SHIFT)
            {
                t->sample_dec();
            } else if (default_note_b.active)
            {
                if (default_pitch_index > 0)
                {
                    default_pitch_index--;
                    note_cursor.x = default_note_b.r.x + (14 * default_pitch_index);
                }
            }
            break;
        case SDLK_DELETE:
        case SDLK_BACKSPACE:
            if (file_name_entry.text.length() > 0 && file_name_entry.active)
            {
                file_name_entry.text.pop_back();
                update_file_name();
            }
            break;
        case SDLK_SPACE:
            if (file_name_entry.active)
            {
                file_name_entry.text += ' ';
                update_file_name();
            }
            break;
        case SDLK_RETURN:
            file_name_entry.active = false;
            default_note_b.active = false;
            break;
        default:
            if (file_name_entry.active)
            {
                string keyname = SDL_GetKeyName(e->key.keysym.sym);
                file_name_entry.text += keymap.sdl_getText(keyname, SDL_GetModState() & KMOD_SHIFT);
                update_file_name();
            } else if (default_note_b.active)
            {
                set_default_note(SDL_GetKeyName(e->key.keysym.sym), default_pitch_index);
            } else if (!(SDL_GetModState() & KMOD_CTRL) && !(SDL_GetModState() & KMOD_SHIFT))
            {
                audioworks->play_sample(e, t->s_pos);
            }
            break;
    }
    refresh();
}
