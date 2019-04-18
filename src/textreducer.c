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
#include "textreducer.h"

#include <stdio.h>
#include <stdlib.h>

#include "utils/misc/error.h"
#include "kmeans.h"

int cr_reduce_text_file(const char* filename_in, const char* filename_out, size_t nvals, float take, float epsilon, size_t iterations) {
    FILE* in = fopen(filename_in, "r");
    if (!in)
        return u_error_fopen(filename_in, "reading");
    unsigned long data_size, ndata;
    if (fscanf(in, "%lu%lu", &ndata, &data_size) != 2)
        return u_error_set(U_ERROR_FORMAT, "Invalid text format.");
    unsigned long nvalues = data_size * ndata;
    float* data = malloc(nvalues * sizeof(*data));
    if (!data) {
        fclose(in);
        return u_error_nomem();
    }
    unsigned long i, j;
    for (i = 0; i < nvalues; i++) {
        if (fscanf(in, "%f", &data[i]) != 1) {
            fclose(in);
            free(data);
            return u_error_set(U_ERROR_FORMAT, "Invalid text format.");
        }
    }
    fclose(in);

    cr_kmeans_run(data, ndata, data_size, nvals, take * ndata, epsilon, iterations);

    FILE* out = fopen(filename_out, "w");
    if (!out) {
        free(data);
        return u_error_fopen(filename_out, "writing");
    }
    fprintf(out, "%lu %lu\n", ndata, data_size);

    for (i = 0; i < ndata; i++) {
        for (j = 0; j < data_size; j++) {
            fprintf(out, "%f ", data[i*data_size+j]);
        }
        fprintf(out,"\n");
    }
    free(data);
    fclose(out);
    return U_ERROR_SUCCESS;
}
