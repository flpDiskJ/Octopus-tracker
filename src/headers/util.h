#ifndef UTIL_H
#define UTIL_H

#include "tracker.h"

// Used for all utility windows

class Util
{
private:
    bool windowopen = false;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surf;
    TTF_Font *font;
    string name;
    SDL_Rect name_rec; // displays window name
    SDL_Texture *name_tex; // displays window name
    Pallet *p;
    Tracker *tracker;
    int mode = 0; // 0 = quit? yes/no // 1 = block parameters

    struct Buttons{
        SDL_Rect yes; SDL_Texture *yes_tex;
        SDL_Rect no; SDL_Texture *no_tex;
        SDL_Rect del; SDL_Texture *del_tex;
    };
    Buttons button;

    // for block parameters (mode 1)
    SDL_Rect block_list;
    SDL_Rect block_name; SDL_Texture *block_name_tex;
    SDL_Rect block_speed; SDL_Texture *block_speed_tex;
    SDL_Rect list_index[10]; SDL_Texture *list_index_tex[10];
    SDL_Rect cursor;
    int pos = -4;
    bool text_mode = false;
    bool speed_mode = false; string speed_entry;

    void update(); // updates textures

    void setSpeedEntry();

public:
    char command = 'n'; // used to send instructions to main

    Util(Tracker *t, TTF_Font *gfont, Pallet *pallet);

    ~Util();

    bool checkButton(int mouseX, int mouseY, SDL_Rect *button); // returns true if mouse is inside button

    void open(string title, int m); // opens window for specified purpose (m for mode)

    void close(); // closes window (window is set to hidden)

    void render(); // renders everything to the screen

    void mouse(int x, int y); // mouse stuff

    void input(SDL_Event *e); // handles keyboard input

};

#endif
