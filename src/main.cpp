// Octopus Version 0.1 2023 - 2024
// Created by Jake Aigner & Stephen Robinson & John Dunbar
// Open source. Free to distribute. Please include original credits!

#include "headers/tracker.h"
#include "headers/util.h"
#include "headers/sequence_list.h"
#include "headers/audioworks.h"
#include "headers/instrument_properties.h"
#include "headers/disk_op.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

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
    pallet.bgd = {128, 128, 128};
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

    tracker_window = SDL_CreateWindow("Octopus V0.1 testing   |   Jake Aigner, Stephen Robinson, John Dunbar", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
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

    Sequencer sequence_list(&tracker, &util, Font, &pallet);

    Instrument_properties inst_prop(&tracker, Font, &pallet);

    DiskOp disk_op(&tracker, Font, &pallet);

    AudioBuffer audio_buffer;

    SDL_AudioSpec mFormat;
    mFormat.format = AUDIO_S16LSB; mFormat.freq = SAMPLE_RATE; mFormat.channels = AUDIO_CHANNELS;
    mFormat.callback = audio_callback; mFormat.samples = BUFF_SIZE;
    mFormat.userdata = &audio_buffer;

    audio_buffer.len = BUFF_SIZE*BYTES_IN_SAMPLE*AUDIO_CHANNELS*AUDIO_LATENCY;
    audio_buffer.data = new Uint8[audio_buffer.len];
    audio_buffer.stop = false;
    audio_buffer.read_pos = 0;
    audio_buffer.write_pos = audio_buffer.len / 2;
    memset(audio_buffer.data, 0, audio_buffer.len);

    if (SDL_OpenAudio(&mFormat, NULL) < 0)
    {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
    }
    SDL_PauseAudio(0);

    AudioW aworks(&tracker, &audio_buffer);

    tracker.load_inst("test_sample.wav", "Test Sample", 0); // used for testing only

    SDL_Event e;
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
                        if (sequence_list.get_state() & SDL_WINDOW_INPUT_FOCUS)
                        {
                            sequence_list.close();
                        } else if (util.get_state() & SDL_WINDOW_INPUT_FOCUS)
                        {
                            util.close();
                        } else if (inst_prop.get_state() & SDL_WINDOW_INPUT_FOCUS)
                        {
                            inst_prop.close();
                        } else if (disk_op.get_state() & SDL_WINDOW_INPUT_FOCUS)
                        {
                            disk_op.close();
                        } else {
                            util.open("Quit?", 0);
                        }
                    } else {
                        if (sequence_list.get_state() & SDL_WINDOW_SHOWN)
                        {
                            sequence_list.refresh();
                        } else if (util.get_state() & SDL_WINDOW_SHOWN)
                        {
                            util.render();
                        } else if (inst_prop.get_state() & SDL_WINDOW_SHOWN)
                        {
                            inst_prop.refresh();
                        } else if (disk_op.get_state() & SDL_WINDOW_SHOWN)
                        {
                            disk_op.refresh();
                        }
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    SDL_GetMouseState(&xM, &yM);
                    if (sequence_list.get_state() & SDL_WINDOW_INPUT_FOCUS)
                    {
                        sequence_list.mouse(xM, yM);
                    } else if (util.get_state() & SDL_WINDOW_INPUT_FOCUS)
                    {
                        util.mouse(xM, yM);
                    } else if (inst_prop.get_state() & SDL_WINDOW_INPUT_FOCUS)
                    {
                        inst_prop.mouse(xM, yM);
                    } else if (disk_op.get_state() & SDL_WINDOW_INPUT_FOCUS)
                    {
                        disk_op.mouse(xM, yM);
                    } else {
                        tracker.mouse(xM, yM);
                    }
                    break;

                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_q && (SDL_GetModState() & KMOD_CTRL) && (SDL_GetModState() & KMOD_SHIFT))
                    {
                        util.open("Quit?", 0);
                    }
                    if ((SDL_GetModState() & KMOD_CTRL) && (SDL_GetModState() & KMOD_ALT))
                    {
                        tracker.tracker_running = true;
                        tracker.run_sequence = false;
                        tracker.pos = 0;
                        tracker.note_trigger();
                        aworks.sample_count = 0;
                    }
                    if (e.key.keysym.sym == SDLK_SPACE)
                    {
                        if ((SDL_GetModState() & KMOD_CTRL) && (SDL_GetModState() & KMOD_SHIFT))
                        {
                            tracker.tracker_running = true;
                            tracker.run_sequence = true;
                            tracker.pos = 0;
                            tracker.sq_pos = 0;
                            tracker.b_pos = tracker.sequence[0];
                            tracker.set_timing_delay();
                            tracker.update_info();
                            tracker.sequence_update = true;
                            tracker.block_update = true;
                            tracker.note_trigger();
                            aworks.sample_count = 0;
                        } else if (SDL_GetModState() & KMOD_CTRL)
                        {
                            tracker.tracker_running = true;
                            tracker.run_sequence = false;
                            tracker.note_trigger();
                            aworks.sample_count = 0;
                        } else if (SDL_GetModState() & KMOD_SHIFT)
                        {
                            tracker.tracker_running = true;
                            tracker.run_sequence = true;
                            tracker.note_trigger();
                            aworks.sample_count = 0;
                        } else {
                            audio_buffer.stop = true;
                            tracker.tracker_running = false;
                            tracker.run_sequence = false;
                        }
                    }
                    if (sequence_list.get_state() & SDL_WINDOW_INPUT_FOCUS)
                    {
                        sequence_list.keyboard(&e);
                    } else if (util.get_state() & SDL_WINDOW_INPUT_FOCUS)
                    {
                        util.input(&e);
                    } else if (inst_prop.get_state() & SDL_WINDOW_INPUT_FOCUS)
                    {
                        //keyboard input needed!!!
                    } else if (disk_op.get_state() & SDL_WINDOW_INPUT_FOCUS)
                    {
                        disk_op.keyboard(&e);
                    } else {
                        if (SDL_GetModState() & KMOD_CTRL) // control key press
                        {
                            if (e.key.keysym.sym == SDLK_b) // open block params
                            {
                                util.open("Block List", 1);
                            } else if (e.key.keysym.sym == SDLK_h) // open track params
                            {
                                util.open("Track Parameters", 2);
                            } else if (e.key.keysym.sym == SDLK_o)
                            {
                                sequence_list.open();
                            } else if (e.key.keysym.sym == SDLK_i)
                            {
                                inst_prop.open();
                            } else if (e.key.keysym.sym == SDLK_f)
                            {
                                disk_op.open();
                            }
                        }
                        tracker.keyboard(&e);
                        aworks.play_note(&e);
                    }
                    break;

                default:
                    break;
            }

        }

        currentTime = SDL_GetTicks64();
        if (currentTime - previousTime >= refresh_delay_ms)
        {
            previousTime = currentTime;
            // Rendering
            SDL_SetRenderDrawColor(tracker_render, pallet.bgd.r, pallet.bgd.g, pallet.bgd.b, 0xFF); // Background color
            SDL_RenderClear(tracker_render);

            SDL_SetRenderDrawColor(tracker_render, pallet.black.r, pallet.black.g, pallet.black.b, 0xFF); // Black

            tracker.render_info();

            tracker.render_steps();

            tracker.render_bars();

            SDL_RenderPresent(tracker_render); // Present image to screen

            tracker.dec_trigger_bars(); // apply decay to level bars

            if (util.command == 'Q')
            {
                run = false;
            }
            if (tracker.sequence_update && (sequence_list.get_state() & SDL_WINDOW_SHOWN))
            {
                tracker.sequence_update = false;
                sequence_list.update_list();
                util.update();
            }
            if (tracker.block_update && (util.get_state() & SDL_WINDOW_SHOWN))
            {
                tracker.block_update = false;
                util.update();
            }
            if (inst_prop.get_state() & SDL_WINDOW_INPUT_FOCUS && SDL_GetMouseState(&xM, &yM) & SDL_BUTTON_LMASK)
            {
                inst_prop.mouse(xM, yM);
            }
        }

        if (audio_buffer.read_pos != audio_buffer.write_pos)
        {
            SDL_LockAudio();
            aworks.audio_works(); // generate audio buffer data
            SDL_UnlockAudio();
            //audio_buffer.update = false;
        }

    }

    util.close_all();
    sequence_list.de_init();
    inst_prop.de_init();
    disk_op.de_init();
    free(audio_buffer.data);
    SDL_CloseAudio();
    SDL_DestroyRenderer(tracker_render);
    SDL_DestroyWindow(tracker_window);
    TTF_CloseFont(Font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
