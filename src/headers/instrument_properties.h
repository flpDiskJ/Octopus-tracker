#ifndef INST_PROP_H
#define INST_PROP_H

#include "tracker.h"

class Instrument_properties{

private:
    Tracker *t;
    SDL_Window *window = NULL;
    SDL_Renderer *render = NULL;
    SDL_Surface *surf;
    TTF_Font *font; // pointer to the program font
    Pallet *pallet; // pointer to the program pallet
    Button inst_name_label;
public:

    Instrument_properties(Tracker *tracker, TTF_Font *f, Pallet *p);

    ~Instrument_properties();

    void de_init();  // call before SDL_Quit in main

    void update();

    void refresh(); // render everything to the window

    void open(); // open sub-window

    void close(); // close sub-window

    Uint32 get_state(); // returns sdl window state

    void mouse(int x, int y);
};








#endif
