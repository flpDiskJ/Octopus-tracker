#include "disk_op.h"

DiskOp::DiskOp(Tracker *tracker, ModuleFormat *m, TTF_Font *f, Pallet *p)
{
    // get pointers from main function
    t = tracker;
    module = m;
    font = f;
    pallet = p;

    // read from config eventually
    parent[0] = "/";
    parent[1] = "/home";
    parent[2] = "/etc";

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
    save_file.r.x = 195;
    save_file.r.y = 10;
    save_file.r.w = 150;
    save_file.r.h = 30;

    load_file.r.x = 370;
    load_file.r.y = 10;
    load_file.r.w = 150;
    load_file.r.h = 30;

    export_audio.r.x = 545;
    export_audio.r.y = 10;
    export_audio.r.w = 150;
    export_audio.r.h = 30;

    surf = TTF_RenderText_Solid(font, "Save File", pallet->black);
    save_file.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "Load File", pallet->black);
    load_file.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "Export Audio", pallet->black);
    export_audio.t = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    // file border init
    file_border.x = 195;
    file_border.y = 55;
    file_border.w = 515;
    file_border.h = 380;

    // init rectangles for file paths to be displayed
    for (int i = 0; i < FILE_LIST_SIZE; i++) {
        path_list_buttons[i].r.x = 200;
        path_list_buttons[i].r.y = (file_border.y + 5) + (i * 36);
        path_list_buttons[i].r.w = 505;
        path_list_buttons[i].r.h = 36;
        path_list_buttons[i].t = NULL;
    }
}

DiskOp::~DiskOp()
{

}

string DiskOp::cat_path(string path1, string path2)
{
    return path1 + path2;
}

void DiskOp::fill_path_list(string sub_path) // for now pass "/", when this is used later in the program it will be different
{
    // clean up path vector
    path_list_strings.clear();

    // TODO Immediately: add the logic to setup the default path and then update the absolute path to files based off of
    // which subfolder the user wishes to browse.

    // Currently the cat_path() method isn't being utilized in this method but once the ability to navigate the diskop menu
    // is implemented it will be used to update the absolute path.

    d_op_path = opendir(cat_path(parent[0], sub_path).c_str());
    while (dp = readdir(d_op_path))// if dp is null no more content to list
    {
        path_list_strings.push_back(dp->d_name); // append path string to path list
    }
    closedir(d_op_path);
}

void DiskOp::update_list_textures()
{
    for (int i = 0; i < FILE_LIST_SIZE; i++) {
        surf = TTF_RenderText_Solid(font, path_list_strings[i].c_str(), pallet->black); // do i need to use c_str?
        path_list_buttons[i].t = SDL_CreateTextureFromSurface(render, surf);
        SDL_FreeSurface(surf);
    }
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
    SDL_RenderDrawRect(render, &save_file.r);
    SDL_RenderCopy(render, save_file.t, NULL, &save_file.r);

    SDL_RenderDrawRect(render, &load_file.r);
    SDL_RenderCopy(render, load_file.t, NULL, &load_file.r);

    SDL_RenderDrawRect(render, &export_audio.r);
    SDL_RenderCopy(render, export_audio.t, NULL, &export_audio.r);

    SDL_RenderDrawRect(render, &file_border);

    // render subpaths
    for (int i = 0; i < FILE_LIST_SIZE; i++) {
        SDL_RenderCopy(render, path_list_buttons[i].t, NULL, &path_list_buttons[i].r);
    }

    SDL_RenderPresent(render); // Present image to screen
}

void DiskOp::open()
{
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_SetWindowInputFocus(window);
    fill_path_list("");
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

bool DiskOp::checkButton(int mouseX, int mouseY, SDL_Rect *button) {
    if (mouseX > button->x && mouseX < button->w+button->x && mouseY > button->y && mouseY < button->h+button->y)
    {
        return true;
    }
    return false;
}

void DiskOp::mouse(int x, int y)
{
    // if (checkButton(x, y, &load_inst.r))
    // {
    //     // do something
    // }
    refresh();
}

void DiskOp::keyboard(SDL_Event *e)
{

}
