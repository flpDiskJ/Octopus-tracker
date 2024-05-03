#pragma once

#include "global.h"

class Tracker
{
private:
    SDL_Surface *surf = NULL;
    Pallet *p;
    SDL_Rect displayrects[DISPLAYRECTS]; // Used to display sequence steps
    SDL_Texture *displaytextures[DISPLAYRECTS];
    SDL_Renderer *renderer;
    TTF_Font *font;

    struct Note{ // holds data for single note
        char note = '-'; // A, B, C, D, E, F, G or - for blank
        char key = '-'; // - or #
        Uint8 octave = 0; // 1 thru 6 or 0 for blank
        Uint8 sample = 0; // 0 - 99
        char command[3];
        char parameter[3]; // parameters for command
    };

    struct Block{ // holds all data for block
        Note *channel[CHANNELS];
        Uint16 length = 0; // number of Notes in the block
        Uint8 speed = 0; // steps per beat
        string name;
    };

    struct Buffer{ // used to copy and paste channels
        Note *data = NULL;
        int length = 0;
    };

    struct Instrument{ // data for each instrument/sample
        Sint16 *data = NULL;
        Uint32 len = 0;
        Uint8 level = 0; // 0-100
        Sint8 tune = 0; // -12 to 12
        Sint8 fine_tune = 0; // -12 to 12
        string name;
        Uint16 sample_rate = 0;
        Uint8 loop = 0; // 0 = no loop, 1 = normal loop, 2 = ping pong
        Uint32 loop_point = 0;
    };

    struct Channel{ // set these values when note is triggered
        bool play = false; // set to true to play audio

        ///// command bs
        Uint8 command_type = COM_NONE;
        unsigned int command_param[3]; // 0 = first slot only, 1 = second slot only, 2 = combined value
        Uint8 octave = 0; // octave of current note

        // 00 Arpeggio
        unsigned int arp_toggle = 0;
        unsigned int arp_rates[3];

        // 01 02 03
        unsigned int slide_pos = 0; // sample rate of pitch
        unsigned int slide_speed = 0;
        // 03
        unsigned int slide_target = 0; // target sample rate

        // 04 Vibrato
        unsigned int vib_high = 0; // high pitch limit
        unsigned int vib_low = 0; // low pitch limit
        unsigned int vib_pos = 0;
        bool vib_up = false; // slide direction toggle
        unsigned int vib_speed = 0;

        // 07 Tremolo
        unsigned int trem_start = 0; // Note: can be used to reference current volume
        unsigned int trem_depth = 0;
        unsigned int trem_current = 0;
        unsigned int trem_speed = 0;
        bool trem_up = false;

        // 08 hold and decay (uses trem_current to keep current volume)
        bool hold_and_decay = false;
        unsigned int hold = 0;
        unsigned int decay = 0;

        // 05 delay & 06 retrigger
        unsigned int trigger_pos = 0;
        unsigned int total_triggers = 0;
        unsigned int retriggers = 0;
        unsigned int retrig_freq = 0;
        unsigned int retrig_count = 0;

        // 0A
        bool reverse = false;

        double amplifier = 1; // control level. calculate: desired_level / 100.0
        //////

        int sample = 0; // sample to play.
        double pos = 0; // curent position of sample in channel
        double pos_adv = 0; // amount to advance pos // calculate: Desired rate / SAMPLE_RATE // multiplied by pitch_mod every advance
    };

    SDL_Rect tracker_box; // only functions for design (box around tracker)
    SDL_Rect octave_display;
    SDL_Rect sample_name;
    SDL_Rect sequence_display; // displays sequence position
    SDL_Rect block_display; // displays block position
    SDL_Rect sample_display; // displays sample position
    SDL_Rect skip_display;
    bool round_skip = false; // if true round skip to nearest multiplier else incpos by skip amount
    int skip_mem = 1; // stores previous skip val when toggled.
    SDL_Texture *sequence_display_tex;
    SDL_Texture *block_display_tex;
    SDL_Texture *sample_display_tex;
    SDL_Texture *octave_display_tex;
    SDL_Texture *sample_name_tex;
    SDL_Texture *skip_display_tex;
    SDL_Rect cursor; // used to display cursor
    int trigger_bar_decay = 0;

    bool check_command(int c, const char *command);

    int hex2dec(char hex); // converts single hex digit to decimal

    Uint8 get_command(int c); // returns command type flag defined in global.h

    void copy_channel();

    void paste_channel();

    void copy_block(int blk);

    void paste_block(int blk);

    void create_block(bool insert); // creates new block. false = append to end / true = insert to current b_pos

    void move_cursor(int position, int chn, int direction); // moves cursor_pos
    // (1, 0, 1) moves cursor right 1
    // (1, 0, 0) moves cursor left 1
    // (0, 1, 1) moves cursor to start of next channel and so on...

    void decpos(); // decrement step position

    void clear_step(); // clears current step

    void clear_channel(); // clears current channel

    void clear_block(int blk); // set all values to default on indicated block

    void clear_index(); // sets cursor_pos index to 0

    void get_note(SDL_Event *e); // handles keyboard notes

    bool checkButton(SDL_Rect *button, int x, int y); // checks if mouse was inside button

    void copy_note(int buff); // copy/paste note to/from note_buffer

    void paste_note(int buff); // copy/paste note to/from note_buffer

public:

    bool edit_mode = false;
    Block block[MAXBLOCKS]; // static array of blocks
    Block block_buffer; // used to copy blocks
    Buffer channel_buffer; // used to copy channels
    Note note_buffer[10]; // used to copy notes
    Uint8 total_blocks = 0;
    Instrument sample[MAXSAMPLES]; // static array of instruments/samples
    Note default_pitch; // only use note, key, and octave (default is C-3)
    Uint8 *sequence; // array of block numbers ex. block[sequence[s_pos]]
    const int seq_size_chunk = 30;
    Uint32 sequence_size = 0; // allocated size of sequence
    Uint16 sequence_len = 0; // num of sequence slots in use
    bool sequence_update = false; // set to true to update sequence sub-window
    bool block_update = false; // set to true to update block sub-window
    bool run_sequence = false; // if true the song plays out otherwise the block loops
    bool tracker_running = false; // if true the tracker runs
    SDL_Rect blkname_displayrect;
    SDL_Texture *blkname_displaytex;
    int master_tempo = 120; // master bpm
    // Meta data
    string track_name = "UNKNOWN"; // Name of the entire track
    string track_artist = "UNKNOWN"; // Artist(s) that created the track
    string track_date = "UNKNOWN"; // date track was created
    // note: if master speed is changed: ask user if the speed in the existing blocks should be set to match or left as is.
    int sq_pos = 0; // sequence position
    int b_pos = 0; // block position
    int pos = 0; // step position
    int skip = 1; // number of steps to incriment when note is entered
    int s_pos = 0; //sample position
    int cursor_channel = 0; // the channel that the cursor is inside
    int cursor_pos = 0; // which piece of data the cursor is on inside of the channel
    int octave = 2;
    bool mute[CHANNELS]; // used to mute channel
    Channel channel[CHANNELS];
    SDL_Rect trigger_bars[CHANNELS];
    Uint64 timing_delay; // time delay in number of audio samples // calculate using set_timing_delay()
    bool sequence_end = false; // set to true when entire sequence has elapsed

    struct Scope{
        SDL_Texture *t = NULL;
        SDL_Rect r;
        int *data = NULL;
        int data_size = 0;
        int data_pos = 0;
        bool active = false;
        bool texture_ready = false;
    };

    Scope scope[CHANNELS];
    SDL_Texture *scope_idle_t;

    Tracker(SDL_Renderer *tracker_renderer, TTF_Font *gFont, Pallet *pallet);
    // default constructor, sets up rects, textures, and copies renderer and font pointers

    ~Tracker(); // default destructor, cleans up memory for Tracker object

    void note_trigger(); // triggers all valid notes in step positon

    void mouse(int x, int y); // handles mouse input

    void set_timing_delay(); // calculates timing delay based on tempo and block speed

    void set_trigger_bar(int channel, int level); // sets trigger bar

    void dec_trigger_bars(); // run in main loop to decay bars

    void low_pass(int sample_slot, int cutoff, int sample_rate); // low pass filter

    void resample(int sample_slot, int old_rate, int new_rate);

    bool load_inst(string path, string name, int sample_slot); // loads wav into instrument slot of s_pos | returns true if successful

    int getFreq(char note, char key, int oct, int sample_id); // returns sample rate of note
    // pass -1 to sample_id to bypass tune and finetune

    void incpos(); // increment step position

    void block_inc(); // increment block position

    void block_dec(); // decrement block position

    void sample_inc(); // increment sample position

    void sample_dec(); // decrement sample position

    void move_step(); // used by timer to run tracker

    void sequencer(Uint8 flag); // append, insert, delete, alter sequence element

    void realloc_block(int size); // reallocates block at b_pos

    void allocate_block(int blk, Uint16 size);

    void delete_block(int blk); // deletes indicated block

    void update_info(); // re-creates textures for sequence, block, sample, etc position displays

    void render_info(); // render info (same stuff in update info) to the screen

    void update_steps(); // re-generates step textures.

    void render_steps(); // creates textures and renders tracker steps to screen

    void render_bars(); // renders level bars

    void keyboard(SDL_Event *e); // handles keyboard input specific to the main window
};
