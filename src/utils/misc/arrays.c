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
#include "arrays.h"

#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "../math/rand.h"

int u_arrays_swap(void* array, size_t size, size_t i, size_t j) {
    void* tmp = malloc(size);
    if (!tmp)
        return u_error_nomem();
    void* a = array + i * size;
    void* b = array + j * size;
    memcpy(tmp, a, size);
    memcpy(a, b, size);
    memcpy(b, tmp, size);
    free(tmp);
    return U_ERROR_SUCCESS;
}
