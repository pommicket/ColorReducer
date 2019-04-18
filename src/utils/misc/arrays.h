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
/** \file arrays.h
\brief Various functions which manipulate arrays
*/

#ifndef CUTILS_MISC_ARRAYS_H
#define CUTILS_MISC_ARRAYS_H

#include <stddef.h>
/** Swaps elements at indices \p i and \p j in the array (where \p size is the
    size of each element). This function should not be used for ordinary arrays
    (only for `void*` arrays), because it will be faster to just swap them
    yourself. It allocates \p size bytes of memory, so it can return an error
    code.
    \returns An error code (and sets \ref u_error_message). */
int u_arrays_swap(void* array, size_t size, size_t i, size_t j);
#endif /* CUTILS_MISC_ARRAYS_H */
