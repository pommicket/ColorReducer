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
/** \file args.h
\brief Functions for dealing with command-line arguments

Command-line parameters have two different forms: a boolean parameter such as
verbosity, could possibly either be enabled by using the argument `-v` or
`--verbose`. For integer or string parameters, a parameter such as "input file"
could be passed in either using `-f some_file.txt` or `--input-file=some_file.txt`

In the first case, to check if the parameter was enabled, you would use
`u_args_param_has('v',"verbose")`. In the second case, to check if the parameter
was enabled, you would use `u_args_param_str_has('f',"input-file")`, and to get
its value, you would use `u_args_param_str_get('f',"input-file","default_input_file.txt")`.

There are also lone arguments, which are arguments which are not flags and
are not part of another parameter. Before calling \ref u_args_lone_get, you
should call \ref u_args_param_has for each boolean parameter, so that it knows
which single-character options are booleans.


These functions are not particularly efficient. In the worst case, they use
time proportional to the total number of characters in all command-line arguments.
*/
#ifndef CUTILS_MISC_ARGS_H
#define CUTILS_MISC_ARGS_H

/** Loads arguments into this argument parser.
    \param argc The number of arguments.
    \param argv An array of arguments. */
void u_args_load(int argc, char** argv);
/** \returns A non-zero value if \p arg was one of the arguments passed in. */
int  u_args_has(const char* arg);
/** \returns A non-zero value if the paramter defined by \p sd and \p dd is enabled. */
int  u_args_param_has(char sd, const char* dd);
/** \returns A non-zero value if the parameter whose value should be a `long`,
    defined by \p sd and \p dd, is enabled.*/
int  u_args_param_long_has(char sd, const char* dd);
/** \returns A non-zero value if the parameter whose value should be a `double`,
    defined by \p sd and \p dd, is enabled. */
int  u_args_param_double_has(char sd, const char* dd);
/** \returns A non-zero value if the parameter whose value should be a string,
    defined by \p sd and \p dd, is enabled. */
int  u_args_param_str_has(char sd, const char* dd);
/** \returns The value of a `long` parameter defined by \p sd and \p dd. */
long u_args_param_long_get(char sd, const char* dd, long def);
/** \returns The value of a `double` parameter defined by \p sd and \p dd. */
double u_args_param_double_get(char sd, const char* dd, double def);
/** \returns The value of a string parameter defined by \p sd and \p dd. */
const char* u_args_param_str_get(char sd, const char* dd, const char* def);
/**
    \returns A pointer to the next lone argument after \p from, or `NULL` if
    there are none left. You can pass in `NULL` to get a pointer to the first
    lone argument.
 */
char** u_args_lone_get(char** from);

#endif /* CUTILS_MISC_ARGS_H */
