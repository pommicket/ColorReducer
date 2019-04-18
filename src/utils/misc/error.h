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
\file error.h
\brief Some error codes for you to return if something goes wrong...

Personally, I'd recommend returning `int`, rather than \ref u_error_t, because
then you can easily add customized error codes without having to edit this
file directly.
 */

#ifndef CUTILS_MISC_ERROR_H
#define CUTILS_MISC_ERROR_H

typedef enum {
    U_ERROR_SUCCESS, /**< Function completed successfully. */
    U_ERROR_ARGUMENT, /**< A general invalid argument error */
    U_ERROR_NULL, /**< An argument was `NULL` and it shouldn't have been. */
    U_ERROR_NOMEM, /**< No memory left! E.g. for when `*alloc` returns `NULL`.  */
    U_ERROR_BOUNDS, /**< An index is out of bounds. */
    U_ERROR_DOMAIN, /**< Parameter is outside of the domain of the function (e.g. division by 0). */
    U_ERROR_ACCESS, /**< Unable to access a resource (due to lack of permission. etc., but not if resource was not found (see \ref U_ERROR_NOT_FOUND)) */
    U_ERROR_NOT_FOUND, /**< Unable to access a resource (file, etc.), because it does not exist or was not found. */
    U_ERROR_FORMAT, /**< Invalid file (or other) format. */
    U_ERROR_SYSTEM, /**< This function does not support the user's system (perhaps because does not have a necessary feature, e.g. 64-bit numbers). */
    U_ERROR_OTHER /**< Some other error */
} u_error_t;


void u_error_throw(void); /**< Prints error message to stderr, and exits. */
/** Sets \ref u_error_message to \p message and \ref u_error_code to \p error_code.
    \returns \p error_code. */
int  u_error_set(int error_code, const char* message);
/** Sets the error message to an appropriate string for when a memory allocation
    fails.
    \returns \ref U_ERROR_NOMEM. */
int  u_error_nomem(void);
/**
Sets the error message to an appropriate string when fopen fails.
\param filename The name of the file which failed to open.
\param mode A description of the mode (e.g. "writing", "reading").
\returns \ref U_ERROR_ACCESS.
 */
int  u_error_fopen(const char* filename, const char* mode);

#define U_ERROR_MESSAGE_SIZE 1024 /**< The maximum length of \ref u_error_message. */
extern char u_error_message[U_ERROR_MESSAGE_SIZE]; /**< The error message */
extern int  u_error_code; /**< The error code */
#endif /* CUTILS_MISC_ERROR_H */
