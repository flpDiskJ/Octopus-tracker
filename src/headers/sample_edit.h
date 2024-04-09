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

    struct Selector{
        unsigned int front; // positon relative to the display rect
        unsigned int back;
        unsigned int sample_front; // actual sample position
        unsigned int sample_back;
    };

    Button waveform;
    Selector selection; // selected area of sample
    double wave_zoom; // number of samples to skip over when drawing waveform
    int wave_offset; // start position offset
    double wave_scale; // amplitude scale

    Entry selection_front_entry;
    Entry selection_back_entry;
    Button sample_len_display;

    void setup_new_sample(); // sets inital values for zoom and selection

    void draw_wave();

    void reset_selection();

    void get_sample_postions(); // calculates sample positions for selection

    void get_positions(); // does the opposite of get_sample_postions()

    string blank_fill(string input, int len, char fill_char); // fills string with 'fill_char' to get desired length

public:

    int waveform_zoom_sensitivity = 25;

    Sample_edit(Tracker *tracker, TTF_Font *f, Pallet *p);

    ~Sample_edit();

    void de_init();  // call before SDL_Quit in main

    void refresh(); // render everything to the window

    void open(); // open sub-window

    void close(); // close sub-window

    Uint32 get_state(); // returns sdl window state

    bool checkButton(int mouseX, int mouseY, SDL_Rect *button);

    void mouse(int x, int y, SDL_Event *e);

    void mouse_wheel(SDL_Event *e);

    void keyboard(SDL_Event *e);
};
