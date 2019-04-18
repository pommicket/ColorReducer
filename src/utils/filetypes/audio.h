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
/** \file audio.h
\brief Read/write audio files

Currently only .wav is supported.
*/
#ifndef CUTILS_FILETYPES_AUDIO_H
#define CUTILS_FILETYPES_AUDIO_H

#include <stddef.h>

#include "../misc/types.h"

typedef struct u_audio u_audio_t;

/** Reads an audio file.
\param filename The file's name.
\returns Either a pointer to an audio object or `NULL` if an error occurs (and sets \ref u_error_message).
 */
u_audio_t* u_audio_read(const char* filename);
/** Creates an audio file from samples.
\param samples An array of unsigned 16-bit integers corresponding to each sample.
               For stereo audio, `samples[0]` should be the first sample of the
               left channel; `samples[1]` should be the first sample of the right
               channel, etc.
\param nsamples The total number of samples. The length of the array should be
                `nsamples * nchannels`
\param nchannels The number of channels of audio (1 = Mono, 2 = Stereo)
\param sample_rate The number of samples per second.
*/
u_audio_t* u_audio_create(const u_u16_t* samples, u_u32_t nsamples, u_u16_t nchannels, u_u32_t sample_rate);
/** Gets the sample rate of a piece of audio.
\returns The sample rate of the audio. */
u_u32_t    u_audio_sample_rate_get(const u_audio_t* audio);
/** Gets the number of channels in a piece of audio.
\returns The number of channels in the audio (1 = Mono, 2 = Stereo). */
u_u16_t    u_audio_number_of_channels_get(const u_audio_t* audio);
/** Gets the number of audio samples in a piece of audio.
\returns The number of samples (counting one multi-channel sample as one sample).
 */
u_u32_t    u_audio_number_of_samples_get(const u_audio_t* audio);
/** Returns an array containing the samples of the audio file.
    See \ref u_audio_create for an explanation of the format of this.
    \param nsamples If this is not `NULL`, sets it to
    the number of samples in the piece of audio.
    This is equal to `duration * sample_rate`.
    The length of the array is this number multiplied by the number of channels.
    */
u_u16_t*   u_audio_samples_get(u_audio_t* audio, u_u32_t* nsamples);
/** Writes a piece of audio to a file
\param filename The name of the audio file to write to
\param audio The piece of audio.
 */
int        u_audio_write(const char* filename, const u_audio_t* audio);
/** Frees a piece of audio. */
void       u_audio_free(u_audio_t* audio);
#endif /* CUTILS_FILETYPES_AUDIO_H */
