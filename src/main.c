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
#include "voronoi.h"
#include "colorreducer.h"
#include "audioreducer.h"
#include "rawreducer.h"
#include "textreducer.h"
#include "version.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils/misc/error.h"
#include "utils/filetypes/audio.h"
#include "utils/misc/args.h"

int voronoify_main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Error: Usage: %s <input image> <output image>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int err = cr_voronoify(argv[1], argv[2]);
    if (err) u_error_throw();

    return EXIT_SUCCESS;
}

int cr_main(int argc, char** argv) {
    if (argc < 4) {
        fprintf(stderr, "Error: Usage: %s <input image> <# of colors> <output image>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int err = cr_reduce_raw_file(argv[1], argv[3], atol(argv[2]), 0.001f, 0.000001f, 1000);
    if (err)
        u_error_throw();

    return EXIT_SUCCESS;
}

void show_help(void) {
    printf("ColorReducer Version %d.%d\n", CR_VERSION_MAJOR, CR_VERSION_MINOR);
    printf("Usage: ColorReducer <input file> <output file>\n");
    printf("Command-line options:\n"
            "-a, --audio\t\tSpecifies the input file as an audio file (currently only WAV is supported).\n"
            "-e, --epsilon\t\tSet the value for epsilon\n"
            "-i, --image\t\tSpecifies the input file as an image file (currently only PNG is supported).\n"
            "-k, --values\t\tSet the number of values to reduce the file to.\n"
            "-n, --iterations\tSet the number of iterations to run on the data.\n"
            "-r, --raw\t\tSpecifies the input file as a raw file.\n"
            "-t, --take\t\tSet how much of the data to actually use (from 0-1).\n"
            "-v, --voronoi\t\tInstead of color reducing, the input image will be turned into a voronoi diagram.\n"
            "-x, --text\t\tSpecifies the input file as a text file.\n");
}


typedef enum {
    AUDIO,
    IMAGE,
    RAW,
    TEXT
} input_type_t;

int main(int argc, char** argv) {
    srand(time(NULL));
    u_args_load(argc, argv);
    if (u_args_param_has('h', "--help")) {
        show_help();
        return 0;
    }

    /* Command line parameters */
    int is_audio = u_args_param_has('a', "audio");
    int is_image = u_args_param_has('i', "image");
    int is_raw   = u_args_param_has('r', "raw");
    int is_text  = u_args_param_has('t', "text");
    int is_voronoi = u_args_param_has('v', "voronoi");
    float epsilon = u_args_param_double_get('e', "epsilon", 0.000002);
    size_t iterations = u_args_param_long_get('n', "iterations", 2000);
    size_t values = u_args_param_long_get('k', "values", 5);
    float take = u_args_param_double_get('t', "take", 0.1);


    char* input_filename = NULL;
    char* output_filename = NULL;
    char** lone = u_args_lone_get(NULL);
    while (lone) {
        if (!input_filename) {
            input_filename = *lone;
        } else if (!output_filename) {
            output_filename = *lone;
        } else {
            fprintf(stderr, "Error: Stray argument: %s.\n", *lone);
            show_help();
            return EXIT_FAILURE;
        }
        lone = u_args_lone_get(lone);
    }

    if (!input_filename) {
        fprintf(stderr, "Error: You must supply an input and output file name.\n");
        show_help();
        return EXIT_FAILURE;
    }

    if (!output_filename) {
        fprintf(stderr, "Error: You must supply an output file name.\n");
        show_help();
        return EXIT_FAILURE;
    }

    if (is_voronoi) {
        int err = cr_voronoify(input_filename, output_filename);
        if (err) u_error_throw();
        return EXIT_SUCCESS;
    }

    input_type_t input_type;

    if (is_audio) {
        input_type = AUDIO;
    } else if (is_image) {
        input_type = IMAGE;
    } else if (is_raw) {
        input_type = RAW;
    } else if (is_text) {
        input_type = TEXT;
    } else {
        char* file_extension = strchr(input_filename, '.');
        if (!file_extension) {
            printf("No file extension on %s. Please specify -a, -i, -r, or -t.\n", input_filename);
            show_help();
            return EXIT_FAILURE;
        }
        if (!strncmp(file_extension, ".png", 4)) {
            input_type = IMAGE;
        } else if (!strncmp(file_extension, ".wav", 4)) {
            input_type = AUDIO;
        } else if (!strncmp(file_extension, ".txt", 4)) {
            input_type = TEXT;
        } else if (!strncmp(file_extension, ".raw", 4)) {
            input_type = RAW;
        } else {
            printf("Unrecognized file extension: %s. Please specify -a, -i, -r, or -t.\n", file_extension);
            show_help();
            return EXIT_FAILURE;
        }
    }

    int err;
    switch (input_type) {
    case AUDIO:
        err = cr_reduce_audio_file(input_filename, output_filename, values, take, epsilon, iterations);
        break;
    case IMAGE:
        err = cr_reduce_image_file(input_filename, output_filename, values, take, epsilon, iterations);
        break;
    case RAW:
        err = cr_reduce_raw_file(input_filename, output_filename, values, take, epsilon, iterations);
        break;
    case TEXT:
        err = cr_reduce_text_file(input_filename, output_filename, values, take, epsilon, iterations);
        break;
    }

    if (err) u_error_throw();

    /*cr_main(argc, argv);*/

    return EXIT_SUCCESS;
}
