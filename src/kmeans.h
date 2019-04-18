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
/** \file kmeans.h
\brief k-means classification

k-means classification classifies a set of data into k groups.

Takes time (approximately) `O(ndata*data_size*log(k)+k*data_size)`,
and (additional) memory `O(k*data_size)`.

*/
#ifndef COLORREDUCER_KMEANS_H
#define COLORREDUCER_KMEANS_H

#include <stddef.h>

/**
 Run k-means on \p data **and sets each element of data to its respective mean**.
\p data is expected to be a `float[ndata*data_size]` (where data[0..data_size] refers to the first piece of data, etc.).
\param data The data to run kmeans on
\param ndata The number of pieces of data
\param data_size The size of a piece of data (how many `float`s are there in \p data for each piece of data).
\param k The number of means
\param take If this value is positive, k-means will only run on that many pieces of data, randomly selected. If it is 0, it will run on all the data.
\param epsilon k-means will stop once the difference between the means of successive iterations is less than it.
\param iterations If this value is positive, k-means will stop after this many iterations (regardless of the change between iterations).
\returns An error code. */
int cr_kmeans_run(float* data, size_t ndata, size_t data_size, size_t k, size_t take, float epsilon, size_t iterations);

#endif /* COLORREDUCER_KMEANS_H */
