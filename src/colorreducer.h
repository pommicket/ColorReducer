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
/** \file colorreducer.h
\brief Reduces the number of colors in an image.

Not the most efficient implementation ever.

Runs in (probably) `O(width*height*log(colors)+colors)` time
and `O(width*height+colors)` memory
*/

#ifndef COLORREDUCER_COLORREDUCER_H
#define COLORREDUCER_COLORREDUCER_H

#include <stddef.h>

#include "utils/filetypes/image.h"

/**
Reduces the number of colors in an image to \p ncolors.
\param image The image to apply the reduction to
\param ncolors The number of colors to reduce it to.
\param take If this value is postive, rather than running on the whole image, k-means will only use that many pixels. This can make it run much faster.
\param epsilon The color reducing will stop when the colors have moved by less than epsilon on average.
\param iterations If this value is positive, the color reducing will stop when it has reached that many iterations (regardless of the change between iterations).
*/
int cr_reduce_image(u_image_t* image, size_t ncolors, size_t take, float epsilon, size_t iterations);

/**
Applies color reduction to an image file.
\param filename_in The filename of the input image
\param filename_out The filename of the output image
\param take If this value is positive and less than 1, k-means will only run on this percentage (randomly selected) of the colors, rather than using the whole image. This can make it run much faster.
 */
int cr_reduce_image_file(const char* filename_in, const char* filename_out, size_t ncolors, float take, float epsilon, size_t iterations);

#endif /* COLORREDUCER_COLORREDUCER_H */
