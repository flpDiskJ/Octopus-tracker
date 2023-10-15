#include "tracker.h"

Tracker::Tracker(SDL_Renderer *renderer) { // default constructor with no arguments
    tracker_box.x = 20;
    tracker_box.y = 20;
    tracker_box.w = 1240;
    tracker_box.h = 680;

    blkname_displayrect.y = 701;
    blkname_displayrect.w = 50;
    blkname_displayrect.x = (1280/2)-(blkname_displayrect.w/2);
    blkname_displayrect.h = 18;

    sequence = (int*)malloc(sizeof(int));
    sequence[0] = 0;
    sequence_len = 1;

    block = (Block*)malloc(sizeof(Block));
    total_blocks = 1;
    block[0].length = 64;
    block[0].speed = 4;
    block[0].name = "New Block";
    for (int c = 0; c < 8; c++)
    {
        block[0].channel[c] = (Note*)malloc(64*sizeof(Note));
        mute[c] = false;
    }
    clear_block(0);

    for (int s = 0; s < DISPLAYRECTS; s++) // init step display rects
    {
        displayrects[s].x = 20;
        displayrects[s].y = (s * 17)+20;
        displayrects[s].w = 1240;
        displayrects[s].h = 17;
    }
    for (int s = 0; s < DISPLAYRECTS; s++) // initializes throwaway textures so that SDL_DestroyTexture() doesn't throw an error.
    {
        displaytextures[s] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2);
    }
    surf = TTF_RenderText_Solid(gFont, block[0].name.c_str(), color_black);
    blkname_displaytex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
}

Tracker::~Tracker() {
    for (int b = 0; b < total_blocks; b++)
    {
        for (int c = 0; c < CHANNELS; c++)
        {
            free(block[b].channel[c]);
        }
    }
    free(block);
    free(sequence);
}

void Tracker::incpos(int amount) // Incriment pos by amount
{
    if ((pos + amount) < block[b_pos].length)
    {
        pos += amount;
    } else {
        pos = amount - (block[b_pos].length - pos);
    }
}

void Tracker::decpos(int amount)
{
    if ((pos - amount) >= 0)
    {
        pos -= amount;
    } else {
        pos = block[b_pos].length - (amount - pos);
    }
}

void Tracker::clear_block(int blk) // Clears indicated block
{
    for (int c = 0; c < CHANNELS; c++)
    {
        for (int s = 0; s < block[blk].length; s++)
        {
            block[blk].channel[c][s].note = '-';
            block[blk].channel[c][s].key = '-';
            block[blk].channel[c][s].octave = '-';
            block[blk].channel[c][s].sample[0] = '0';
            block[blk].channel[c][s].sample[1] = '0';
            block[blk].channel[c][s].command[0] = '0';
            block[blk].channel[c][s].command[1] = '0';
            block[blk].channel[c][s].parameter[0] = '0';
            block[blk].channel[c][s].parameter[1] = '0';
        }
    }
}

void Tracker::render_info(SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, blkname_displaytex, NULL, &blkname_displayrect);
    SDL_RenderDrawRect(renderer, &tracker_box);
}

void Tracker::render_steps(SDL_Renderer *renderer) // Renders block data to screen
{
    string step_data;
    int step_pos = 0;
    for (int step = 0; step < DISPLAYRECTS; step++)
    {
        step_pos = pos + (step - (DISPLAYRECTS / 2)); // calculates position offset
        step_data.clear();
        if (step_pos < block[b_pos].length && step_pos >= 0) // Checks if position is in bounds of the block
        {
            // combines all elements into one string
            step_data += "> ";
            for (int chan = 0; chan < CHANNELS; chan++)
            {
                step_data += block[b_pos].channel[chan][step_pos].note;
                step_data += block[b_pos].channel[chan][step_pos].key;
                step_data += block[b_pos].channel[chan][step_pos].octave;
                step_data += block[b_pos].channel[chan][step_pos].sample[0];
                step_data += block[b_pos].channel[chan][step_pos].sample[1];
                step_data += block[b_pos].channel[chan][step_pos].command[0];
                step_data += block[b_pos].channel[chan][step_pos].command[1];
                step_data += block[b_pos].channel[chan][step_pos].parameter[0];
                step_data += block[b_pos].channel[chan][step_pos].parameter[1];
                if (chan < CHANNELS-1) {step_data += " | ";}
            }
            step_data += " <";
        } else {
            step_data = " "; // renders empty step if position is out of bounds
        }
        if (step_pos == pos) // Renders the cursor positon red instead of black
        {
            surf = TTF_RenderText_Solid(gFont, step_data.c_str(), color_red); // Makes surface with text
        } else {
            surf = TTF_RenderText_Solid(gFont, step_data.c_str(), color_black);
        }
        SDL_DestroyTexture(displaytextures[step]); // destroys old texture. If not destroyed there will be a memory leak.
        displaytextures[step] = SDL_CreateTextureFromSurface(renderer, surf); // Coverts surface to texture
        SDL_RenderCopy(renderer, displaytextures[step], NULL, &displayrects[step]); // Renderes texture to screen
        SDL_FreeSurface(surf); // frees surface otherwise there will be a memory leak
    }
}
