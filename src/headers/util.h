#ifndef UTIL_H
#define UTIL_H

#include "tracker.h"

// Used for all utility windows

class Util
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event e;
    SDL_Surface *surf;
    TTF_Font *font;
    SDL_Rect name; // displays window name
    SDL_Texture *name_tex; // displays window name
    Pallet *p;

public:

    Util(TTF_Font *gfont, Pallet *pallet);

    ~Util();

    void open(string title); // opens window for specified purpose

    void update(); // updates textures

    void render(); // renders everything to the screen

    void input(); // handles keyboard and mouse input
};

#endif
