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
#include "args.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

static int sd_args_are_boolean[256];
static int argc;
static char** argv;


static int is_sd(const char* str) {
    if (!str[0]) return 0;
    return str[0] == '-' && str[1] != '-';
}

static int is_dd(const char* str) {
    if (!str[0]) return 0;
    return str[0] == '-' && str[1] == '-';
}

static int has_sd(const char* str, char sd) {
    if (!is_sd(str)) return 0;
    return strchr(str+1, sd) != NULL;
}

static int has_dd(const char* str, const char* dd) {
    if (!is_dd(str)) return 0;
    return !strcmp(str+2, dd);
}

void u_args_load(int ac, char** av) {
    memset(sd_args_are_boolean, 0, sizeof(sd_args_are_boolean));
    argc = ac;
    argv = av;
}

int u_args_has(const char* arg) {
    int i;
    for (i = 1; i < argc; i++) {
        if (!strcmp(arg, argv[i]))
            return i;
    }
    return 0;
}


int u_args_param_has(char sd, const char* dd) {
    int i;
    for (i = 1; i < argc; i++) {
        if (has_sd(argv[i], sd)) {
            sd_args_are_boolean[(unsigned)sd] = 1;
            return i;
        }
        if (has_dd(argv[i], dd))
            return i;
    }
    return 0;
}


int u_args_param_long_has(char sd, const char* dd) {
    int i;
    for (i = 1; i < argc; i++) {
        if (i < argc-1 && has_sd(argv[i], sd)) {
            long l;
            if (sscanf(argv[i+1], "%ld", &l) == 1)
                return i;
        }
        if (!is_dd(argv[i])) continue;
        size_t len = strlen(dd);
        if (!strncmp(argv[i]+2, dd, len)) {
            if (argv[i][len+2] == '=') {
                long l;
                if (sscanf(&argv[i][len+3], "%ld", &l) == 1)
                    return i;
            }
        }
    }
    return 0;
}


int u_args_param_double_has(char sd, const char* dd) {
    int i;
    for (i = 1; i < argc; i++) {
        if (i < argc-1 && has_sd(argv[i], sd)) {
            double d;
            if (sscanf(argv[i+1], "%lf", &d) == 1)
                return i;
        }
        if (!is_dd(argv[i])) continue;
        size_t len = strlen(dd);
        if (!strncmp(argv[i]+2, dd, len)) {
            if (argv[i][len+2] == '=') {
                double d;
                if (sscanf(&argv[i][len+3], "%lf", &d) == 1)
                    return i;
            }
        }
    }
    return 0;
}


int u_args_param_str_has(char sd, const char* dd) {
    int i;
    for (i = 1; i < argc; i++) {
        if (i < argc-1 && has_sd(argv[i], sd)) {
            return i;
        }
        if (!is_dd(argv[i])) continue;
        size_t len = strlen(dd);
        if (!strncmp(argv[i]+2, dd, len)) {
            if (argv[i][len+2] == '=') {
                return i;
            }
        }
    }
    return 0;
}

long u_args_param_long_get(char sd, const char* dd, long def) {
    int i = u_args_param_long_has(sd, dd);
    if (i == 0) return def;
    if (i < argc-1 && has_sd(argv[i], sd)) {
        long l;
        if (sscanf(argv[i+1], "%ld", &l) == 1)
            return l;
    }
    assert(is_dd(argv[i]));
    size_t len = strlen(dd);
    if (!strncmp(argv[i]+2, dd, len)) {
        if (argv[i][len+2] == '=') {
            long l;
            if (sscanf(&argv[i][len+3], "%ld", &l) == 1)
                return l;
        }
    }
    return def;
}

double u_args_param_double_get(char sd, const char* dd, double def) {
    int i = u_args_param_double_has(sd, dd);
    if (i == 0) return def;
    if (i < argc-1 && has_sd(argv[i], sd)) {
        double d;
        if (sscanf(argv[i+1], "%lf", &d) == 1)
            return d;
    }
    assert(is_dd(argv[i]));
    size_t len = strlen(dd);
    if (!strncmp(argv[i]+2, dd, len)) {
        if (argv[i][len+2] == '=') {
            double d;
            if (sscanf(&argv[i][len+3], "%lf", &d) == 1)
                return d;
        }
    }
    return def;
}

const char* u_args_param_str_get(char sd, const char* dd, const char* def) {
    int i = u_args_param_str_has(sd, dd);
    if (i == 0) return def;
    if (i < argc-1 && has_sd(argv[i], sd)) {
        return argv[i+1];
    }
    assert(is_dd(argv[i]));
    size_t len = strlen(dd);
    if (!strncmp(argv[i]+2, dd, len)) {
        if (argv[i][len+2] == '=') {
            return &argv[i][len+3];
        }
    }
    return def;
}


char** u_args_lone_get(char** from) {
    if (from == NULL) from = argv;
    int index = from - argv + 1;
    while (1) {
        if (index > argc - 1) {
            return NULL;
        }
        char* arg = argv[index];
        if (arg[0] == '-') {
            index++;
            continue;
        }
        if (index != 0 && is_sd(argv[index-1]) && !sd_args_are_boolean[(unsigned)argv[index-1][1]]) {
            index++;
            continue;
        }
        break;
    }

    return argv + index;
}
