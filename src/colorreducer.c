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
#include "colorreducer.h"

#include "kmeans.h"
#include "utils/filetypes/image.h"
#include "utils/misc/error.h"

#include <stdlib.h>

int cr_reduce_image(u_image_t* image, size_t ncolors, size_t take, float epsilon, size_t iterations) {
    int width = u_image_width_get(image),
        height = u_image_height_get(image);

    /* The color of every pixel in the image */
    float* colors = malloc(3 * width * height * sizeof(*colors));
    if (!colors)
        return u_error_nomem();

    u_color_t** pixels = u_image_pixels_get(image);
    int x, y;
    size_t i = 0;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            u_color_t pixel = pixels[y][x];
            colors[i+0] = pixel.r / 256.0f;
            colors[i+1] = pixel.g / 256.0f;
            colors[i+2] = pixel.b / 256.0f;
            i += 3;
        }
    }

    int err = cr_kmeans_run(colors, width * height, 3, ncolors, take, epsilon, iterations);
    if (err) {
        free(colors);
        return err;
    }
    i = 0;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            u_color_t pixel = u_color_from_rgb(
                256 * colors[i+0],
                256 * colors[i+1],
                256 * colors[i+2]
            );
            pixels[y][x] = pixel;
            i += 3;
        }
    }
    free(colors);
    return U_ERROR_SUCCESS;
}

int cr_reduce_image_file(const char* filename_in, const char* filename_out, size_t ncolors, float take, float epsilon, size_t iterations) {
    u_image_t* image = u_image_read(filename_in);
    if (!image)
        return u_error_code;

    if (take < 0 || take >= 1) take = 0;
    size_t ntake = u_image_width_get(image)  * u_image_height_get(image) * take;
    int err = cr_reduce_image(image, ncolors, ntake, epsilon, iterations);
    if (err) return err;
    err = u_image_write(filename_out, image);
    if (err) return err;
    u_image_free(image);
    return U_ERROR_SUCCESS;
}
