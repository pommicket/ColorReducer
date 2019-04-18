/*
    Copyright (C) 2019 Leo Tenenbaum
    This file is part of cutils.

    cutils is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cutils is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cutils.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../misc/types_exact.h"
#include "../misc/error.h"

struct u_audio {
    u_u16_t* samples;
    u_u32_t sample_rate;
    u_u16_t nchannels;
    u_u32_t nsamples;
};

#if CHAR_BIT != 8
#error "Invalid value of CHAR_BIT."
#endif

u_byte_t a;


typedef struct {
    char    chunk_id[4];
    u_u4b_t chunk_size;
    u_i1b_t format[4];
    char    subchunk1_id[4];
    u_u4b_t subchunk1_size;
    u_u2b_t audio_format;
    u_u2b_t nchannels;
    u_u4b_t sample_rate;
    u_u4b_t byte_rate;
    u_u2b_t block_align;
    u_u2b_t bits_per_sample;
} wav_header_t;

u_audio_t* u_audio_read(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        char filename_short[256];
        strncpy(filename_short, filename, 255);
        sprintf(u_error_message,
                "Could not read audio file: %s", filename);
        u_error_code = U_ERROR_ACCESS;
        return NULL;
    }
    u_audio_t* audio = malloc(sizeof(u_audio_t));
    wav_header_t header;
    if (fread(&header, sizeof(header), 1, fp) != 1) {
        fclose(fp);
        free(audio);
        u_error_set(U_ERROR_FORMAT, "Invalid .wav file.");
        return NULL;
    }
    /* Check header properties */
    if (strncmp(header.chunk_id, "RIFF", 4)) {
        fclose(fp);
        free(audio);
        u_error_set(U_ERROR_FORMAT, "Invalid .wav chunk ID.");
        return NULL;
    }
    if (strncmp(header.format, "WAVE", 4)) {
        fclose(fp);
        free(audio);
        u_error_set(U_ERROR_FORMAT, "Invalid .wav format.");
        return NULL;
    }
    /* Get metadata */

    if (header.bits_per_sample != CHAR_BIT &&
        header.bits_per_sample != 2*CHAR_BIT &&
        header.bits_per_sample != 4*CHAR_BIT) {
        fclose(fp);
        free(audio);
        u_error_set(U_ERROR_FORMAT, "Invalid wav bits per sample (must be 1, 2, or 4 bytes).");
        return NULL;
    }

    audio->nchannels = header.nchannels;
    audio->sample_rate = header.sample_rate;

    /* Now we're done reading the header, but there might be other non-data chunks */
    char chunk_id[4];
    u_u4b_t chunk_size;
    do {
        if (fread(chunk_id, sizeof(chunk_id), 1, fp) != 1
         || fread(&chunk_size, sizeof(chunk_size), 1, fp) != 1) {
            fclose(fp);
            free(audio);
            u_error_set(U_ERROR_FORMAT, "Invalid .wav file.");
            return NULL;
        }
        if (!strncmp(chunk_id, "data", 4))
            break; /* We found the data chunk! */
        fseek(fp, chunk_size, SEEK_CUR);
        if (feof(fp)) {
            /* Reached end of file */
            u_error_set(U_ERROR_FORMAT, "Malformed wave file (missing data chunk).");
            return NULL;
        }
    } while (1);

    /* Read the actual data */
    u_u32_t data_length = chunk_size;
    u_u1b_t* data = malloc(data_length);
    if (!data) {
        fclose(fp);
        free(audio);
        u_error_nomem();
        return NULL;
    }
    if (fread(data, 1, data_length, fp) != data_length) {
        fclose(fp);
        free(audio);
        u_error_set(U_ERROR_FORMAT, "Invalid .wav file");
        return NULL;
    }

    u_u32_t nsamples = data_length / audio->nchannels / (header.bits_per_sample / CHAR_BIT);
    u_u32_t nchannels = audio->nchannels;
    u_u32_t nvalues = nsamples * nchannels;
    u_u16_t* samples = malloc(nvalues * sizeof(*samples));
    if (!samples) {
        fclose(fp);
        free(audio);
        free(data);
        u_error_nomem();
        return NULL;
    }
    u_u16_t* samples_out = samples;
    u_u16_t* samples_end = samples_out + nvalues;
    switch (header.bits_per_sample) {
    case 8: {
        u_u1b_t* samples_in = (u_u1b_t*)data;
        while (samples_out != samples_end) {
            *samples_out++ = (*samples_in++) << 8;
        }
        break;
    }
    case 16: {
        u_i2b_t* samples_in = (u_i2b_t*)data;
        while (samples_out != samples_end) {
            *samples_out++ = (*samples_in++) + 32767;
        }
        break;
    }
    case 32: {
        if (sizeof(float) != 4) {
            u_error_set(U_ERROR_SYSTEM, "You can only use 32-bit wav files if your sizeof(float) is 4.");
            fclose(fp);
            free(audio);
            free(data);
            return NULL;
        }
        float* samples_in = (float*)data;
        while (samples_out != samples_end) {
            *samples_out++ = (*samples_in++) * 65535 - 32768;
        }
        break;
    }
    }
    audio->samples = samples;
    audio->nsamples = nsamples;
    fclose(fp);
    free(data);
    return audio;
}

u_audio_t* u_audio_create(const u_u16_t* samples, u_u32_t nsamples, u_u16_t nchannels, u_u32_t sample_rate) {
    u_audio_t* audio = malloc(sizeof(*audio));
    /* Creates a copy of samples */
    u_u16_t* samples_copy = malloc(nsamples * nchannels * sizeof(*samples));
    if (!audio || !samples_copy) {
        free(audio);
        u_error_nomem();
        return NULL;
    }
    memcpy(samples_copy, samples, nsamples * nchannels * sizeof(*samples));
    audio->samples = samples_copy;
    audio->sample_rate = sample_rate;
    audio->nsamples = nsamples;
    audio->nchannels = nchannels;
    return audio;
}

u_u32_t u_audio_sample_rate_get(const u_audio_t* audio) {
    return audio->sample_rate;
}

u_u16_t u_audio_number_of_channels_get(const u_audio_t* audio) {
    return audio->nchannels;
}

u_u32_t u_audio_number_of_samples_get(const u_audio_t* audio) {
    return audio->nsamples;
}

u_u16_t* u_audio_samples_get(u_audio_t* audio, u_u32_t* nsamples) {
    if (nsamples)
        *nsamples = audio->nsamples;
    return audio->samples;
}

int u_audio_write(const char* filename, const u_audio_t* audio) {
    FILE* fp = fopen(filename, "w");
    wav_header_t header;
    strcpy(header.chunk_id, "RIFF");
    u_u32_t data_size = audio->nchannels * audio->nsamples * sizeof(*audio->samples);
    header.chunk_size = 36 + data_size;
    strcpy(header.format, "WAVE");
    strcpy(header.subchunk1_id, "fmt ");
    header.subchunk1_size = 16;
    header.audio_format = 1;
    header.nchannels = audio->nchannels;
    header.sample_rate = audio->sample_rate;
    header.byte_rate = audio->sample_rate * audio->nchannels * sizeof(*audio->samples);
    header.block_align = header.nchannels * sizeof(*audio->samples);
    header.bits_per_sample = CHAR_BIT * sizeof(*audio->samples);
    fwrite(&header, sizeof(header), 1, fp);
    fwrite("data", 1, 4, fp);
    u_u32_t nvalues = audio->nchannels * audio->nsamples;
    u_u32_t chunk_size = nvalues * sizeof(*audio->samples);
    fwrite(&chunk_size, sizeof(chunk_size), 1, fp);

    /* Convert to signed */
    const u_u16_t* samples_in = audio->samples;
    u_i2b_t* samples =  malloc(nvalues * sizeof(*samples));
    if (!samples) {
        fclose(fp);
        return u_error_nomem();
    }

    u_i2b_t* samples_out = samples;
    u_i2b_t* samples_end = samples_out + nvalues;
    while (samples_out != samples_end)
        *samples_out++ = (*samples_in++) - 32768;

    fwrite(samples, sizeof(*samples), nvalues, fp);

    free(samples);
    fclose(fp);
    return U_ERROR_SUCCESS;
}

void u_audio_free(u_audio_t* audio) {
    if (!audio) return;
    free(audio->samples);
    free(audio);
}
