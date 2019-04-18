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
/** \file rand.h
\brief Random number generation

These functions generate random numbers. They just use `rand`, so they probably
won't be cryptographically secure or anything.

They should be more uniformly random than `rand() % N`.

*/

#ifndef CUTILS_MATH_RAND_H
#define CUTILS_MATH_RAND_H

#include "../misc/types.h"

#include <stddef.h>

/** \returns A random boolean value (either \reF U_FALSE or \ref U_TRUE). */
u_bool_t u_rand_bool(void);
/** \returns A random `int` from \p from to \p to. */
int      u_rand_int(int from, int to);
/** \returns A random `size_t` from \p from to \p to. */
size_t   u_rand_size(size_t from, size_t to);
/** \returns A random `double` from 0 to 1. */
double   u_rand_double(void);
/** Shuffles the array of \p nmemb elements of size \p size (in bytes).
    \returns An error code (and sets \ref u_error_message). */
int      u_rand_shuffle(void* array, size_t nmemb, size_t size);

#endif /* CUTILS_MATH_RAND_H */
