// Octapus Version 1.0 2023
// Code by Jake Aigner & Stephen Robinson
// Open source. Free to distribute.

#include "headers/tracker.h"
#include "headers/util.h"
#include "headers/audioworks.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

void audio_callback(void* buffer, Uint8* stream, int len)
{
    AudioBuffer *b = (AudioBuffer*)buffer;
    memset(stream, 0, len);
    Uint32 amount = b->len - b->pos;
    if ( amount > len ) {
        amount = len;
    }
    SDL_MixAudio(stream, b->data, amount, SDL_MIX_MAXVOLUME);
    b->pos += amount;
    b->update = true; // tell audio_works() to fill buffer
}

int main(int argc, char* args[]) {

    Pallet pallet;
    pallet.black = {0, 0, 0};
    pallet.red = {200, 0, 0};
    pallet.blue = {0, 0, 180};
    pallet.green = {0, 100, 0};

    TTF_Font* Font = NULL;

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

    Font = TTF_OpenFont("font.TTF", 20);
    if (Font == NULL)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        return 3;
    }

    Tracker tracker(tracker_render, Font, &pallet);

    tracker.update_info();

    Util util(&tracker, Font, &pallet);

    AudioBuffer audio_buffer;

    SDL_AudioSpec mFormat;
    mFormat.format = AUDIO_S16LSB; mFormat.freq = SAMPLE_RATE; mFormat.channels = AUDIO_CHANNELS;
    mFormat.callback = audio_callback; mFormat.samples = BUFF_SIZE;
    mFormat.userdata = &audio_buffer;

    audio_buffer.len = BUFF_SIZE*BYTES_IN_SAMPLE*AUDIO_CHANNELS;
    audio_buffer.data = new Uint8[audio_buffer.len];
    audio_buffer.update = false;
    audio_buffer.stop = false;
    audio_buffer.pos = 0;
    memset(audio_buffer.data, 0, BUFF_SIZE*BYTES_IN_SAMPLE*AUDIO_CHANNELS);

    if (SDL_OpenAudio(&mFormat, NULL) < 0)
    {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
    }
    SDL_PauseAudio(0);

    AudioW aworks(&tracker, &audio_buffer);

    tracker.load_inst("/home/jake/code/c/octapus/test_sample.wav", "Test Sample"); // used for testing only

    SDL_Event e;
    bool render = true; // set to true to update screen
    int windowID = 0; // 0 = main // 1 = utility window
    int xM, yM; // mouse cords
    bool run = true;
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
                    if (e.window.event == SDL_WINDOWEVENT_CLOSE)
                    {
                        switch (windowID)
                        {
                            case 0:
                                util.open("Quit?", 0);
                                windowID = 1;
                                break;
                            case 1:
                                util.close();
                                windowID = 0;
                                break;
                            default: break;
                        }
                    }
                    render = true;
                    if (windowID == 1)
                    {
                        util.render();
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    SDL_GetMouseState(&xM, &yM);
                    switch (windowID) {
                        case 0:
                            break;
                        case 1:
                            util.mouse(xM, yM);
                            switch (util.command)
                            {
                                case 'Q':
                                    run = false;
                                    break;
                                case 'q':
                                    util.close();
                                    windowID = 0;
                                    render = true;
                                    break;
                                default:
                                    break;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_q && (SDL_GetModState() & KMOD_CTRL) && (SDL_GetModState() & KMOD_SHIFT))
                    {
                        util.open("Quit?", 0);
                        windowID = 1;
                    }
                    if (e.key.keysym.sym == SDLK_SPACE)
                    {
                        audio_buffer.stop = true;
                    }
                    switch (windowID) {
                        case 0:
                            render = true;
                            if (SDL_GetModState() & KMOD_CTRL)
                            {
                                if (e.key.keysym.sym == SDLK_b)
                                {
                                    util.open("Block Parameters", 1);
                                    windowID = 1;
                                } else if (e.key.keysym.sym == SDLK_h)
                                {
                                    util.open("Track Parameters", 2);
                                    windowID = 1;
                                }
                            }
                            tracker.keyboard(&e);
                            aworks.play_note(&e);
                            break;
                        case 1:
                            util.input(&e);
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

            tracker.render_info();

            tracker.render_steps();

            SDL_RenderPresent(tracker_render); // Present image to screen
            render = false;
        }

        if (audio_buffer.update)
        {
            SDL_LockAudio();
            aworks.audio_works(); // generate audio buffer data
            SDL_UnlockAudio();
            audio_buffer.update = false;
        }
    }

    util.close_all();
    free(audio_buffer.data);
    SDL_CloseAudio();
    SDL_DestroyRenderer(tracker_render);
    SDL_DestroyWindow(tracker_window);
    TTF_CloseFont(Font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
