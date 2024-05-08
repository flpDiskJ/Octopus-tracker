#pragma once
#include "global.h"

class HelpWindow {
 private:
  SDL_Window *window = NULL;
  SDL_Renderer *render = NULL;
  SDL_Surface *surf;
  TTF_Font *font;
  Pallet *pallet;

public:
    HelpWindow(TTF_Font *f, Pallet *p);

    ~HelpWindow();

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
