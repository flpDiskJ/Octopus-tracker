#include "moduleformat.h"

ModuleFormat::ModuleFormat(Tracker *t_pointer)
{
    t = t_pointer;
}

ModuleFormat::~ModuleFormat()
{

}

void ModuleFormat::setup_mod_head()
{
    if (t->track_name.length() >= 20)
    {
        strncpy(header.module_name, t->track_name.c_str(), 20);
        header.module_name[19] = '\0';
    } else {
        strncpy(header.module_name, t->track_name.c_str(), t->track_name.length());
    }
    if (t->track_artist.length() >= 20)
    {
        strncpy(header.module_artist, t->track_artist.c_str(), 20);
        header.module_artist[19] = '\0';
    } else {
        strncpy(header.module_artist, t->track_artist.c_str(), t->track_artist.length());
    }
    if (t->track_date.length() >= 20)
    {
        strncpy(header.module_date, t->track_date.c_str(), 20);
        header.module_date[19] = '\0';
    } else {
        strncpy(header.module_date, t->track_date.c_str(), t->track_date.length());
    }

    header.number_of_blocks = t->total_blocks;
    header.sequence_length = t->sequence_len;
    header.number_of_samples = 0;
    for (int x = 0; x < MAXSAMPLES; x++)
    {
        if (t->sample[x].len > 0)
        {
            header.number_of_samples++;
        }
    }
    header.master_tempo = (Uint16)t->master_tempo;
}

void ModuleFormat::setup_block_head(int b)
{
    block_spec.length = t->block[b].length;
    block_spec.total_length = block_spec.length * CHANNELS;
    block_spec.speed = t->block[b].speed;

    if (t->block[b].name.length() >= 20)
    {
        strncpy(block_spec.name, t->block[b].name.c_str(), 20);
        block_spec.name[19] = '\0';
    } else {
        strncpy(block_spec.name, t->block[b].name.c_str(), t->block[b].name.length());
    }
}

void ModuleFormat::setup_block_note(int b, int c, int s)
{
    note_data.note = t->block[b].channel[c][s].note;
    note_data.key = t->block[b].channel[c][s].key;
    note_data.octave = t->block[b].channel[c][s].octave;
    note_data.sample = t->block[b].channel[c][s].sample;
    strncpy(note_data.command, t->block[b].channel[c][s].command, 3);
    strncpy(note_data.parameter, t->block[b].channel[c][s].parameter, 3);
}

void ModuleFormat::setup_sample_head(int index)
{
    sample_spec.index = (Uint8)index;

    if (t->sample[index].name.length() >= 20)
    {
        strncpy(sample_spec.name, t->sample[index].name.c_str(), 20);
        sample_spec.name[19] = '\0';
    } else {
        strncpy(sample_spec.name, t->sample[index].name.c_str(), t->sample[index].name.length());
    }

    sample_spec.length = t->sample[index].len;
    sample_spec.level = t->sample[index].level;
    sample_spec.transpose = t->sample[index].tune;
    sample_spec.finetune = t->sample[index].fine_tune;
    sample_spec.loop = t->sample[index].loop;
    sample_spec.loop_point = t->sample[index].loop_point;
}

bool ModuleFormat::save_module(string path)
{
    setup_mod_head();

    FILE *fp = fopen(path.c_str(), "wb");
    if (fp == NULL)
    {
        return false;
    }

    fwrite(&header, sizeof(ModuleHead), 1, fp);

    for (int b = 0; b < t->total_blocks; b++)
    {
        setup_block_head(b);

        fwrite(&block_spec, sizeof(BlockHead), 1, fp);

        for (int c = 0; c < CHANNELS; c++)
        {
            for (int step = 0; step < block_spec.length; step++)
            {
                setup_block_note(b, c, step);
                fwrite(&note_data, sizeof(BlockNote), 1, fp);
            }
        }
    }

    for (int s = 0; s < MAXSAMPLES; s++)
    {
        if (t->sample[s].len > 0)
        {
            setup_sample_head(s);
            fwrite(&sample_spec, sizeof(SampleHead), 1, fp);
            fwrite(t->sample[s].data, sizeof(Sint16), sample_spec.length, fp);
        }
    }

    fclose(fp);
    return true;
}

bool ModuleFormat::load_module(string path)
{
    return true;
}
