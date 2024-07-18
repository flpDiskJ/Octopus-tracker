#include "help.h"

HelpWindow::HelpWindow(TTF_Font *f, Pallet *p)
{
    font = f;
    pallet = p;

    window = SDL_CreateWindow("Help", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    800, 600, SDL_WINDOW_HIDDEN);
    if (window == NULL)
    {
        printf("Help window creation failed!\n");
        return;
    }

    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (render == NULL)
    {
        printf("Failed to create renderer for help window!\n");
    }

    help_list.x = 50;
    help_list.y = 60;
    help_list.h = 520;
    help_list.w = 700;

    // radio button initialization
    keyboard_info.boundary.x = 100;
    keyboard_info.boundary.y = 5;
    keyboard_info.boundary.w = 200;
    keyboard_info.boundary.h = 45;

    keyboard_info.clicked = true;

    tracker_commands.boundary.x = 500;
    tracker_commands.boundary.y = 5;
    tracker_commands.boundary.w = 200;
    tracker_commands.boundary.h = 45;

    tracker_commands.clicked = false;

    // radio button text
    surf = TTF_RenderText_Solid(font, "Keyboard Info", pallet->black);
    keyboard_info.text = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    surf = TTF_RenderText_Solid(font, "Tracker Commands", pallet->black);
    tracker_commands.text = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);

    // populate trackerCommand vector with text info
    trackerCommands.push_back("00 = arpeggio");
    trackerCommands.push_back("01 = pitch up");
    trackerCommands.push_back("02 = pitch down");
    trackerCommands.push_back("03 = portament");
    trackerCommands.push_back("04 = vibrato");
    trackerCommands.push_back("05 = echo (param 1 = number of re-triggers  param 2 = speed)");
    trackerCommands.push_back("06 = multi-trigger");
    trackerCommands.push_back("07 = tremolo");
    trackerCommands.push_back("08 = hold and decay");
    trackerCommands.push_back("09 = sample offset (scaled to sample size)");
    trackerCommands.push_back("0A = reverse playback with offset");
    trackerCommands.push_back("0B = playing sequence position jump");
    trackerCommands.push_back("0C = set channel volume");
    trackerCommands.push_back("0D = volume slide (param 1 = increase, param 2 = decrease)");
    trackerCommands.push_back(" ");
    trackerCommands.push_back("18 = set volume (relative to sample volume");
    trackerCommands.push_back(" ");
    trackerCommands.push_back("19 = sample offset (Octamed style)");
    trackerCommands.push_back(" ");
    trackerCommands.push_back("0FFF = stop sound in channel");

    // populate keyboardLines vector with info needed to render in refresh()
    for (unsigned int i = 0; i < trackerCommands.size(); i++) {
        keyboardLines.push_back(getLine(trackerCommands[i], i));
    }

}

HelpWindow::~HelpWindow()
{

}

void HelpWindow::de_init()
{
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
}

void HelpWindow::set_text(const char *text, SDL_Texture *texture)
{
    surf = TTF_RenderText_Solid(font, text, pallet->black);
    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
    }
    texture = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);
}

// need to clean this code up
void HelpWindow::refresh()
{
    SDL_SetRenderDrawColor(render, pallet->bgd.r, pallet->bgd.g, pallet->bgd.b, 0xFF); // Background color
    SDL_RenderClear(render);

    // outline box for help text
    SDL_SetRenderDrawColor(render, 0, 0, 0, 255); // change color data to match rest of code
    SDL_RenderDrawRect(render, &help_list);

    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
    if (keyboard_info.clicked)
    {
        // render button selection for keyboard info
        SDL_RenderFillRect(render, &keyboard_info.boundary);
    } else if (tracker_commands.clicked)
    {
        // render button selection for tracker commands
        SDL_RenderFillRect(render, &tracker_commands.boundary);

        // render text to tracker commands submenu
        for (unsigned int i = 0; i < keyboardLines.size(); i++) {
            SDL_RenderCopy(render, keyboardLines[i].t, NULL, &keyboardLines[i].r);
        }

    }

    SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
    // button outlines
    SDL_RenderDrawRect(render, &keyboard_info.boundary);
    SDL_RenderDrawRect(render, &tracker_commands.boundary);

    // button text
    SDL_RenderCopy(render, keyboard_info.text, NULL, &keyboard_info.boundary);
    SDL_RenderCopy(render, tracker_commands.text, NULL, &tracker_commands.boundary);


    SDL_RenderPresent(render); // Present image to screen
}

void HelpWindow::open()
{
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    SDL_SetWindowInputFocus(window);
    refresh();
}

line HelpWindow::getLine(string s, unsigned int y) {
    line l;
    surf = TTF_RenderText_Solid(font, s.c_str(), pallet->black);
    l.t = SDL_CreateTextureFromSurface(render, surf); // add texture to list struct
    SDL_FreeSurface(surf);
    l.r.w = s.length() * CHAR_WIDTH;
    l.r.h = 20;
    l.r.x = 60;
    l.r.y = (y * 25) + 60;
    return l;
}

void HelpWindow::close()
{
    SDL_HideWindow(window);
}

Uint32 HelpWindow::get_state()
{
    return SDL_GetWindowFlags(window);
}

bool HelpWindow::checkButton(int mouseX, int mouseY, SDL_Rect *button)
{
    if (mouseX > button->x && mouseX < button->w+button->x && mouseY > button->y && mouseY < button->h+button->y)
    {
        return true;
    }
    return false;
}

void HelpWindow::mouse(int x, int y)
{
    if (checkButton(x, y, &keyboard_info.boundary))
    {
        keyboard_info.clicked = true;
        tracker_commands.clicked = false;
    } else if (checkButton(x, y, &tracker_commands.boundary))
    {
        tracker_commands.clicked = true;
        keyboard_info.clicked = false;
    }
    refresh();
}

void HelpWindow::mouse_wheel(SDL_Event *e)
{
    if(e->wheel.y > 0) // scroll down
    {

    }
    else if(e->wheel.y < 0) // scroll up
    {

    }
    refresh();
}

void HelpWindow::keyboard(SDL_Event *e)
{

}
