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
#include "audioreducer.h"

#include "kmeans.h"
#include "utils/filetypes/audio.h"
#include "utils/misc/error.h"

#include <stdlib.h>


int cr_reduce_audio(u_audio_t* audio, size_t nvals, size_t take, float epsilon, size_t iterations) {
    /* Convert the samples to floats */
    u_u32_t nsamples;
    u_u16_t* samples = u_audio_samples_get(audio, &nsamples);
    u_u32_t naudio_values = nsamples * u_audio_number_of_channels_get(audio);
    float* fsamples = malloc(naudio_values * sizeof(*fsamples));
    if (!fsamples) {
        return u_error_nomem();
    }

    u_u32_t i;
    for (i = 0; i < naudio_values; i++) {
        fsamples[i] = samples[i] / 65536.0f; /* Technically we don't need to divide, but why not (: */
    }

    int err = cr_kmeans_run(fsamples, naudio_values, 1, nvals, take, epsilon, iterations);
    if (err) {
        free(fsamples);
        return err;
    }

    for (i = 0; i < naudio_values; i++) {
        samples[i] = fsamples[i] * 65536;
    }
    free(fsamples);
    return U_ERROR_SUCCESS;
}

int cr_reduce_audio_file(const char* filename_in, const char* filename_out, size_t nvals, float take, float epsilon, size_t iterations) {
    u_audio_t* audio = u_audio_read(filename_in);
    if (!audio) return u_error_code;

    if (nvals >= 65536) {
        /* More values than 16-bit numbers */
        int err = u_audio_write(filename_out, audio);
        u_audio_free(audio);
        if (err) return err;
        return U_ERROR_SUCCESS;
    }

    if (take < 0 || take >= 1) take = 0;
    size_t ntake = u_audio_number_of_samples_get(audio) * take;

    int err = cr_reduce_audio(audio, nvals, ntake, epsilon, iterations);
    if (err) {
        u_audio_free(audio);
        return err;
    }

    err = u_audio_write(filename_out, audio);
    if (err) {
        u_audio_free(audio);
        return err;
    }

    u_audio_free(audio);
    return U_ERROR_SUCCESS;
}
