#pragma once

#include "tracker.h"
#include "util.h"

#define LIST_ENTRIES 15

class Sequencer{

private:
    Tracker *t; // pointer to instance of the tracker class
    Util *u; // pointer to utility class
    SDL_Window *window = NULL;
    SDL_Renderer *render = NULL;
    SDL_Surface *surf;
    TTF_Font *font; // pointer to the program font
    Pallet *pallet; // pointer to the program pallet
    SDL_Rect list_box;
    Button append_btn, insert_btn, delete_btn;
    Button list[LIST_ENTRIES]; // Note: same amount as list_entries
    int cursor = 7;
    bool selected = false;
    int scroll_pos = 0;
    string file_name;
    bool set_seq_pos(int entry); // sets sq_pos in tracker class

public:

    Sequencer(Tracker *tracker, Util *util, TTF_Font *f, Pallet *p);

    ~Sequencer();

    void de_init(); // call before SDL_Quit in main

    void refresh(); // render everything to the window

    void update_list(); // regenerates list textures

    void open(); // open sub-window

    void close(); // close sub-window

    Uint32 get_state(); // returns sdl window state

    bool checkButton(int mouseX, int mouseY, SDL_Rect *button);

    void mouse(int x, int y);

    void keyboard(SDL_Event *e);

};
