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
/** \file types_exact.h
\brief Types with exactly 1, 2, and 4 bytes.

You shouldn't include this file unless you really need to (e.g. for specific
file formats), because it could (in theory) cause compile-time errors on certain
systems.
*/

#ifndef CUTILS_MISC_TYPES_EXACT_H
#define CUTILS_MISC_TYPES_EXACT_H

#include <limits.h>

#if ((1 << CHAR_BIT) - 1) != UCHAR_MAX
#error "Invalid value of UCHAR_MAX."
#endif

#if (UCHAR_MAX >> 1) != CHAR_MAX
#error "Invalid value of CHAR_MAX."
#endif

typedef char u_i1b_t; /**< A type which holds exactly one byte. **/
typedef unsigned char u_u1b_t;  /**< An unsigned type which holds exactly one byte */

#if USHRT_MAX != ((UCHAR_MAX+1)*(UCHAR_MAX+1)-1)
#error "Invalid value of USHRT_MAX."
#endif

#if (USHRT_MAX >> 1) != SHRT_MAX
#error "Invalid value of SHRT_MAX."
#endif

typedef short u_i2b_t; /**< A type which holds exactly two bytes */
typedef unsigned short u_u2b_t; /**< An unsigned type which holds exactly two bytes. */

#if UINT_MAX == ((USHRT_MAX+1)*(USHRT_MAX+1)-1)

#if (UINT_MAX >> 1) != INT_MAX
#error "Invalid value of INT_MAX."
#endif

typedef int u_i4b_t; /**< A type which holds exactly four bytes. */
typedef unsigned u_u4b_t; /**< An unsigned type which holds exactly four bytes. */

#else
#if ULONG_MAX != ((USHRT_MAX+1)*(USHRT_MAX+1)-1)
#error "Cannot find 32-bit type."
#endif
#if (ULONG_MAX >> 1) != LONG_MAX
#error "Invalid value of LONG_MAX."
#endif

typedef long u_i4b_t; /**< A type which holds exactly four bytes. */
typedef unsigned long u_u4b_t; /**< An unsigned type which holds exactly four bytes. */

#endif


#endif /* CUTILS_MISC_TYPES_EXACT_H */
