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
/** \file rawreducer.h
\brief Reduces raw (binary) files.

Reduces the number of values in a file.

Here is the format for the file:

<number of pieces of data (ndata), 4 bytes unsigned integer><size of each piece of data (data_size), 4 bytes unsigned integer>
<data 4*ndata*data_size bytes>

Data should be stored as a series of `float`s (4 bytes) in binary, with the first referring
to the first element of the first piece of data, the second referring to the second
element of the first piece of data, etc.. Here is an example file (on a little-endian system):

```
03 00 00 00 02 00 00 00
ndata = 3   data_size=2
00 00 a0 40 00 00 40 40
5 as a float    3.0
00 00 c0 40 cd cc 0c 40
6.0         2.2
00 00 10 41 00 00 80 bf
9.0         -1.0
```

Here we have a file which represents the values
(5, 3), (6, 2.2), and (9, -1). If we run this program
on it, we will get something like:

```
03 00 00 00 02 00 00 00
ndata = 3   data_size=2
00 00 b0 40 66 66 26 40
5.5         2.6
00 00 b0 40 66 66 26 40
5.5         2.6
00 00 10 41 00 00 80 bf
9.0         -1.0
```

In this example, k-means has selected the two means
(5.5, 2.6) and (9.0, -1.0).
*/

#ifndef COLORREDUCER_RAWREDUCER_H
#define COLORREDUCER_RAWREDUCER_H

#include <stddef.h>

/**
Reduces the number of values in a raw file, as described in this file's description.
Uses roughly 8 bytes per float in the file of memory.
\param filename_in The name of the input file
\param filename_out The name of the output file
\param nvals The number of values to reduce it to
\param epsilon See \ref cr_kmeans_run
\param iterations See \ref cr_kmeans_run
\param take If this value is positive, it refers to the percentage of raw data to run k-means on. Otherwise, it will run on all the data.
 */
int cr_reduce_raw_file(const char* filename_in, const char* filename_out, size_t nvals, float take, float epsilon, size_t iterations);

#endif /* COLORREDUCER_RAWREDUCER_H */
