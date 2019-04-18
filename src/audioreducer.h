/*
    Copyright (C) 2019 Leo Tenenbaum
    This file is part of ColorReducer.

    ColorReducer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ColorReducer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ColorReducer.  If not, see <https://www.gnu.org/licenses/>.
*/
/** \file audioreducer.h
\brief Reduce the number of values in audio files.

Reduce the number of possible values a sample can have in an audio file.
Check utils/filetypes/audio.h for supported file types.

This is like converting between 16-bit and 8-bit audio, but it tries to find
the values which match the audio most closely, rather than just sampling them
evenly (but it won't make too much of a difference...). With this library,
you can also find out what 4-bit, 2-bit or even 1-bit audio would sound like!
*/
#ifndef COLORREDUCER_AUDIOREDUCER_H
#define COLORREDUCER_AUDIOREDUCER_H

#include <stddef.h>

#include "utils/filetypes/audio.h"
/**
Reduces the number of values in a piece of audio to \p nvals.
\param audio The input audio
\param nvals The number of values to reduce the audio to. The file will just be copied if this is more than 65535, because it uses 16-bit audio anyways.
\param take If this value is positive, k-means will randomly select this many samples, rather than using all of them. It is recommended that you use this, because it can make it run much faster.
\param epsilon k-means will stop when the values have moved by less than epsilon on average.
\param iterations If this value is positive, k-means will stop when it has reached that many iterations (regardless of the change between iterations).
*/
int cr_reduce_audio(u_audio_t* audio, size_t nvals, size_t take, float epsilon, size_t iterations);

/**
Applies value reduction to an audio file.
\param filename_in The filename of the input audio
\param filename_out The filename of the output audio
\param take The percentage of audio samples to actually use for k-means.
 */
int cr_reduce_audio_file(const char* filename_in, const char* filename_out, size_t nvals, float take, float epsilon, size_t iterations);

#endif /* COLORREDUCER_AUDIOREDUCER_H */
