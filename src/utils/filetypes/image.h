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
/** \file image.h
\brief Image I/O

Reading and writing images. Currently only supports 8-bit RGB/RGBA PNG.
*/
#ifndef CUTILS_FILETYPES_IMAGE_H
#define CUTILS_FILETYPES_IMAGE_H

#include "../misc/color.h"

typedef struct u_image u_image_t; /**< The struct for an image. */

/** Opens an image file for reading.
\returns An image, or NULL on failure and sets \ref u_error_message accordingly. */
u_image_t*  u_image_read(const char* filename);
/** Creates a new RGBA image with the given width and height and with all pixels
    set to the given color. Returns NULL on failure and sets \ref u_error_message
    accordingly. */
u_image_t*  u_image_new(int width, int height, u_color_t color);
/** Outputs the image to a file. \returns An error code. */
int         u_image_write(const char* filename, u_image_t* image);
/** \returns The width in pixels of \p image. */
int         u_image_width_get(const u_image_t* image);
/** \returns The height in pixels of \p image. */
int         u_image_height_get(const u_image_t* image);
/** \returns The color of the pixel at (\p x, \p y) */
u_color_t   u_image_pixel_get(const u_image_t* image, int x, int y);
/** Sets the color of the pixel at (\p x,\p y) to \p color */
void        u_image_pixel_set(u_image_t* image, int x, int y, u_color_t color);
/** \returns An array containing the pixels in the image.
    To access a pixel, simply use `pixels[y][x]`. */
u_color_t** u_image_pixels_get(u_image_t* image);
/** Frees the memory used by an image. */
void        u_image_free(u_image_t* image);

#endif /* CUTILS_FILETYPES_IMAGE_H */
