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

#include "voronoi.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils/filetypes/image.h"
#include "utils/containers/kdtree.h"
#include "utils/math/rand.h"
#include "utils/misc/error.h"

typedef struct {
    float point[2];
    u_color_t color;
} point_t;

static int voronoify_image(u_image_t* image) {
    /* Frees image on failure */

    int w = u_image_width_get(image), h = u_image_height_get(image);

    u_color_t** pixels = u_image_pixels_get(image);
    int x, y, nseeds = 0;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            if (u_color_to_u32(pixels[y][x]) != 0xFFFFFFFF) {
                nseeds++;
            }
        }
    }
    point_t* seeds = malloc(nseeds * sizeof(*seeds));
    if (!seeds) {
        u_image_free(image);
        return u_error_nomem();
    }
    nseeds = 0;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            if (u_color_to_u32(pixels[y][x]) != 0xFFFFFFFF) {
                seeds[nseeds].point[0] = x;
                seeds[nseeds].point[1] = y;
                seeds[nseeds].color = pixels[y][x];
                nseeds++;
            }
        }
    }
    int err = u_rand_shuffle(seeds, nseeds, sizeof(*seeds));
    if (err) return err;

    u_kdtree_t kdtree;
    u_kdtree_construct(&kdtree, 2, sizeof(u_color_t));
    int i;
    for (i = 0; i < nseeds; i++) {
        int err = u_kdtree_insert(&kdtree, seeds[i].point, &seeds[i].color);
        if (err) {
            free(seeds);
            u_image_free(image);
            u_kdtree_destroy(&kdtree);
            return err;
        }
    }
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            float p[2] = {x, y};
            u_color_t closest_color =
                *(const u_color_t*)u_kdtree_nearest(&kdtree, p, NULL);
            pixels[y][x] = closest_color;
        }
    }

    free(seeds);
    u_kdtree_destroy(&kdtree);
    return U_ERROR_SUCCESS;
}

int cr_voronoify(const char* filename_in, const char* filename_out) {
    u_image_t* image = u_image_read(filename_in);
    if (!image)
        return U_ERROR_OTHER;

    int err = voronoify_image(image);
    if (err) {
        return err;
    }
    err = u_image_write(filename_out, image);
    if (err) {
        u_image_free(image);
        return err;
    }

    u_image_free(image);
    return U_ERROR_SUCCESS;
}

int cr_voronoi_random(int width, int height, int nseeds, const char* filename_out) {
    u_image_t* image = u_image_new(width, height, u_color_from_rgb(255, 255, 255));
    u_color_t** pixels = u_image_pixels_get(image);
    int i, x, y;
    for (i = 0; i < nseeds; i++) {
        x = rand() % width;
        y = rand() % height;
        pixels[y][x] = u_color_random_rgb();
    }
    int err = voronoify_image(image);
    if (err) return err;
    err = u_image_write(filename_out, image);
    if (err) {
        u_image_free(image);
        return err;
    }
    u_image_free(image);
    return U_ERROR_SUCCESS;
}
