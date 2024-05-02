#pragma once

#include "tracker.h"
#include <dirent.h>
#include <vector>
#include "moduleformat.h"

#define FILE_LIST_SIZE 16
#define MOD_PATH 0
#define EXPORT_PATH 1
#define SAMPLE_PATH 2

/**
NOTE FROM JAKE

    Add a way for the user to change t->default_pitch (by default it is set to 'C-3')

    when t->load_inst() is called it will now resample the audio to the pitch specified in default_pitch

    only the note, key, and octave elements from default_pitch are used (check Note struct in tracker.h)

    NEW: load_module(), save_module(), and export_wav() are ready for use. Check moduleformat.h

**/

class DiskOp {
    private:
        Tracker *t; // pointer to instance of the tracker class
        SDL_Window *window = NULL;
        SDL_Renderer *render = NULL;
        SDL_Surface *surf;
        TTF_Font *font; // pointer to the program font
        Pallet *pallet; // pointer to the program pallet
        Button save_file, load_file; // DiskOp gui buttons
        Button inst_b, octo_b, wave_b;
        SDL_Rect file_border;
        struct dirent *dp; // instance of the dirent struct to get sub-directory strings
        DIR *d_op_path; // directory stream for disk op window
        vector<string> path_list_strings; // all the file/dir names in the given directory
        vector<int> path_list_types; // list item type (file or dir) corresponds to path_list_strings
        // DT_REG = normal file, DT_DIR = directory

        int list_offset = 0;
        int selected = -1; // set to -1 when nothing is selected
        Button path_list_buttons[FILE_LIST_SIZE];
        Uint8 parent_index = SAMPLE_PATH;
        string parent[3];
        Button parent_path_display;

        ModuleFormat *module; // use this class to save and load the module

        // helper methods
        void fill_path_list();

        void set_parent_display();

        void update_list_textures(); // this might end up being more similar to the update_sequence_list method from sequence_list.h


    public:
        DiskOp(Tracker *tracker, ModuleFormat *m, TTF_Font *f, Pallet *p);

        ~DiskOp();

        void de_init(); // call before SDL_Quit in main

        void refresh(); // render everything to the window

        void open(); // open sub-window

        void close(); // close sub-window

        Uint32 get_state(); // returns sdl window state

        bool checkButton(int mouseX, int mouseY, SDL_Rect *button);

        void mouse(int x, int y);

        void mouse_wheel(SDL_Event *e);

        void keyboard(SDL_Event *e);
};
