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
/**
\file color.h
\brief A color type
*/
#ifndef CUTILS_MISC_COLOR_H
#define CUTILS_MISC_COLOR_H

#include "types.h"

typedef struct {
    u_byte_t r;
    u_byte_t g;
    u_byte_t b;
    u_byte_t a;
} u_color_t;

u_color_t u_color_from_u32_rgba(u_u32_t rgba); /**< Creates a color from a 32-bit value of the form 0xRRGGBBAA. */
u_color_t u_color_from_u32_rgb(u_u32_t rgb); /**< Creates a color from a 32-bit value of the form 0x00RRGGBB. Alpha is set to 255. */
u_color_t u_color_from_rgba(u_u8_t r, u_u8_t g, u_u8_t b, u_u8_t a); /**< Creates a color from red, green, blue, and alpha values in the range [0,256) */
u_color_t u_color_from_rgb(u_u8_t r, u_u8_t g, u_u8_t b); /**< Creates a color from red, green, blue values in the range [0,256). Alpha is set to 255. */
u_u32_t   u_color_to_u32(u_color_t color); /**< Converts a color to a 32-bit value in the form 0xRRGGBBAA */
u_color_t u_color_random(void); /**< Generates a random color (using `rand`). */
u_color_t u_color_random_rgb(void); /**< Generates a random color with alpha = 255. */
#endif /* CUTILS_MISC_COLOR_H */
