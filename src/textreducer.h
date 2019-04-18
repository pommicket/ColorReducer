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

/** \file textreducer.h
\brief Reduces non-raw text files.

The files should be in this format:
```
5 (the number of pieces of data) 2 (numbers per piece of data)
0.3 0.2 (1st piece of data)
1.1 2.2 (2nd)
7.4 3.6 (...)
2.2 8.9
-1.3 -3.8
```
If you run this function on it, you will get something like
```
5 2
0.033333 -0.466667
0.033333 -0.466667
4.800000 6.250000
4.800000 6.250000
0.033333 -0.466667
```
Here, k-means has found the two means (0.033333, -0.466667) and (4.8, 6.25).


If you are dealing with large data sets,
you should use raw files, since they will probably
take up less space (although it is possible
that text files could take up less space if
you don't want the full 4 bytes of precision).
*/
#ifndef COLORREDUCER_TEXTREDUCER_H
#define COLORREDUCER_TEXTREDUCER_H

#include <stddef.h>

/**
Reduces the number of values in a text file, as described in this file's description.
\param filename_in The name of the input file
\param filename_out The name of the output file
\param nvals The number of values to reduce it to
\param take If this number is between 0 and 1, it refers to the percentage of pieces of data in the file to use in k-means. Otherwise, it will use the whole file.
\param epsilon See \ref cr_kmeans_run
\param iterations See \ref cr_kmeans_run
 */
int cr_reduce_text_file(const char* filename_in, const char* filename_out, size_t nvals, float take, float epsilon, size_t iterations);

#endif /* COLORREDUCER_TEXTREDUCER_H */
