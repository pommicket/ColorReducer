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
#include "color.h"

#include <stdlib.h>

u_color_t u_color_from_u32_rgba(u_u32_t rgba) {
    u_color_t color;
    color.r = (rgba >> 24) & 0xFF;
    color.g = (rgba >> 16) & 0xFF;
    color.b = (rgba >>  8) & 0xFF;
    color.a = (rgba >>  0) & 0xFF;
    return color;
}

u_color_t u_color_from_u32_rgb(u_u32_t rgb) {
    return u_color_from_u32_rgba((rgb << 8) | 0xFF);
}

u_color_t u_color_from_rgba(u_u8_t r, u_u8_t g, u_u8_t b, u_u8_t a) {
    u_color_t color = {r, g, b, a};
    return color;
}

u_color_t u_color_from_rgb(u_u8_t r, u_u8_t g, u_u8_t b) {
    return u_color_from_rgba(r, g, b, 255);
}

u_u32_t u_color_to_u32(u_color_t color) {
    return (color.r << 24) + (color.g << 16) + (color.b << 8) + (color.a << 0);
}

u_color_t u_color_random(void) {
    return u_color_from_rgba(rand() & 0xFF, rand() & 0xFF, rand() & 0xFF, rand() & 0xFF);
}

u_color_t u_color_random_rgb(void) {
    return u_color_from_rgb(rand() & 0xFF, rand() & 0xFF, rand() & 0xFF);
}
