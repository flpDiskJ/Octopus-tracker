// Octapus Version 1.0 2023
// Code by Jake Aigner & Stephen Robinson
// Open source. Free to distribute.

#include "headers/tracker.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

int main(int argc, char* args[]) {

    SDL_Window* tracker_window = NULL;

    SDL_Surface* t_surface = NULL;

    SDL_Renderer* tracker_render = NULL;

    if (SDL_Init(SDL_INIT_VIDEO||SDL_INIT_AUDIO) < 0)
    {
        printf("SDL Init Failed!\n");
        return 1;
    }

    if(TTF_Init() == -1)
    {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return 1;
    }

    tracker_window = SDL_CreateWindow("Octapus V1.0   |   Code by Jake Aigner & Stephen Robinson", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (tracker_window == NULL)
    {
        printf("Window creation failed!\n");
        return 2;
    }

    tracker_render = SDL_CreateRenderer(tracker_window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (tracker_render == NULL)
    {
        printf("Failed to create renderer!\n");
        return 2;
    }

    Tracker tracker(tracker_render);

    tracker.gFont = TTF_OpenFont("font.TTF", 20);
    if (tracker.gFont == NULL)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        return 3;
    }

    SDL_Event e;
    bool run = true;
    bool render = true; // set to true to update screen
    while(run)
    {
        // Event loop
        while(SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    run = false;
                    break;
                case SDL_WINDOWEVENT:
                    render = true; // ensures window re-renders when un-minimized (linux)
                    break;
                case SDL_KEYDOWN:
                    render = true;
                    switch (e.key.keysym.sym) // Keyboard input
                    {
                        case SDLK_UP:
                            tracker.decpos(1);
                            break;
                        case SDLK_DOWN:
                            tracker.incpos(1);
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        if (render)
        {
            // Rendering
            SDL_SetRenderDrawColor(tracker_render, 128, 128, 128, 0xFF); // Background color
            SDL_RenderClear(tracker_render);

            SDL_SetRenderDrawColor(tracker_render, 0, 0, 0, 0xFF); // Black

            tracker.render_steps(tracker_render);

            tracker.render_info(tracker_render);

            SDL_RenderPresent(tracker_render); // Present image to screen
            render = false;
        }
    }

    // destructor takes care of cleaning up the memory for us when the program exits

    SDL_DestroyRenderer(tracker_render);
    SDL_DestroyWindow(tracker_window);
    TTF_CloseFont(tracker.gFont);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
