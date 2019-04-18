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
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char u_error_message[U_ERROR_MESSAGE_SIZE];
int u_error_code = U_ERROR_SUCCESS;

#define U_ERROR_NOMEM_STRING "Memory allocation failed."

void u_error_throw(void) {
    fprintf(stderr, "Error (code %d): %s\n", u_error_code, u_error_message);
    exit(EXIT_FAILURE);
}

int u_error_set(int error_code, const char* message) {
    u_error_code = error_code;
    strncpy(u_error_message, message, U_ERROR_MESSAGE_SIZE);
    return error_code;
}

int u_error_nomem(void) {
    return u_error_set(U_ERROR_NOMEM, U_ERROR_NOMEM_STRING);
}

int u_error_fopen(const char* filename, const char* mode) {

    char fname[256];
    strncpy(fname, filename, 255);

    char md[256];
    strncpy(md, mode, 255);

    sprintf(u_error_message, "Could not open file for %s: %s.", md, fname);

    return U_ERROR_ACCESS;
}
