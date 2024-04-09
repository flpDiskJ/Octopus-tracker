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
        unsigned int front;
        unsigned int back;
    };

    Button waveform;
    Selector selection; // selected area of sample
    double wave_zoom; // number of samples to skip over when drawing waveform
    int wave_offset; // start position offset
    double wave_scale; // amplitude scale

    void setup_new_sample(); // sets inital values for zoom and selection

    void draw_wave();

    void reset_selection();

public:

    int waveform_zoom_sensitivity = 25;

    Sample_edit(Tracker *tracker, TTF_Font *f, Pallet *p);

    ~Sample_edit();

    void de_init();  // call before SDL_Quit in main

    void update();

    void refresh(); // render everything to the window

    void open(); // open sub-window

    void close(); // close sub-window

    Uint32 get_state(); // returns sdl window state

    bool checkButton(int mouseX, int mouseY, SDL_Rect *button);

    void mouse(int x, int y, SDL_Event *e);

    void mouse_wheel(SDL_Event *e);

    void keyboard(SDL_Event *e);
};
