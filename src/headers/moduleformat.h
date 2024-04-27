#pragma once

#include "global.h"
#include "tracker.h"

// FORMATING
/**

    ModuleHead

    BlockHead
    (followed by full note data (block chunks) for the block)
    (all notes in channel starting at channel 0 and incrementing up)
    (repeats for each block)

    SampleHead
    (followed by Sint16 array of sample data)
    (repeats for each sample)

**/


class ModuleFormat{
private:
    struct ModuleHead{
        char module_name[20];
        char module_artist[20];
        char module_date[20];
        Uint8 number_of_blocks;
        Uint16 sequence_length;
        Uint8 number_of_samples;
    };

    struct BlockHead{
        Uint16 length;
        Uint32 total_length; // length * channels
        Uint8 speed;
        char name[20];
    };

    struct BlockNote{
        char note;
        char key;
        Uint8 octave;
        Uint8 sample;
        char command[3];
        char parameter[3];
    };

    struct SampleHead{
        char name[20];
        Uint32 length;
        Uint8 level;
        Sint8 transpose;
        Sint8 finetune;
        Uint8 loop;
        Uint32 loop_point;
    };

    Tracker *t;
    ModuleHead header;
    BlockHead block_spec;
    BlockNote note_data;
    SampleHead sample_spec;

public:

    ModuleFormat(Tracker *t_pointer);

    ~ModuleFormat();

    bool save_module(string path);

    bool load_module(string path);

};
