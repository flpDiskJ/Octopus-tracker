#include "disk_op.h"

DiskOp::DiskOp(Tracker *tracker, ModuleFormat *m, TTF_Font *f, Pallet *p)
{
    // get pointers from main function
    t = tracker;
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
}

DiskOp::~DiskOp()
{

}

void DiskOp::fill_path_list() // for now pass "/", when this is used later in the program it will be different
{
    // clean up path vector
    path_list_strings.clear();
    path_list_types.clear();
    selected = -1;

    d_op_path = opendir(parent[parent_index].c_str());

    while (dp = readdir(d_op_path))// if dp is null no more content to list
    {
        if (dp->d_name[0] != '.')
        {
            path_list_strings.push_back(dp->d_name); // append path string to path list
            path_list_types.push_back(dp->d_type);
        }
    }

    closedir(d_op_path);
    set_parent_display();
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

    SDL_RenderDrawRect(render, &file_border);

    // render subpaths
    for (int i = 0; i < FILE_LIST_SIZE; i++)
    {
        SDL_RenderCopy(render, path_list_buttons[i].t, NULL, &path_list_buttons[i].r);
    }

    SDL_RenderCopy(render, parent_path_display.t, NULL, &parent_path_display.r);

    SDL_RenderPresent(render); // Present image to screen
}

void DiskOp::open()
{
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_SetWindowInputFocus(window);
    fill_path_list();
    update_list_textures();
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
            }
        }
        else if(e->wheel.y < 0) // scroll up
        {
            if (list_offset < path_list_strings.size() - 5)
            {
                list_offset++;
                update_list_textures();
            }
        }
    }
}

void DiskOp::mouse(int x, int y)
{
    if (checkButton(x, y, &inst_b.r))
    {
        parent_index = SAMPLE_PATH;
        fill_path_list();
    } else if (checkButton(x, y, &octo_b.r))
    {
        parent_index = MOD_PATH;
        fill_path_list();
    } else if (checkButton(x, y, &wave_b.r))
    {
        parent_index = EXPORT_PATH;
        fill_path_list();
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
                    // string concatination
                }
                update_list_textures();
            }
        }
    }
    refresh();
}

void DiskOp::keyboard(SDL_Event *e)
{

}
