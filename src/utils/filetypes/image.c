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
#include "image.h"

#include <stdlib.h>
#include <string.h>
#include <png.h>

#include "../misc/error.h"

struct u_image {
    u_color_t** pixels;
    png_infop info;
    png_structp png;
    int width;
    int height;
    png_byte color_type;
    u_bool_t read; /* was png created with png_create_read_struct or png_create_write_struct? = U_FALSE for write, U_TRUE for read */
};

#if defined(_WIN32) || defined(__MINGW32__)
#define WINDOWS /* Using an older version of libpng on windows, so it doesn't have png_jmpbuf. */
#endif

void user_error_fn(png_structp png_ptr, png_const_charp error_msg) {
    printf("%s\n",error_msg);
}

u_image_t* u_image_read(const char* filename) {
    u_image_t* image = malloc(sizeof(*image));
    if (!image) {
        u_error_nomem();
        return NULL;
    }
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        u_error_set(U_ERROR_ACCESS, "Image file not found.");
        return NULL;
    }
    unsigned char header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) { /* Check header */
        free(image);
        fclose(fp);
        u_error_set(U_ERROR_FORMAT, "Failed to open image: Must be a PNG file.");
        return NULL;
    }

    image->png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, user_error_fn, NULL);
    image->read = U_TRUE;
    if (!image->png) {
        free(image);
        fclose(fp);
        u_error_set(U_ERROR_OTHER, "Failed to create PNG struct.");
        return NULL;
    }

    image->info = png_create_info_struct(image->png);
    if (!image->info) {
        free(image);
        fclose(fp);
        u_error_set(U_ERROR_OTHER, "Failed to create PNG info struct.");
        return NULL;
    }

    #ifndef WINDOWS
    if (setjmp(png_jmpbuf(image->png))) {
        free(image);
        fclose(fp);
        u_error_set(U_ERROR_OTHER, "Failed to read image.");
        return NULL;
    }
    #endif

    png_init_io(image->png, fp);
    png_set_sig_bytes(image->png, 8);
    png_read_info(image->png, image->info);

    image->width = png_get_image_width(image->png, image->info);
    image->height = png_get_image_height(image->png, image->info);
    image->color_type = png_get_color_type(image->png, image->info);
    png_byte bit_depth = png_get_bit_depth(image->png, image->info);
    if (bit_depth != 8) {
        free(image);
        fclose(fp);
        u_error_set(U_ERROR_FORMAT, "Invalid image bit depth. Must be an 8-bit RGB/RGBA PNG.");
        return NULL;
    }

    #ifndef WINDOWS
    if (setjmp(png_jmpbuf(image->png))) {
        free(image);
        fclose(fp);
        u_error_set(U_ERROR_OTHER, "Failed to read image.");
        return NULL;
    }
    #endif

    png_bytepp rows;
    rows = malloc(image->height * sizeof(*rows));
    image->pixels = malloc(image->height * sizeof(*image->pixels));
    if (!rows || !image->pixels) {
        free(image);
        fclose(fp);
        u_error_nomem();
        return NULL;
    }
    int i;
    for (i = 0; i < image->height; i++) {
        rows[i] = malloc(png_get_rowbytes(image->png, image->info));
        image->pixels[i] = malloc(image->width * sizeof(*image->pixels[i]));
        if (!rows[i] || !image->pixels[i]) {
            int j;
            for (j = 0; j < i; j++) {
                free(rows[j]);
                free(image->pixels[j]);
            }
            free(image->pixels);
            free(rows);
            free(image);
            fclose(fp);
            u_error_nomem();
            return NULL;
        }
    }
    png_read_image(image->png, rows);
    int has_alpha = 0;
    switch (png_get_color_type(image->png, image->info)) {
    case PNG_COLOR_TYPE_RGB:
        has_alpha = 0;
        break;
    case PNG_COLOR_TYPE_RGBA:
        has_alpha = 1;
        break;
    default:
        for (i = 0; i < image->height; i++) {
            free(rows[i]);
            free(image->pixels[i]);
        }
        free(image->pixels);
        free(rows);
        free(image);
        fclose(fp);
        u_error_set(U_ERROR_FORMAT, "Invalid PNG color type (must be RGB or RGBA).");
        return NULL;
    }
    int x, y;
    for (y = 0; y < image->height; y++) {
        png_bytep row = rows[y];
        for (x = 0; x < image->width; x++) {
            if (has_alpha) {
                image->pixels[y][x]
                    = u_color_from_rgba(row[4*x], row[4*x+1], row[4*x+2],
                                        row[4*x+3]);
            } else {
                image->pixels[y][x]
                    = u_color_from_rgb(row[3*x], row[3*x+1], row[3*x+2]);
            }
        }
    }

    for (i = 0; i < image->height; i++)
        free(rows[i]);
    free(rows);
    fclose(fp);
    return image;
}

u_image_t* u_image_new(int width, int height, u_color_t color) {
    u_image_t* image = malloc(sizeof(*image));
    if (!image) {
        u_error_nomem();
        return NULL;
    }
    image->color_type = PNG_COLOR_TYPE_RGBA;
    image->width = width;
    image->height = height;
    image->png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!image->png) {
        free(image);
        u_error_set(U_ERROR_OTHER, "Failed to create PNG struct.");
        return NULL;
    }

    image->info = png_create_info_struct(image->png);
    if (!image->info) {
        free(image);
        u_error_set(U_ERROR_OTHER, "Failed to create PNG info struct.");
        return NULL;
    }
    #ifndef WINDOWS
    if (setjmp(png_jmpbuf(image->png))) {
        free(image);
        u_error_set(U_ERROR_OTHER, "Failed to create PNG info struct.");
        return NULL;
    }
    #endif
    image->read = U_FALSE;
    image->pixels = malloc(height * sizeof(*image->pixels));
    if (!image->pixels) {
        png_destroy_write_struct(&image->png, &image->info);
        free(image);
    }
    int i;
    for (i = 0; i < height; i++) {
        image->pixels[i] = malloc(width * sizeof(*image->pixels[i]));
        int j;
        if (!image->pixels[i]) {
            for (j = 0; j < i; j++)
                free(image->pixels[j]);
            free(image->pixels);
            free(image);
            u_error_nomem();
            return NULL;
        }
        for (j = 0; j < width; j++) {
            image->pixels[i][j] = color;
        }
    }
    return image;
}

int u_image_write(const char* filename, u_image_t* image) {
    if (image->read) {
        png_destroy_read_struct(&image->png, &image->info, NULL);
        image->png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!image->png) {
            return u_error_set(U_ERROR_OTHER, "Failed to create PNG struct.");
        }

        image->info = png_create_info_struct(image->png);
        if (!image->info) {
            return u_error_set(U_ERROR_OTHER, "Failed to create PNG info struct.");
        }
        #ifndef WINDOWS
        if (setjmp(png_jmpbuf(image->png))) {
            return u_error_set(U_ERROR_OTHER, "Failed to create PNG info struct.");
        }
        #endif
        image->read = U_FALSE;

    }


    FILE* fp = fopen(filename, "wb");
    if (!fp)
        return U_ERROR_ACCESS;


    png_init_io(image->png, fp);
    #ifndef WINDOWS
    if (setjmp(png_jmpbuf(image->png))) {
        fclose(fp);
        return u_error_set(U_ERROR_OTHER, "Failed to write image to file.");
    }
    #endif
    png_set_IHDR(image->png, image->info, image->width, image->height,
                 8 /* Bit depth */, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(image->png, image->info);


    png_bytepp rows = malloc(image->height * sizeof(*rows));
    if (!rows) {
        fclose(fp);
        u_error_nomem();
        return U_ERROR_NOMEM;
    }
    int i, j;
    for (i = 0; i < image->height; i++) {
        rows[i] = malloc(4 * image->width * sizeof(*rows[i]));
        if (!rows[i]) {
            for (j = 0; j < i; j++)
                free(rows[j]);
            free(rows);
            fclose(fp);
            u_error_nomem();
            return U_ERROR_NOMEM;
        }
        for (j = 0; j < image->width; j++) {
            rows[i][4*j+0] = image->pixels[i][j].r;
            rows[i][4*j+1] = image->pixels[i][j].g;
            rows[i][4*j+2] = image->pixels[i][j].b;
            rows[i][4*j+3] = image->pixels[i][j].a;
        }
    }

    #ifndef WINDOWS
    if (setjmp(png_jmpbuf(image->png))) {
        for (i = 0; i < image->height; i++)
            free(rows[i]);
        free(rows);
        fclose(fp);
        return u_error_set(U_ERROR_OTHER, "Failed to write image to file.");
    }
    #endif

    png_write_image(image->png, rows);

    #ifndef WINDOWS
    if (setjmp(png_jmpbuf(image->png))) {
        for (i = 0; i < image->height; i++)
            free(rows[i]);
        free(rows);
        fclose(fp);
        return u_error_set(U_ERROR_OTHER, "Failed to write image to file.");
    }
    #endif
    png_write_end(image->png, NULL);
    for (i = 0; i < image->height; i++)
        free(rows[i]);
    free(rows);
    fclose(fp);
    return U_ERROR_SUCCESS;
}

int u_image_width_get(const u_image_t* image) {
    return image->width;
}

int u_image_height_get(const u_image_t* image) {
    return image->height;
}

u_color_t u_image_pixel_get(const u_image_t* image, int x, int y) {
    return image->pixels[y][x];
}

void u_image_pixel_set(u_image_t* image, int x, int y, u_color_t color) {
    image->pixels[y][x] = color;
}

u_color_t** u_image_pixels_get(u_image_t* image) {
    return image->pixels;
}

void u_image_free(u_image_t* image) {
    if (image->read) {
        png_destroy_read_struct(&image->png, &image->info, NULL);
    } else {
        png_destroy_write_struct(&image->png, &image->info);
    }
    int i;
    for (i = 0; i < image->height; i++) {
        free(image->pixels[i]);
    }
    free(image->pixels);
    free(image);
}
