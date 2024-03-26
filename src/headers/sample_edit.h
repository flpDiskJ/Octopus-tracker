#pragma once

#include "tracker.h"

class Sample_edit{

private:
    Tracker *t;
    SDL_Window *window = NULL;
    SDL_Renderer *render = NULL;
    SDL_Surface *surf;
    TTF_Font *font; // pointer to the program font
    Pallet *pallet; // pointer to the program pallet
    Button samplename_entry;
    Slider transpose_sliderbox, finetune_sliderbox, volume_sliderbox;
    Button inst_name_label, transpose_label, finetune_label, volume_label;
public:

    Sample_edit(Tracker *tracker, TTF_Font *f, Pallet *p);

    ~Sample_edit();

    void de_init();  // call before SDL_Quit in main

    void update();

    void refresh(); // render everything to the window

    void open(); // open sub-window

    void close(); // close sub-window

    Uint32 get_state(); // returns sdl window state

    bool checkButton(int mouseX, int mouseY, SDL_Rect *button);

    void mouse(int x, int y);
};
