#pragma once
#include "global.h"
#include "tracker.h"

class HelpWindow {
 private:
  Tracker *t;
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  SDL_Surface *surf;
  TTF_Font *font;
  Pallet *pallet;
};
