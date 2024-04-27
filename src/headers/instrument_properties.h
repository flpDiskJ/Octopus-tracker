#pragma once

#include "tracker.h"
#include "audioworks.h"
#include "sdl_keymap.h"

class Instrument_properties{

private:
    Tracker *t;
    AudioW *audioworks;
    SDL_Window *window = NULL;
    SDL_Renderer *render = NULL;
    SDL_Surface *surf;
    TTF_Font *font; // pointer to the program font
    Pallet *pallet; // pointer to the program pallet
    Slider transpose_sliderbox, finetune_sliderbox, volume_sliderbox;
    Button inst_name_label, transpose_label, finetune_label, volume_label;
    Entry samplename_entry;
    SDL_KEYMAP keymap;

    void update_instname();

    string blank_fill(string input, int len, char fill_char);

    void set_inst_name();

    void setup_tuning_sliders();

    void set_transpose_slider(int x);

    void set_finetune_slider(int x);

public:

    Instrument_properties(Tracker *tracker, AudioW *a, TTF_Font *f, Pallet *p);

    ~Instrument_properties();

    void de_init();  // call before SDL_Quit in main

    void update();

    void refresh(); // render everything to the window

    void open(); // open sub-window

    void close(); // close sub-window

    Uint32 get_state(); // returns sdl window state

    bool checkButton(int mouseX, int mouseY, SDL_Rect *button);

    void mouse(int x, int y);

    void keyboard(SDL_Event *e);
};
