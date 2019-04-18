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

/** \file voronoi.h
\brief Creating voronoi diagrams

Just something I made on the side to test out the k-d tree implementation I made.

Takes `O(width*height*log(nseeds))` time (in theory)
*/

#ifndef COLORREDUCER_VORONOI_H
#define COLORREDUCER_VORONOI_H

int cr_voronoify(const char* filename_in, const char* filename_out); /**< Turns a PNG file from a set of seeds (colored pixels) into a voronoi diagram. The image should be entirely white (#FFFFFFFF), except for the seeds of the voronoi diagram. \returns An error code, or zero on success. Check \ref u_error_message for the error message. */
int cr_voronoi_random(int width, int height, int nseeds, const char* filename_out); /**< Produces a voronoi diagram with a random set of \p nseeds different points and colors. \returns An error code. */
#endif /* COLORREDUCER_VORON_H */
