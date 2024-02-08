// Octapus Version 1.0 2023
// Code by Jake Aigner & Stephen Robinson
// Open source. Free to distribute.

#include "headers/tracker.h"
#include "headers/util.h"
#include "headers/audioworks.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

class Timing{ // handles timing for tracker
private:
    Tracker *t;
    AudioW *works;
    Uint64 previous;
    Uint64 current;
    int amount;
    bool run_seq = false;
    Uint64 barTime;
public:
    bool active = false;

    Timing(Tracker *trk, AudioW *wrk)
    {
        t = trk;
        works = wrk;
        barTime = SDL_GetTicks64();
    }

    ~Timing(){}

    void start(bool run_sequence)
    {
        active = true;
        run_seq = run_sequence;
        t->set_timing_delay();
        previous = SDL_GetTicks64();
    }

    void render_bars(SDL_Renderer *renderer)
    {
        SDL_SetRenderDrawColor(renderer, 0, 128, 0, 0xFF); // Black
        for (int bar = 0; bar < CHANNELS; bar++)
        {
            SDL_RenderFillRect(renderer, &t->trigger_bars[bar]);
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF); // Black
    }

    void check()
    {
        if (active)
        {
            current = SDL_GetTicks64();
            amount = (int)((current - previous) / t->timing_delay);
            if (amount > 0)
            {
                for (int x = 0; x < amount; x++)
                {
                    works->play_step();
                    t->move_step(run_seq);
                }
                previous = current;
            }
        }
    }

    void end()
    {
        active = false;
        run_seq = false;
    }
};

void audio_callback(void* buffer, Uint8* stream, int len)
{
    AudioBuffer *b = (AudioBuffer*)buffer;

    int region1size = len;
    int region2size = 0;

    if (b->read_pos + len > b->len)
    {
        region1size = b->len - b->read_pos;
        region2size = len - region1size;
    }

    SDL_memcpy(
    stream,
    (&b->data[b->read_pos]),
    region1size
    );
    SDL_memcpy(
    &stream[region1size],
    b->data,
    region2size
    );

    b->read_pos = (b->read_pos + len) % b->len;
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

    if (SDL_Init(SDL_INIT_VIDEO||SDL_INIT_AUDIO||SDL_INIT_TIMER) < 0)
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

    Font = TTF_OpenFont("font.TTF", 16);
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

    audio_buffer.len = BUFF_SIZE*2*BYTES_IN_SAMPLE*AUDIO_CHANNELS * 3;
    audio_buffer.data = new Uint8[audio_buffer.len];
    audio_buffer.stop = false;
    audio_buffer.read_pos = 0;
    audio_buffer.write_pos = 0;
    memset(audio_buffer.data, 0, audio_buffer.len);

    if (SDL_OpenAudio(&mFormat, NULL) < 0)
    {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
    }
    SDL_PauseAudio(0);

    AudioW aworks(&tracker, &audio_buffer);

    Timing timing(&tracker, &aworks);

    tracker.load_inst("test_sample.wav", "Test Sample"); // used for testing only

    SDL_Event e;
    int windowID = 0; // 0 = main // 1 = utility window
    int xM, yM; // mouse cords
    bool run = true;
    Uint64 previousTime = SDL_GetTicks64();
    Uint64 currentTime;
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
                    if (windowID == 1)
                    {
                        util.render();
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    SDL_GetMouseState(&xM, &yM);
                    switch (windowID) {
                        case 0:
                            tracker.mouse(xM, yM);
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
                        timing.end();
                    }
                    switch (windowID) {
                        case 0:
                            if (SDL_GetModState() & KMOD_CTRL) // control key press
                            {
                                if (e.key.keysym.sym == SDLK_b) // open block params
                                {
                                    util.open("Block Parameters", 1);
                                    windowID = 1;
                                    break;
                                } else if (e.key.keysym.sym == SDLK_h) // open track params
                                {
                                    util.open("Track Parameters", 2);
                                    windowID = 1;
                                    break;
                                } else if (e.key.keysym.sym == SDLK_SPACE) // run tracker. Loop current block.
                                {
                                    timing.start(false);
                                    break;
                                }
                            } else if (SDL_GetModState() & KMOD_SHIFT) // shift key press
                            {
                                if (e.key.keysym.sym == SDLK_SPACE) // run tracker and step through track sequence.
                                {
                                    timing.start(true);
                                    break;
                                }
                            }
                            tracker.keyboard(&e, timing.active);
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

        timing.check();

        currentTime = SDL_GetTicks64();
        if (currentTime - previousTime >= 40)
        {
            previousTime = currentTime;
            // Rendering
            SDL_SetRenderDrawColor(tracker_render, 128, 128, 128, 0xFF); // Background color
            SDL_RenderClear(tracker_render);

            SDL_SetRenderDrawColor(tracker_render, 0, 0, 0, 0xFF); // Black

            tracker.render_info();

            tracker.render_steps();

            timing.render_bars(tracker_render);

            SDL_RenderPresent(tracker_render); // Present image to screen

            tracker.dec_trigger_bars(); // apply decay to level bars
        }

        timing.check();

        if (audio_buffer.read_pos != audio_buffer.write_pos)
        {
            SDL_LockAudio();
            aworks.audio_works(); // generate audio buffer data
            SDL_UnlockAudio();
            //audio_buffer.update = false;
        }

        timing.check();

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
