#include "moduleformat.h"

ModuleFormat::ModuleFormat(Tracker *t_pointer, AudioW *a_pointer)
{
    t = t_pointer;
    aworks = a_pointer;
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
        header.module_name[t->track_name.length()] = '\0';
    }
    if (t->track_artist.length() >= 20)
    {
        strncpy(header.module_artist, t->track_artist.c_str(), 20);
        header.module_artist[19] = '\0';
    } else {
        strncpy(header.module_artist, t->track_artist.c_str(), t->track_artist.length());
        header.module_artist[t->track_artist.length()] = '\0';
    }
    if (t->track_date.length() >= 20)
    {
        strncpy(header.module_date, t->track_date.c_str(), 20);
        header.module_date[19] = '\0';
    } else {
        strncpy(header.module_date, t->track_date.c_str(), t->track_date.length());
        header.module_date[t->track_date.length()] = '\0';
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
        block_spec.name[t->block[b].name.length()] = '\0';
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
        sample_spec.name[t->sample[index].name.length()] = '\0';
    }

    sample_spec.length = t->sample[index].len;
    sample_spec.level = t->sample[index].level;
    sample_spec.transpose = t->sample[index].tune;
    sample_spec.finetune = t->sample[index].fine_tune;
    sample_spec.loop = t->sample[index].loop;
    sample_spec.loop_point = t->sample[index].loop_point;
    sample_spec.original_rate = t->sample[index].sample_rate;
}

bool ModuleFormat::save_module(string path)
{
    setup_mod_head();
    FILE *fp = fopen(path.c_str(), "wb");
    if (fp == NULL)
    {
        return false;
    }

    fwrite(&id, sizeof(Octo_ID), 1, fp);

    fwrite(&header, sizeof(ModuleHead), 1, fp);
    fwrite(t->sequence, sizeof(Uint8), t->sequence_len, fp);

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
    FILE *fp = fopen(path.c_str(), "rb");
    if (fp == NULL)
    {
        return false;
    }

    fread(&id, sizeof(Octo_ID), 1, fp);

    if (id.identifier[0] != 'O' || id.identifier[1] != 'C' || id.identifier[2] != 'T' || id.identifier[3] != 'O')
    {
        fclose(fp);
        return false;
    }

    fread(&header, sizeof(ModuleHead), 1, fp);
    if (t->sequence != NULL)
    {
        free(t->sequence);
    }
    t->sequence_len = t->sequence_size = header.sequence_length;
    t->sequence_size += t->seq_size_chunk;
    t->sequence = (Uint8*)malloc(sizeof(Uint8)*t->sequence_size);
    fread(t->sequence, sizeof(Uint8), t->sequence_len, fp);
    t->master_tempo = header.master_tempo;
    t->total_blocks = header.number_of_blocks;
    t->track_name.clear();
    t->track_name = string(header.module_name);
    t->track_artist.clear();
    t->track_artist = string(header.module_artist);
    t->track_date.clear();
    t->track_date = string(header.module_date);

    for (int b = 0; b < t->total_blocks; b++)
    {
        fread(&block_spec, sizeof(BlockHead), 1, fp);
        t->block[b].name.clear();
        t->block[b].name = string(block_spec.name);
        t->block[b].speed = block_spec.speed;
        t->allocate_block(b, block_spec.length);

        for (int c = 0; c < CHANNELS; c++)
        {
            for (int step = 0; step < block_spec.length; step++)
            {
                fread(&note_data, sizeof(BlockNote), 1, fp);
                t->block[b].channel[c][step].note = note_data.note;
                t->block[b].channel[c][step].key = note_data.key;
                t->block[b].channel[c][step].octave = note_data.octave;
                t->block[b].channel[c][step].sample = note_data.sample;
                strncpy(t->block[b].channel[c][step].command, note_data.command, 3);
                strncpy(t->block[b].channel[c][step].parameter, note_data.parameter, 3);
            }
        }
    }

    for (int s = 0; s < header.number_of_samples; s++)
    {
        fread(&sample_spec, sizeof(SampleHead), 1, fp);
        t->sample[sample_spec.index].name.clear();
        t->sample[sample_spec.index].name = string(sample_spec.name);
        t->sample[sample_spec.index].len = sample_spec.length;
        t->sample[sample_spec.index].level = sample_spec.level;
        t->sample[sample_spec.index].tune = sample_spec.transpose;
        t->sample[sample_spec.index].fine_tune = sample_spec.finetune;
        t->sample[sample_spec.index].loop = sample_spec.loop;
        t->sample[sample_spec.index].loop_point = sample_spec.loop_point;
        t->sample[sample_spec.index].sample_rate = sample_spec.original_rate;

        if (t->sample[sample_spec.index].data != NULL)
        {
            free(t->sample[sample_spec.index].data);
        }
        t->sample[sample_spec.index].data = (Sint16*)malloc(sizeof(Sint16)*sample_spec.length);
        fread(t->sample[sample_spec.index].data, sizeof(Sint16), sample_spec.length, fp);
    }

    fclose(fp);
    t->set_timing_delay();
    return true;
}

void ModuleFormat::export_wav(string path)
{
    AudioFile<int> audioFile;
    Uint32 signal_length = aworks->prepare_export();
    audioFile.setAudioBufferSize (1, signal_length);
    audioFile.setBitDepth (16);
    audioFile.setSampleRate (SAMPLE_RATE);
    for (int x = 0; x < signal_length; x++)
    {
        audioFile.samples[0][x] = aworks->wav_data[x];
    }
    audioFile.save (path, AudioFileFormat::Wave);
    free(aworks->wav_data);
}
