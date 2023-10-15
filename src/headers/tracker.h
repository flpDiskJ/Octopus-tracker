#ifndef TRACKER_H
#define TRACKER_H

#include "global.h"

class Tracker
{
private:
    SDL_Surface *surf = NULL;
    SDL_Color color_black = {0, 0, 0}, color_red = {200, 0, 0};
    SDL_Rect displayrects[DISPLAYRECTS]; // 1280x18 // Used to display sequence steps
    SDL_Texture *displaytextures[DISPLAYRECTS];

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

    Block *block;
    int total_blocks = 0;
    int *sequence; // array of block numbers ex. block[sequence[s_pos]]
    int sequence_len = 0;
    SDL_Rect blkname_displayrect;
    SDL_Texture *blkname_displaytex;
    int master_tempo = 120; // master bpm
    int master_speed = 4; // steps per beat
    // note: if master speed is changed: ask user if the speed in the existing blocks should be set to match or left as is.
    int s_pos = 0; // sequence position
    int b_pos = 0; // block position
    int pos = 0; // step position
    bool mute[8]; // used to mute channel
    SDL_Rect tracker_box;

public:
    TTF_Font* gFont = NULL;

    Tracker(SDL_Renderer *renderer); // default constructor, add argument for SDL_Renderer?

    ~Tracker(); // default destructor, cleans up memory for Tracker object

    void incpos(int amount);

    void decpos(int amount);

    void clear_block(int blk);

    void render_info(SDL_Renderer *renderer);

    void render_steps(SDL_Renderer *renderer);

};

#endif
