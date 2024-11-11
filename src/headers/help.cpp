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

    // rectangle boundary for text initialization
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
    trackerCommands.push_back("03 = portamento");
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
    trackerCommands.push_back("0E = set channel pan (param 1 = left, param 2 = right)");
    trackerCommands.push_back(" ");
    trackerCommands.push_back("10 = set channel low pass filter (set to 0 to disable)");
    trackerCommands.push_back(" ");
    trackerCommands.push_back("18 = set volume (relative to sample volume)");
    trackerCommands.push_back(" ");
    trackerCommands.push_back("19 = sample offset (Octamed style)");
    trackerCommands.push_back(" ");
    trackerCommands.push_back("0FFF = Mute channel");

    // populate keyboardLines vector with info needed to render in refresh()
    for (unsigned int i = 0; i < trackerCommands.size(); i++) {
        keyboardLines.push_back(getLine(trackerCommands[i], i));
    }

    // populate keyboardInfo vector with text info
    keyboardInfo.push_back("Playback (global):");
    keyboardInfo.push_back("  CNTRL+SPACE = loop block (resume)");
    keyboardInfo.push_back("  CNTRL+ALT = loop block (from top)");
    keyboardInfo.push_back("  SHIFT+SPACE = play track (resume)");
    keyboardInfo.push_back("  CNTRL+SHIFT+SPACE = play track from the start");
    keyboardInfo.push_back("  SPACE = stop");
    keyboardInfo.push_back(" ");
    keyboardInfo.push_back("Main window:");
    keyboardInfo.push_back(" ");
    keyboardInfo.push_back("  Navigation:");
    keyboardInfo.push_back("    Arrows = navigate steps and entry points");
    keyboardInfo.push_back("    TAB/SHIFT+TAB or ALT+LEFT/RIGHT = move cursor to prev/next chann");
    keyboardInfo.push_back("    -el");
    keyboardInfo.push_back("    SHIFT+UP/DOWN = change block");
    keyboardInfo.push_back("    SHIFT+LEFT/RIGHT = change instrument/sample");
    keyboardInfo.push_back("    FUNC1-5 = change octave");
    keyboardInfo.push_back("    FUNC6-10 or CTRL+1-5 = navigate block");
    keyboardInfo.push_back(" ");
    keyboardInfo.push_back("  Editing:");
    keyboardInfo.push_back("    ESC = toggle edit mode");
    keyboardInfo.push_back("    CTRL+X = clear channel");
    keyboardInfo.push_back("    CTRL+SHIFT+X = clear block");
    keyboardInfo.push_back("    Delete or Backspace = delete step");
    keyboardInfo.push_back("    CTRL+N = append new block");
    keyboardInfo.push_back("    CTRL+SHIFT+N = insert new block");
    keyboardInfo.push_back("    CTRL+C/V = Copy/Paste channel");
    keyboardInfo.push_back("    CTRL+SHIFT+C/V = Copy/Paste block");
    keyboardInfo.push_back("    SHIFT+ALT+0-9 Copy note to buffer");
    keyboardInfo.push_back("    SHIFT+0-9 = Paste note from buffer");
    keyboardInfo.push_back(" ");
    keyboardInfo.push_back("  Skip and Highlight:");
    keyboardInfo.push_back("    CTRL+LEFT/RIGHT = change skip amount");
    keyboardInfo.push_back("    tilde key = toggle skip between 1 and previously set amount-");
    keyboardInfo.push_back("    -click on skip with mouse to toggle skip mode (nearest multiple");
    keyboardInfo.push_back("    -/ absolute)");
    keyboardInfo.push_back("    CTRL+SHIFT+LEFT/RIGHT = change highlight iteration");
    keyboardInfo.push_back(" ");
    keyboardInfo.push_back("  Sub-Windows:");
    keyboardInfo.push_back("    CTRL+O = open sequence order window");
    keyboardInfo.push_back("    CTRL+H = open track properties window (bpm, speed, and such)");
    keyboardInfo.push_back("    CTRL+B = open block list window (block length, speed, and such)");
    keyboardInfo.push_back("    CTRL+F = open disk op window (load/save module)");
    keyboardInfo.push_back("    CTRL+I = open instrument window");
    keyboardInfo.push_back("    CTRL+E = open sample edit window");
    keyboardInfo.push_back("    CTRL+X = close sub-window");
    keyboardInfo.push_back(" ");
    keyboardInfo.push_back("  Quit:");
    keyboardInfo.push_back("    CTRL+SHIFT+Q = quit");
    keyboardInfo.push_back("    Linux heads: TAB to select yes/no then hit enter");
    keyboardInfo.push_back(" ");
    keyboardInfo.push_back("Block sub-window:");
    keyboardInfo.push_back("  UP/DOWN = scroll block list");
    keyboardInfo.push_back("  SHIFT+UP/DOWN = change block");
    keyboardInfo.push_back("  - Click entry box with mouse to activate keyboard input.");
    keyboardInfo.push_back("    Hit enter/return to exit keyboard input.");
    keyboardInfo.push_back(" ");
    keyboardInfo.push_back("Sequence Sub-window:");
    keyboardInfo.push_back("  UP/DOWN = scroll sequence list");
    keyboardInfo.push_back("  LEFT/RIGHT = change block number in sequence element (only effects");
    keyboardInfo.push_back("  -selected element)");
    keyboardInfo.push_back("  SHIFT+UP/DOWN = change block");
    keyboardInfo.push_back("  CTRL+B = open block list sub-window");
    keyboardInfo.push_back(" ");
    keyboardInfo.push_back("Sample Editor Sub-window:");
    keyboardInfo.push_back("  LEFT/RIGHT = move waveform");
    keyboardInfo.push_back("  MOUSE SCROLL = zoom waveform");
    keyboardInfo.push_back("  LEFT/RIGHT/MIDDLE MOUSE BUTTONS = set waveform selection");
    keyboardInfo.push_back("  SHIFT+LEFT/RIGHT = change sample/instrument");
    keyboardInfo.push_back("  CTRL+LeftMouse = set loop point");
    keyboardInfo.push_back(" ");
    keyboardInfo.push_back("Disk Op Sub-window:");
    keyboardInfo.push_back("  UP/DOWN = scroll by 15");
    keyboardInfo.push_back("  In inst mode. To set pitch, click on the pitch display-");
    keyboardInfo.push_back("    -then use RIGHT/LEFT arrows to select either the note, key, or oc");
    keyboardInfo.push_back("    -tave.");
    keyboardInfo.push_back("    Filter button toggles wether or not low pass is applied before do");
    keyboardInfo.push_back("    -wn-sampling.");

    // generate all textures for text in keyboardInfo subwindow
    for (unsigned int i = 0; i < keyboardInfo.size(); i++) {
        surf = TTF_RenderText_Solid(font, keyboardInfo[i].c_str(), pallet->black);
        keyboardInfoTextures.push_back(SDL_CreateTextureFromSurface(render, surf));
        SDL_FreeSurface(surf);
    }

    // initialize rectangles that will be used in rendering text to keyboardInfo subwindow
    for (unsigned int i = 0; i < KEYBOARD_LINES; i++) {
        keyboardInfoRects[i].x = 60;
        keyboardInfoRects[i].y = (i * 25) + 60;
        keyboardInfoRects[i].h = 20;
        keyboardInfoRects[i].w = keyboardInfo[i].length() * CHAR_WIDTH; // calculate this from text length for window startup
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

        // render text, rectangles will be updated in update_rectangles() method which is called from the mouse_wheel() method
        for (unsigned int r = 0, t = scroll_offset; r < KEYBOARD_LINES; r++, t++) {
            if (t < keyboardInfoTextures.size())
            {
                keyboardInfoRects[r].w = keyboardInfo[t].length() * CHAR_WIDTH;
                SDL_RenderCopy(render, keyboardInfoTextures[t], NULL, &keyboardInfoRects[r]);
            }
        }

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
    if(e->wheel.y < 0)
    {
        if (scroll_offset < keyboardInfoTextures.size() - KEYBOARD_LINES)
        {
            scroll_offset++;
        }
    }
    else if(e->wheel.y > 0)
    {
        if (scroll_offset > 0)
        {
            scroll_offset--;
        }
    }
    refresh();
}

void HelpWindow::keyboard(SDL_Event *e)
{
    if (SDL_GetModState() & KMOD_CTRL && e->key.keysym.sym == SDLK_x)
    {
        close();
    }
}
