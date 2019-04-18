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
/**<
    \file types.h

    \brief Defines basic types.

    This file defines the type \ref u_bool_t and \ref U_FALSE and \ref U_TRUE for pre-C99/C++ code.

    It also defines some integer types of various sizes of the form u_[iu](8|16|32)_t.

*/
#ifndef CUTILS_MISC_TYPES_H
#define CUTILS_MISC_TYPES_H

#include <limits.h>

typedef unsigned char u_bool_t; /**< A type for boolean values */

#define U_FALSE ((u_bool_t)0) /**< False */
#define U_TRUE  ((u_bool_t)1) /**< True */



/* Some  sized types. */
#if CHAR_BIT < 8
#error "Invalid value of CHAR_BIT."
#endif

typedef char           u_i8_t; /**< The smallest type which has at least 8 bits */
typedef unsigned char  u_u8_t; /**< Unsigned version of \ref u_i8_t */

typedef u_u8_t u_byte_t; /**< A type for single-byte values */

#if SHRT_MAX < 32767
#error "Invalid value of SHRT_MAX."
#endif
typedef short          u_i16_t; /**< A type which is guaranteed to have at least 16 bits */
typedef unsigned short u_u16_t; /**< Unsigned version of \ref u_i16_t */

#if INT_MAX >= 2147483647L
typedef int            u_i32_t; /**< A type which is guaranteed to have at least 32 bits */
typedef unsigned int   u_u32_t; /**< Unsigned version of \ref u_i32_t */
#else
#if LONG_MAX < 2147483647L
#error "Invalid value of LONG_MAX"
#endif
typedef long           u_i32_t; /**< A type which is guaranteed to have at least 32 bits */
typedef unsigned long  u_u32_t; /**< Unsigned version of \ref u_i32_t */
#endif


#endif /* CUTILS_MISC_TYPES_H */
