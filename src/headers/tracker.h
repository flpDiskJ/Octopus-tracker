#ifndef TRACKER_H
#define TRACKER_H

#include "global.h"

class Tracker
{
private:
    SDL_Surface *surf = NULL;
    SDL_Color color_black = {0, 0, 0}, color_red = {200, 0, 0}, color_blue = {0, 0, 180};
    SDL_Rect displayrects[DISPLAYRECTS]; // 1280x18 // Used to display sequence steps
    SDL_Texture *displaytextures[DISPLAYRECTS];
    SDL_Renderer *renderer;
    TTF_Font *font;

    struct Note{
        char note; // A, B, C, D, E, F, G or - for blank
        char key; // - or #
        char octave; // 1 thru 5 or - for blank
        char sample[2]; // 00 - 99
        char command[2];
        char parameter[2];
    };
    struct Block{
        Note *channel[CHANNELS];
        int length;
        int speed;
        string name;
    };

    struct Sample{
        uint16_t *data;
        int length;
        int level;
        int tune;
        string name;
    };

    Block block[MAXBLOCKS]; // static array of blocks
    int total_blocks = 0;
    Sample sample[MAXSAMPLES]; // static array of samples
    int *sequence; // array of block numbers ex. block[sequence[s_pos]]
    int sequence_len = 0;
    SDL_Rect blkname_displayrect;
    SDL_Texture *blkname_displaytex;
    int master_tempo = 120; // master bpm
    int master_speed = 4; // steps per beat
    // note: if master speed is changed: ask user if the speed in the existing blocks should be set to match or left as is.
    int sq_pos = 0; // sequence position
    int b_pos = 0; // block position
    int pos = 0; // step position
    int s_pos = 0; //sample position
    bool mute[8]; // used to mute channel
    SDL_Rect tracker_box; // only functions for design (box around tracker)
    SDL_Rect sequence_display; // displays sequence position
    SDL_Rect block_display; // displays block position
    SDL_Rect sample_display; // displays sample position
    SDL_Texture *sequence_display_tex;
    SDL_Texture *block_display_tex;
    SDL_Texture *sample_display_tex;
    bool edit_mode = false;
    SDL_Rect cursor; // used to display cursor
    int cursor_channel = 0; // the channel that the cursor is inside
    int cursor_pos = 0; // which piece of data the cursor is on inside of the channel

public:
    Tracker(SDL_Renderer *tracker_renderer, TTF_Font *gFont);
    // default constructor, sets up rects, textures, and copies renderer and font pointers

    ~Tracker(); // default destructor, cleans up memory for Tracker object

    void update_info(); // re-creates textures for sequence, block, sample, etc position displays

    void move_cursor(int position, int chn, int direction); // moves cursor_pos
    // (1, 0, 1) moves cursor right 1
    // (1, 0, 0) moves cursor left 1
    // (0, 1, 1) moves cursor to start of next channel and so on...

    void block_inc(); // incriment block position

    void block_dec(); // decriment block position

    void sample_inc(); // incriment sample position

    void sample_dec(); // decriment sample position

    void incpos(int amount); // incriment step position by amount

    void decpos(int amount); // decriment step position by amount

    void clear_block(int blk); // set all values to default on indicated block

    void render_info(); // render info (same stuff in update info) to the screen

    void render_steps(); // creates textures and renders tracker steps to screen

    void keyboard(SDL_Event *e); // handles keyboard input specific to the main window
};

#endif
