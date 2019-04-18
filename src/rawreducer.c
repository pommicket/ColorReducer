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
#include "rawreducer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/misc/error.h"
#include "utils/misc/types_exact.h"
#include "kmeans.h"

int cr_reduce_raw_file(const char* filename_in, const char* filename_out, size_t nvals, float take, float epsilon, size_t iterations) {
    if (sizeof(float) != 4)
        return u_error_set(U_ERROR_SYSTEM, "sizeof(float) must be 4 to read raw files.");

    FILE* in = fopen(filename_in, "r");
    if (!in)
        return u_error_fopen(filename_in, "reading");

    u_u4b_t ndata, data_size;
    if (fread(&ndata, sizeof(ndata), 1, in) != 1)
        return u_error_set(U_ERROR_FORMAT, "Invalid raw file format.");
    if (fread(&data_size, sizeof(data_size), 1, in) != 1)
        return u_error_set(U_ERROR_FORMAT, "Invalid raw file format.");

    float* data = malloc(data_size * ndata * sizeof(*data));
    if (!data) {
        fclose(in);
        return u_error_nomem();
    }

    if (fread(data, sizeof(*data), data_size * ndata, in) != data_size * ndata)
        return u_error_set(U_ERROR_FORMAT, "Invalid raw file format.");

    fclose(in);

    cr_kmeans_run(data, ndata, data_size, nvals, take * ndata, epsilon, iterations);

    FILE* out = fopen(filename_out, "w");
    if (!out)
        return u_error_fopen(filename_out, "writing");

    if (fwrite(&ndata, sizeof(ndata), 1, out) != 1)
        return u_error_set(U_ERROR_ACCESS, "File write failed.");

    if (fwrite(&data_size, sizeof(data_size), 1, out) != 1)
        return u_error_set(U_ERROR_ACCESS, "File write failed.");

    if (fwrite(data, sizeof(*data), data_size * ndata, out) != data_size * ndata)
        return u_error_set(U_ERROR_ACCESS, "File write failed.");

    fclose(out);
    free(data);
    return U_ERROR_SUCCESS;
}
