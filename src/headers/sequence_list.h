#ifndef SEQ_LIST_H
#define SEQ_LIST_H

#include "tracker.h"

class Sequencer{

private:
    Tracker *t; // pointer to instance of the tracker class
    SDL_Window *window = NULL;
    SDL_Renderer *render = NULL;
    SDL_Surface *surf;
    TTF_Font *font; // pointer to the program font
    Pallet *pallet; // pointer to the program pallet

public:

    Sequencer(Tracker *tracker, TTF_Font *f, Pallet *p);

    ~Sequencer();

    void de_init(); // call before SDL_Quit in main

    void refresh(); // render everything to the window

    void open(); // open sub-window

    void close(); // close sub-window

    Uint32 get_state(); // returns sdl window state

    void keyboard(SDL_Event *e);

};

#endif
