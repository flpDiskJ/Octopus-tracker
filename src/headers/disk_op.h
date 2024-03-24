#pragma once

#include "tracker.h"
#include <dirent.h>
#include <vector>

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
        struct dirent *dp; // instance of the dirent struct to get sub-directory strings
        DIR *d_op_path; // directory stream for disk op window
        vector<string> path_list;
        enum path_type
        {
            module_p, sample_p, export_p
        };
        path_type parent_index = module_p;
        string parent[3];

        // helper methods
        string cat_path(string path1, string path2);

        void fill_path_list();

        void update_list_textures();


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
