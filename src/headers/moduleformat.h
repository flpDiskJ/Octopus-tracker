#pragma once

#include "global.h"
#include "tracker.h"
#include "AudioFile.h"
#include "audioworks.h"

// FORMATING
/**

    ModuleHead

    Sequence data

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
        Uint16 master_tempo;
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
        Uint8 index; // sample slot
        char name[20];
        Uint32 length;
        Uint8 level;
        Sint8 transpose;
        Sint8 finetune;
        Uint8 loop;
        Uint32 loop_point;
        Uint16 original_rate;
    };

    Tracker *t;
    AudioW *aworks;
    ModuleHead header;
    BlockHead block_spec;
    BlockNote note_data;
    SampleHead sample_spec;

    void setup_mod_head();

    void setup_block_head(int b);

    void setup_block_note(int b, int c, int s);

    void setup_sample_head(int index);

public:

    ModuleFormat(Tracker *t_pointer, AudioW *a_pointer);

    ~ModuleFormat();

    bool save_module(string path);

    bool load_module(string path);

    void export_wav(string path);
};
