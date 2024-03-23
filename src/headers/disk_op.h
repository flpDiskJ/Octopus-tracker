#ifndef DISK_OP_H
#define DISK_OP_H

#include "tracker.h"
#include <dirent.h>

class DiskOp {
    private:
        Tracker *t; // pointer to instance of the tracker class
        SDL_Window *window = NULL;
        SDL_Renderer *render = NULL;
        SDL_Surface *surf;
        TTF_Font *font; // pointer to the program font
        Pallet *pallet; // pointer to the program pallet
        Button load_inst, save_file, load_file, export_audio; // DiskOp gui buttons
        SDL_Rect file_border;
    public:
        DiskOp(Tracker *tracker, TTF_Font *f, Pallet *p);

        ~DiskOp();

        void de_init(); // call before SDL_Quit in main

        void refresh(); // render everything to the window

        void open(); // open sub-window

        void close(); // close sub-window

        Uint32 get_state(); // returns sdl window state

        bool checkButton(int mouseX, int mouseY, SDL_Rect *button);

        void mouse(int x, int y);

        void keyboard(SDL_Event *e);
};

#endif
