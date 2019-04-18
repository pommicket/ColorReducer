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
#include "rand.h"

#include <stdlib.h>

#include "../misc/arrays.h"
#include "../misc/error.h"

u_bool_t u_rand_bool(void) {
    return rand() & 1;
}

#if RAND_MAX >= INT_MAX
int random_int(void) {
    return (int)rand();
}
#else
int random_int(void) {
    int i;
    int x = 0;
    for (i = 0; i <= (sizeof(int)*CHAR_BIT)/15; i++) {
        x = (x << 15) + (rand() & 32767);
    }
    return x;
}
#endif

#if RAND_MAX >= ULONG_MAX
unsigned long random_ulong(void) {
    return (unsigned long)rand();
}
#elif RAND_MAX >= 2147483647
unsigned long random_ulong(void) {
    int i;
    int x = 0;
    for (i = 0; i <= (sizeof(unsigned long)*CHAR_BIT)/31; i++) {
        x = (x << 31) + (rand() & 2147483647);
    }
    return x;
}
#else
unsigned long random_ulong(void) {
    int i;
    int x = 0;
    for (i = 0; i <= (sizeof(unsigned long)*CHAR_BIT)/15; i++) {
        x = (x << 15) + (rand() & 32767);
    }
    return x;
}
#endif


int u_rand_int(int from, int to) {
    int range = to - from;
    int accept_up_to = INT_MAX - (INT_MAX % range);
    int r = random_int();
    while (r >= accept_up_to) r = random_int();
    return from + (r % range);
}

size_t u_rand_size(size_t from, size_t to) {
    unsigned long range = to - from;
    unsigned long accept_up_to = ULONG_MAX - (ULONG_MAX % range);
    unsigned long r = random_ulong();
    while (r >= accept_up_to) r = random_ulong();
    return from + (r % range);
}

double u_rand_double(void) {
    return (double)rand()/RAND_MAX;
}

int u_rand_shuffle(void* array, size_t nmemb, size_t size) {
    size_t i;
    for (i = 1; i < nmemb; i++) {
        size_t j = u_rand_size(0, i);
        int err = u_arrays_swap(array, size, i, j);
        if (err) return err;
    }
    return U_ERROR_SUCCESS;
}
