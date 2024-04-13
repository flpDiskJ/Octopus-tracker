#pragma once

#include "tracker.h"
#include "audioworks.h"

class Sample_edit{

private:
    AudioW *audioworks;
    Tracker *t;
    SDL_Window *window = NULL;
    SDL_Renderer *render = NULL;
    SDL_Surface *surf;
    TTF_Font *font; // pointer to the program font
    Pallet *pallet; // pointer to the program pallet

    struct Selector{
        unsigned long int sample_front; // actual sample position
        unsigned long int sample_back;
    };

    Button waveform;
    Selector selection; // selected area of sample
    double wave_zoom; // number of samples to skip over when drawing waveform
    long int zoom_offset; // used to calculate offset based on zoom to start/end
    long int wave_offset; // start position offset
    double wave_scale; // amplitude scale

    Entry selection_front_entry;
    Entry selection_back_entry;
    Button sample_len_display;
    Entry zoom_to_start, zoom_to_end;
    Button range_all;

    void setup_new_sample(); // sets inital values for zoom and selection

    void draw_wave();

    void get_sample_postions(int x, bool front); // calculates sample positions for selection

    string blank_fill(string input, int len, char fill_char); // fills string with 'fill_char' to get desired length

    void bound_offset(); // ensures wave_offset is within bounds

    void get_zoom_offset(); // call after changing selection or zoom type

    void select_all(); // selects entire sample

    void render_struct(SDL_Renderer *r, Button *b, Entry *e); // renders button and/or entry

    void update_selection_index();

public:

    int waveform_zoom_sensitivity = 25;

    Sample_edit(AudioW *audio, Tracker *tracker, TTF_Font *f, Pallet *p);

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
