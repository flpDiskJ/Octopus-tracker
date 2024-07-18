#pragma once

#define CHAR_WIDTH 10

#include "global.h"
#include <vector>

struct radio_button {
  SDL_Rect boundary;
  SDL_Texture *text;
  bool clicked = false;
};

struct line {
  SDL_Rect r;
  SDL_Texture *t;
};

class HelpWindow {
 private:
  SDL_Window *window = NULL;
  SDL_Renderer *render = NULL;
  SDL_Surface *surf;
  SDL_Rect help_list;
  TTF_Font *font;
  Pallet *pallet;
  radio_button keyboard_info, tracker_commands;
  vector<string> keyboardInfo;
  vector<string> trackerCommands;
  vector<line> keyboardLines;
  line getLine(string s, unsigned int y);


public:
    HelpWindow(TTF_Font *f, Pallet *p);

    ~HelpWindow();

    void de_init();  // call before SDL_Quit in main

    void set_text(const char *text, SDL_Texture *texture);

    void refresh(); // render everything to the window

    void open(); // open sub-window

    void close(); // close sub-window

    Uint32 get_state(); // returns sdl window state

    bool checkButton(int mouseX, int mouseY, SDL_Rect *button);

    void mouse(int x, int y);

    void mouse_wheel(SDL_Event *e);

    void keyboard(SDL_Event *e);

};
