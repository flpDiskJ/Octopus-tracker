#pragma once

#include "tracker.h"
#include <dirent.h>
#include <vector>
#include <algorithm>
#include "moduleformat.h"
#include "sdl_keymap.h"
#include "sample_edit.h"

#define FILE_LIST_SIZE 16
#define MOD_PATH 0
#define EXPORT_PATH 1
#define SAMPLE_PATH 2
#define INPUT_MAX 500

class DiskOp {
    private:

        const char os_slash = '/'; // swap to \ for Windows

        Tracker *t; // pointer to instance of the tracker class
        Sample_edit *sampler_p;
        AudioW *audioworks; // pointer to audioworks instance
        SDL_Window *window = NULL;
        SDL_Renderer *render = NULL;
        SDL_Surface *surf;
        TTF_Font *font; // pointer to the program font
        Pallet *pallet; // pointer to the program pallet
        SDL_KEYMAP keymap;
        Button save_file, load_file; // DiskOp gui buttons
        Button inst_b, octo_b, wave_b;
        Button back_b;
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
        Entry file_name_entry;
        Entry default_note_b;
        Uint8 default_pitch_index = 0;
        SDL_Rect note_cursor;
        Button filter_b;
        bool filter = true;
        bool overwrite = false;
        bool overwrite_prompt = false;
        string filename_store;
        Button set_paths_b;
        Button stem_track_b;


        ModuleFormat *module; // use this class to save and load the module

        bool file_exist(string filename);

        void update_default_note();

        void set_default_note(const char* k, int index);

        bool check_file_extension(const char* ext, char* file);

        void fill_path_list();

        void set_parent_display();

        void set_file_name();

        void update_file_name();

        void revert_dir();

        void save_button();

        void load_button();

        void stem_button();

        void update_list_textures(); // this might end up being more similar to the update_sequence_list method from sequence_list.h

        bool save_default_paths();

        bool load_default_paths();

    public:
        DiskOp(Tracker *tracker, Sample_edit *sampler, AudioW *a, ModuleFormat *m, TTF_Font *f, Pallet *p);

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
