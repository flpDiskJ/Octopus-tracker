#include "sdl_keymap.h"

SDL_KEYMAP::SDL_KEYMAP()
{
    lmap["A"].val = 'a';
    lmap["B"].val = 'b';
    lmap["C"].val = 'c';
    lmap["D"].val = 'd';
    lmap["E"].val = 'e';
    lmap["F"].val = 'f';
    lmap["G"].val = 'g';
    lmap["H"].val = 'h';
    lmap["I"].val = 'i';
    lmap["J"].val = 'j';
    lmap["K"].val = 'k';
    lmap["L"].val = 'l';
    lmap["M"].val = 'm';
    lmap["N"].val = 'n';
    lmap["O"].val = 'o';
    lmap["P"].val = 'p';
    lmap["Q"].val = 'q';
    lmap["R"].val = 'r';
    lmap["S"].val = 's';
    lmap["T"].val = 't';
    lmap["U"].val = 'u';
    lmap["V"].val = 'v';
    lmap["W"].val = 'w';
    lmap["X"].val = 'x';
    lmap["Y"].val = 'y';
    lmap["Z"].val = 'z';
    lmap["Space"].val = ' ';
}

SDL_KEYMAP::~SDL_KEYMAP()
{

}

string SDL_KEYMAP::sdl_getText(string keyname, bool shift)
{
    if (strlen(keyname.c_str()) != 1)
    {
        return "";
    }
    if (shift)
    {
        return keyname;
    }
    char key = lmap[keyname].val;
    if (key == '~')
    {
        return keyname;
    }
    string out;
    out += key;
    return out;
}
