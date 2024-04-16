#ifndef SDL_KEYMAP_H
#define SDL_KEYMAP_H

#include <map>
#include <string>
#include <cstring>
#include "global.h"

class SDL_KEYMAP{
private:
    struct Dflt{
        char val = '~';
    };
    map<string, Dflt> lmap;
public:

    SDL_KEYMAP();

    ~SDL_KEYMAP();

    string sdl_getText(string keyname, bool shift); // pass in SDL_KEYNAME returns character as string

};

#endif
