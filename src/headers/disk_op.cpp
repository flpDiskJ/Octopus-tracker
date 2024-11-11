#include "disk_op.h"

DiskOp::DiskOp(Tracker *tracker, Sample_edit *sampler, AudioW *a, ModuleFormat *m, TTF_Font *f, Pallet *p)
{
    // get pointers from main function
    t = tracker;
    sampler_p = sampler;
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

    stem_track_b.r.x = back_b.r.x;
    stem_track_b.r.y = filter_b.r.y;
    stem_track_b.r.w = 4 * 14;
    stem_track_b.r.h = 28;

    surf = TTF_RenderText_Solid(font, "Stem", pallet->red);
    stem_track_b.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    set_paths_b.r.x = 1;
    set_paths_b.r.y = file_border.y + file_border.h - 28;
    set_paths_b.r.w = 6 * 14;
    set_paths_b.r.h = 28;

    surf = TTF_RenderText_Solid(font, "Set Paths", pallet->red);
    set_paths_b.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "Filter", pallet->blue);
    filter_b.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);
    update_default_note();
    load_default_paths();
}

DiskOp::~DiskOp()
{

}

bool DiskOp::save_default_paths()
{
    char file_path[INPUT_MAX];
    strcpy(file_path, getenv("HOME"));
    strncat(file_path, "/.octopus_paths", 16);
    FILE *fp = fopen(file_path, "wb");
    if (fp == NULL)
    {
        printf("Error saving default paths config!\n");
        return false;
    }

    // size of parent string 0, 1, 2. 2 bytes each
    Uint16 size = parent[0].length();
    fwrite(&size, 2, 1, fp);
    size = parent[1].length();
    fwrite(&size, 2, 1, fp);
    size = parent[2].length();
    fwrite(&size, 2, 1, fp);

    fwrite(parent[0].c_str(), 1, parent[0].length(), fp);
    fwrite(parent[1].c_str(), 1, parent[1].length(), fp);
    fwrite(parent[2].c_str(), 1, parent[2].length(), fp);

    fclose(fp);
    file_name_entry.text = "Paths set to default.";
    update_file_name();
    return true;
}

bool DiskOp::load_default_paths()
{
    char file_path[INPUT_MAX];
    strcpy(file_path, getenv("HOME"));
    strncat(file_path, "/.octopus_paths", 16);
    FILE *fp = fopen(file_path, "rb");
    if (fp == NULL)
    {
        printf("Error loading default paths config! (This can be ignored if you haven't set default paths)\n");
        return false;
    }

    // size of parent string 0, 1, 2. 2 bytes each
    Uint16 size[3];
    for (int i = 0; i < 3; i++)
    {
        fread(&size[i], 2, 1, fp);
    }

    char buff;
    for (int i = 0; i < 3; i++)
    {
        parent[i].clear();
        for (int c = 0; c < size[i]; c++)
        {
            fread(&buff, 1, 1, fp);
            parent[i] += buff;
        }
    }

    fclose(fp);
    return true;
}

bool DiskOp::file_exist(string filename)
{
    for (int i = 0; i < path_list_strings.size(); i++)
    {
        if (path_list_strings[i] == filename)
        {
            return true;
        }
    }
    return false;
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
    default_note_b.active = false;
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
            switch (key)
            {
                case '0':
                    octave_num = 0;
                    break;
                case '1':
                    octave_num = 1;
                    break;
                case '2':
                    octave_num = 2;
                    break;
                case '3':
                    octave_num = 3;
                    break;
                case '4':
                    octave_num = 4;
                    break;
                case '5':
                    octave_num = 5;
                    break;
                default:
                    octave_num = 3;
                    break;
            }
            t->default_pitch.octave = octave_num;
            update_default_note();
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

void DiskOp::fill_path_list()
{
    // clean up path vector
    path_list_strings.clear();
    path_list_types.clear();
    selected = -1;
    file_name_entry.text.clear();
    list_offset = 0;

    vector<string> dir_list;
    vector<string> file_list;

    d_op_path = opendir(parent[parent_index].c_str());

    while (dp = readdir(d_op_path))// if dp is null no more content to list
    {
        if (dp->d_name[0] != '.')
        {
            if (parent_index == SAMPLE_PATH && check_file_extension(".wav\0", dp->d_name))
            {
                file_list.push_back(dp->d_name);
            } else if (parent_index == MOD_PATH && check_file_extension(".octo\0", dp->d_name))
            {
                file_list.push_back(dp->d_name);
            } else if (parent_index == EXPORT_PATH && check_file_extension(".wav\0", dp->d_name))
            {
                file_list.push_back(dp->d_name);
            } else {
                if (dp->d_type == DT_DIR)
                {
                    dir_list.push_back(dp->d_name);
                }
            }
        }
    }

    sort(dir_list.begin(), dir_list.end());
    sort(file_list.begin(), file_list.end());

    for (int i = 0; i < dir_list.size(); i++)
    {
        path_list_strings.push_back(dir_list[i]);
        path_list_types.push_back(DT_DIR);
    }
    for (int i = 0; i < file_list.size(); i++)
    {
        path_list_strings.push_back(file_list[i]);
        path_list_types.push_back(DT_REG);
    }

    dir_list.clear();
    file_list.clear();

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
            item += ' ';
            item += path_list_strings[index];
            for (int f = path_list_strings[index].length(); f < 29; f++)
            {
                item += ' ';
            }
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
    string path;
    if (file_exist(file_name_entry.text) && !overwrite)
    {
        filename_store = file_name_entry.text;
        file_name_entry.text = "Overwrite? Y/N";
        update_file_name();
        overwrite_prompt = true;
        return;
    }
    if (overwrite)
    {
        path = parent[parent_index] + filename_store;
    } else {
        path = parent[parent_index] + file_name_entry.text;
    }
    switch (parent_index)
    {
        case MOD_PATH:
            module->save_module(path);
            fill_path_list();
            break;
        case EXPORT_PATH:
            module->export_wav(path);
            fill_path_list();
            break;
        default:
            break;
    }
    overwrite = false;
}

void DiskOp::load_button()
{
    if (file_name_entry.text.size() == 0)
    {
        return;
    }
    string path = parent[parent_index] + file_name_entry.text;
    switch (parent_index)
    {
        case MOD_PATH:
            if (module->load_module(path))
            {
                t->pos = 0;
                t->sq_pos = 0;
                t->b_pos = t->sequence[0];
                t->update_info();
                close();
            }
            break;
        case SAMPLE_PATH:
            if (t->load_inst(path, file_name_entry.text, t->s_pos, filter))
            {
                sampler_p->setup_new_sample();
            }
            break;
        default:
            break;
    }
}

void DiskOp::stem_button()
{
    string filepath = parent[parent_index] + file_name_entry.text;
    string filename;
    file_name_entry.text = "Please wait...";
    update_file_name();
    refresh();
    for (int c = 0; c < CHANNELS; c++)
    {
        for (int i = 0; i < CHANNELS; i++)
        {
            if (i == c)
            {
                t->mute[i] = false;
            } else {
                t->mute[i] = true;
            }
        }
        filename = filepath;
        filename += to_string(c);
        filename += ".wav";
        module->export_wav(filename);
        filename.clear();
        fill_path_list();
        update_list_textures();
        refresh();
    }
    for (int i = 0; i < CHANNELS; i++)
    {
        t->mute[i] = false;
    }
    file_name_entry.text = "Done!";
    update_file_name();
    refresh();
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

    SDL_RenderDrawRect(render, &set_paths_b.r);
    SDL_RenderCopy(render, set_paths_b.t, NULL, &set_paths_b.r);

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
    } else if (parent_index == EXPORT_PATH)
    {
        SDL_RenderDrawRect(render, &stem_track_b.r);
        SDL_RenderCopy(render, stem_track_b.t, NULL, &stem_track_b.r);
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
    } else if (checkButton(x, y, &set_paths_b.r))
    {
        save_default_paths();
    } else if (checkButton(x, y, &stem_track_b.r) && parent_index == EXPORT_PATH)
    {
        stem_button();
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
    if (SDL_GetModState() & KMOD_CTRL && e->key.keysym.sym == SDLK_x)
    {
        close();
        return;
    }
    switch (e->key.keysym.sym)
    {
        case SDLK_UP:
            if (default_note_b.active && default_pitch_index == 1)
            {
                t->default_pitch.key = '#';
                update_default_note();
            } else {
                if (list_offset > 14)
                {
                    list_offset -= 15;
                    update_list_textures();
                }
            }
            break;
        case SDLK_DOWN:
            if (default_note_b.active && default_pitch_index == 1)
            {
                t->default_pitch.key = '-';
                update_default_note();
            } else {
                if (list_offset + 15 < path_list_strings.size())
                {
                    list_offset += 15;
                    update_list_textures();
                }
            }
            break;
        case SDLK_RIGHT:
            if (SDL_GetModState() & KMOD_SHIFT)
            {
                t->sample_inc();
                sampler_p->setup_new_sample();
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
                sampler_p->setup_new_sample();
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
        case SDLK_F1:
            t->octave = 1; t->update_info();
            break;
        case SDLK_F2:
            t->octave = 2; t->update_info();
            break;
        case SDLK_F3:
            t->octave = 3; t->update_info();
            break;
        case SDLK_F4:
            t->octave = 4; t->update_info();
            break;
        case SDLK_F5:
            t->octave = 5; t->update_info();
            break;
        case SDLK_y:
            if (overwrite_prompt)
            {
                overwrite = true;
                overwrite_prompt = false;
                save_button();
            } else {
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
                    audioworks->play_sample(e, t->s_pos, 0);
                }
            }
            break;
        case SDLK_n:
            if (overwrite_prompt)
            {
                overwrite_prompt = false;
                overwrite = false;
                file_name_entry.text.clear();
                update_file_name();
            } else {
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
                    audioworks->play_sample(e, t->s_pos, 0);
                }
            }
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
                audioworks->play_sample(e, t->s_pos, 0);
            }
            break;
    }
    refresh();
}
