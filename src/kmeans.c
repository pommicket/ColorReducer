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
#include "kmeans.h"

#include "utils/containers/kdtree.h"
#include "utils/misc/error.h"
#include "utils/math/rand.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <assert.h>

#define CR_KMEANS_DEBUG

typedef struct {
    u_kdtree_t kdtree;
    size_t data_size, ndata, k;
    float* data;
    float* means;
    float* new_means;
    size_t* data_idxs;
    size_t* num_belonging_to;
    float change;
    u_bool_t has_kdtree, was_data_alloced;
} kmeans_state_t;

static void kmeans_state_free(kmeans_state_t* state) {
    assert(state);
    if (state->has_kdtree) {
        u_kdtree_destroy(&state->kdtree);
        state->has_kdtree = U_FALSE;
    }
    free(state->means);
    state->means = NULL;
    free(state->new_means);
    state->new_means = NULL;
    free(state->data_idxs);
    state->data_idxs = NULL;
    free(state->num_belonging_to);
    state->num_belonging_to = NULL;
    if (state->was_data_alloced)
        free(state->data);
}

static int kmeans_state_init(kmeans_state_t* state, float* data, size_t ndata, size_t data_size, size_t k, size_t take) {
    /* Initializes various variables and initializes means to random points. Returns an error code */

    memset(state, 0, sizeof(*state)); /* Most things are initialized to 0 (make sure pointers are NULL so that kmeans_state_free doesn't try to free them) */
    if (take > 0 && take < ndata) {

        state->data = malloc(take * data_size * sizeof(*state->data));
        if (!state->data) {
            kmeans_state_free(state);
            return u_error_nomem();
        }
        state->was_data_alloced = U_TRUE;
        size_t i, need = take, left = ndata, data_index = 0;
        for (i = 0; i < ndata; i++) {
            if (u_rand_double() < (double)need/left) {
                /* take this one */
                memcpy(&state->data[data_index * data_size], &data[i * data_size],
                       data_size * sizeof(*state->data));
                need--;
                data_index++;
            }
            left--;
        }
        assert(need == 0);
        state->ndata = take;

    } else {
        state->data = data;
        state->ndata = ndata;
    }

    state->data_size = data_size;
    state->k = k;
    if (state->k > state->ndata) {
        kmeans_state_free(state);
        return u_error_set(U_ERROR_ARGUMENT, "k must be less than or equal to the number of pieces of data.");
    }
    u_kdtree_construct(&state->kdtree, data_size, sizeof(size_t));
    state->has_kdtree = U_TRUE;
    state->means = malloc(k * data_size * sizeof(*state->means));

    if (!state->means) {
        kmeans_state_free(state);
        return u_error_nomem();
    }
    state->new_means = malloc(k * data_size * sizeof(*state->new_means));
    if (!state->new_means) {
        kmeans_state_free(state);
        return u_error_nomem();
    }

    state->data_idxs = malloc(state->ndata * sizeof(*state->data_idxs));
    if (!state->data_idxs) {
        kmeans_state_free(state);
        return u_error_nomem();
    }
    state->num_belonging_to = malloc(k * sizeof(*state->num_belonging_to));
    if (!state->num_belonging_to) {
        kmeans_state_free(state);
        return u_error_nomem();
    }

    size_t i;
    for (i = 0; i < state->ndata; i++)
        state->data_idxs[i] = i;

    int err =
        u_rand_shuffle(state->data_idxs, state->ndata, sizeof(*state->data_idxs));
    /* data_idxs[0..k] are the indices of the starting means */
    if (err) {
        kmeans_state_free(state);
        return err;
    }

    for (i = 0; i < k; i++) {
        memcpy(&state->means[i*data_size], &data[state->data_idxs[i]*data_size],
                data_size * sizeof(*state->means));
    }
    free(state->data_idxs);
    state->data_idxs = NULL;
    state->change = FLT_MAX;
    return U_ERROR_SUCCESS;
}

static int kmeans_state_build_kdtree(kmeans_state_t* state) {
    /* Builds a k-d tree from state->means. Returns an error code, and frees state if an error occurs.  */
    u_kdtree_clear(&state->kdtree);
    size_t i;
    for (i = 0; i < state->k; i++) {
        int err = u_kdtree_insert(&state->kdtree, &state->means[i*state->data_size], &i);
        if (err) {
            kmeans_state_free(state);
            return err;
        }
    }
    return U_ERROR_SUCCESS;
}

static int kmeans_state_run_iteration(kmeans_state_t* state) {
    /* Runs one iteration and sets state->change. Frees state and returns an error code if an error occurs. */
    int err = kmeans_state_build_kdtree(state);
    if (err) return err;
    size_t ds = state->data_size;

    /* Reset new_means to 0 */
    memset(state->new_means, 0, state->k * ds * sizeof(*state->new_means));
    /* Reset num_belonging_to to 0 */
    memset(state->num_belonging_to, 0, state->k * sizeof(*state->num_belonging_to));

    /* Find sum of points belonging to each mean */
    size_t i, j;
    for (i = 0; i < state->ndata; i++) {
        size_t belongs_to = *(size_t*)u_kdtree_nearest(&state->kdtree, &state->data[i*ds], NULL);
        for (j = 0; j < state->data_size; j++)
            state->new_means[belongs_to*ds+j] += state->data[i*ds+j];
        state->num_belonging_to[belongs_to]++;
    }

    /* Turn sum into mean, compute change. Move new_means to means. */
    state->change = 0;
    for (i = 0; i < state->k; i++) {
        for (j = 0; j < ds; j++) {
            size_t index = i*ds+j;
            if (state->num_belonging_to[i] != 0)
                state->new_means[index] /= state->num_belonging_to[i];
            state->change += fabs(state->means[index] - state->new_means[index]);
            state->means[index] = state->new_means[index];
        }
    }
    state->change  /= state->k * ds;
    return U_ERROR_SUCCESS;
}

int cr_kmeans_run(float* data, size_t ndata, size_t data_size, size_t k, size_t take, float epsilon, size_t iterations) {
    if (ndata == 0 || data_size == 0) return U_ERROR_ARGUMENT;
    kmeans_state_t state;
    int err = kmeans_state_init(&state, data, ndata, data_size, k, take);
    if (err) return err;

    size_t i = 0;
    while (state.change > epsilon && (iterations == 0 || i < iterations)) {
        #ifdef CR_KMEANS_DEBUG
        printf("Iteration %lu. Change: %f\n", i+1, state.change);
        #endif
        err = kmeans_state_run_iteration(&state);
        if (err) return err;
        i++;
    }

    #ifdef CR_KMEANS_DEBUG
    printf("Iteration %lu. Change: %f\n", i+1, state.change);
    #endif

    /* Move data to means */
    err = kmeans_state_build_kdtree(&state);
    if (err) return err;
    for (i = 0; i < ndata; i++) {
        size_t belongs_to = *(size_t*)u_kdtree_nearest(&state.kdtree, &data[i*data_size], NULL);
        memcpy(&data[i*data_size], &state.means[belongs_to*data_size], data_size * sizeof(*data));
    }
    kmeans_state_free(&state);
    return U_ERROR_SUCCESS;
}
