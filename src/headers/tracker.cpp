#include "tracker.h"

Tracker::Tracker(SDL_Renderer *tracker_renderer, TTF_Font *gFont) { // default constructor with no arguments
    renderer = tracker_renderer;
    font = gFont;

    tracker_box.x = 20;
    tracker_box.y = 20;
    tracker_box.w = 1240;
    tracker_box.h = 680;

    sequence_display.x = 20;
    sequence_display.y = 0;
    sequence_display.w = 70;
    sequence_display.h = 20;

    block_display.x = 100;
    block_display.y = 0;
    block_display.w = 70;
    block_display.h = 20;

    sample_display.x = 180;
    sample_display.y = 0;
    sample_display.w = 80;
    sample_display.h = 20;

    blkname_displayrect.y = 700;
    blkname_displayrect.w = 200;
    blkname_displayrect.x = (1280/2)-(blkname_displayrect.w/2);
    blkname_displayrect.h = 20;

    cursor.x = 45;
    cursor.y = 360;
    cursor.w = 40;
    cursor.h = 17;

    sequence = (int*)malloc(sizeof(int));
    sequence[0] = 0;
    sequence_len = 1;

    total_blocks = 1;
    block[0].length = 64;
    block[0].speed = 4;
    block[0].name = "New Block";
    for (int c = 0; c < CHANNELS; c++)
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

    sequence_display_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2);
    block_display_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2);
    sample_display_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2);
    blkname_displaytex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 2, 2);
}

Tracker::~Tracker() {
    for (int b = 0; b < total_blocks; b++)
    {
        for (int c = 0; c < CHANNELS; c++)
        {
            free(block[b].channel[c]);
        }
    }
    free(sequence);
}

void Tracker::update_info()
{
    string text;
    text = block[0].name;
    int length = strlen(text.c_str());
    if ( length < 20)
    {
        for (int a = 0; a < 20 - length; a++)
        {
            text += " ";
        }
    }
    surf = TTF_RenderText_Solid(font, text.c_str(), color_black);
    SDL_DestroyTexture(blkname_displaytex);
    blkname_displaytex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    text.clear();
    text += "SEQ:";
    if (sq_pos < 100){text += "0";}
    if (sq_pos < 10){text += "0";}
    text += to_string(sq_pos);
    surf = TTF_RenderText_Solid(font, text.c_str(), color_black);
    SDL_DestroyTexture(sequence_display_tex);
    sequence_display_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    text.clear();
    text += "BLK:";
    if (b_pos < 100){text += "0";}
    if (b_pos < 10){text += "0";}
    text += to_string(b_pos);
    surf = TTF_RenderText_Solid(font, text.c_str(), color_black);
    SDL_DestroyTexture(block_display_tex);
    block_display_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    text.clear();
    text += "INST:";
    if (s_pos < 10){text += "0";}
    text += to_string(s_pos);
    surf = TTF_RenderText_Solid(font, text.c_str(), color_black);
    SDL_DestroyTexture(sample_display_tex);
    sample_display_tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
}

void Tracker::move_cursor(int position, int chn, int direction)
{
    // set cursor_pos and cursor_channel
    if (position == 1)
    {
        if (direction == 1)
        {
            if (cursor_pos < 6)
            {cursor_pos++;}
            else
            {cursor_pos = 0;
            if (cursor_channel < CHANNELS-1){cursor_channel++;}else{cursor_channel = 0;}
            }
        } else {
            if (cursor_pos > 0)
            {cursor_pos--;}
            else
            {cursor_pos = 6;
            if (cursor_channel > 0){cursor_channel--;}else{cursor_channel = CHANNELS-1;}
            }
        }
    } else if (chn > 0)
    {
        if (direction == 1)
        {
            if (cursor_channel < CHANNELS-1){cursor_channel++;}else{cursor_channel = 0;}
        } else {
            if (cursor_channel > 0){cursor_channel--;}else{cursor_channel = CHANNELS-1;}
        }
    }

    // set cursor rect positions
    int width;
    switch (cursor_channel) {
        case 0:
            width = 0;
            break;
        case 1:
            width = 153;
            break;
        case 2:
            width = 306;
            break;
        case 3:
            width = 460;
            break;
        case 4:
            width = 613;
            break;
        case 5:
            width = 767;
            break;
        case 6:
            width = 920;
            break;
        case 7:
            width = 1074;
            break;
        default:
            width = 0;
            break;
    }
    switch (cursor_pos) {
        case 0:
            cursor.x = 45 + width;
            cursor.w = 40;
            break;
        case 1:
            cursor.x = 84 + width;
            cursor.w = 12;
            break;
        case 2:
            cursor.x = 97 + width;
            cursor.w = 12;
            break;
        case 3:
            cursor.x = 110 + width;
            cursor.w = 12;
            break;
        case 4:
            cursor.x = 122 + width;
            cursor.w = 12;
            break;
        case 5:
            cursor.x = 135 + width;
            cursor.w = 12;
            break;
        case 6:
            cursor.x = 148 + width;
            cursor.w = 12;
            break;
        default:
            break;
    }
}

void Tracker::block_dec()
{
    if ((b_pos - 1) >= 0)
    {
        b_pos--;
    } else {
        b_pos = total_blocks - 1;
    }
    update_info();
}

void Tracker::block_inc()
{
    if ((b_pos + 1) < total_blocks)
    {
        b_pos++;
    } else {
        b_pos = 0;
    }
    update_info();
}

void Tracker::sample_inc()
{
    if (s_pos + 1 < MAXSAMPLES)
    {
        s_pos++;
    } else {
        s_pos = 0;
    }
    update_info();
}

void Tracker::sample_dec()
{
    if (s_pos - 1 >= 0)
    {
        s_pos--;
    } else {
        s_pos = MAXSAMPLES - 1;
    }
    update_info();
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

void Tracker::render_info()
{
    SDL_RenderCopy(renderer, blkname_displaytex, NULL, &blkname_displayrect);
    SDL_RenderDrawRect(renderer, &tracker_box);
    SDL_RenderCopy(renderer, sequence_display_tex, NULL, &sequence_display);
    SDL_RenderCopy(renderer, block_display_tex, NULL, &block_display);
    SDL_RenderCopy(renderer, sample_display_tex, NULL, &sample_display);
    SDL_RenderDrawRect(renderer, &cursor);
}

void Tracker::render_steps() // Renders block data to screen
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
            if (edit_mode)
            {
                surf = TTF_RenderText_Solid(font, step_data.c_str(), color_red); // Makes surface with text
            } else {
                surf = TTF_RenderText_Solid(font, step_data.c_str(), color_blue); // Makes surface with text
            }
        } else {
            surf = TTF_RenderText_Solid(font, step_data.c_str(), color_black);
        }
        SDL_DestroyTexture(displaytextures[step]); // destroys old texture. If not destroyed there will be a memory leak.
        displaytextures[step] = SDL_CreateTextureFromSurface(renderer, surf); // Coverts surface to texture
        SDL_RenderCopy(renderer, displaytextures[step], NULL, &displayrects[step]); // Renderes texture to screen
        SDL_FreeSurface(surf); // frees surface otherwise there will be a memory leak
    }
}

void Tracker::keyboard(SDL_Event *e)
{
    switch (e->key.keysym.sym) // Keyboard input
    {
        case SDLK_UP:
            if (SDL_GetModState() & KMOD_SHIFT)
            {
                block_dec();
            } else {
                decpos(1);
            }
            break;
        case SDLK_DOWN:
            if (SDL_GetModState() & KMOD_SHIFT)
            {
                block_inc();
            } else {
                incpos(1);
            }
            break;
        case SDLK_RIGHT:
            if (SDL_GetModState() & KMOD_SHIFT)
            {
                sample_inc();
            } else {
                move_cursor(1, 0, 1);
            }
            break;
        case SDLK_LEFT:
            if (SDL_GetModState() & KMOD_SHIFT)
            {
                sample_dec();
            } else {
                move_cursor(1, 0, 0);
            }
            break;
        case SDLK_ESCAPE:
            if (edit_mode){edit_mode = false;} else {edit_mode = true;}
            break;
        case SDLK_TAB:
            if (SDL_GetModState() & KMOD_SHIFT)
            {
                move_cursor(0, 1, 0);
            } else {
                move_cursor(0, 1, 1);
            }
            break;
        default:
            break;
    }
}
