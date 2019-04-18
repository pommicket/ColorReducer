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
#include "kdtree.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../misc/error.h"

void u_kdtree_construct(u_kdtree_t* tree, size_t k, size_t vsize) {
    tree->k = k;
    tree->vsize = vsize;
    tree->root = NULL;
}

static void u_kdtree_node_free(u_kdtree_node_t* node) {
    if (!node) return;
    u_kdtree_node_free(node->left);
    u_kdtree_node_free(node->right);
    free(node->key);
    free(node->value);
    free(node);
}

void u_kdtree_clear(u_kdtree_t* tree) {
    u_kdtree_node_free(tree->root);
    tree->root = NULL;
}

static u_kdtree_node_t* u_kdtree_node_insert(u_kdtree_node_t* node, size_t k, size_t vsize, size_t last_dimension_split, const float* key, const void* value) {
    if (!node) {
        /* Create a new node */
        node = malloc(sizeof(*node));
        if (!node) {
            u_error_nomem();
            return NULL;
        }
        node->d = (last_dimension_split+1) % k;
        node->key = malloc(k * sizeof(*node->key));
        if (!node->key) {
            free(node);
            u_error_nomem();
            return NULL;
        }
        memcpy(node->key, key, k * sizeof(*key));
        if (vsize == 0) {
            node->value = NULL;
        } else {
            node->value = malloc(vsize);
            if (!node->value) {
                free(node);
                free(node->key);
                return NULL;
            }
            memcpy(node->value, value, vsize);
        }
        node->left = NULL;
        node->right = NULL;
        return node;
    }
    /* Insert into tree */
    if (key[node->d] < node->key[node->d]) {
        /* Insert into left subtree */
        u_kdtree_node_t* newnode = u_kdtree_node_insert(node->left, k, vsize, node->d, key, value);
        if (!newnode) return NULL;
        node->left = newnode;
    } else {
        /* Insert into right subtree */
        u_kdtree_node_t* newnode = u_kdtree_node_insert(node->right, k, vsize, node->d, key, value);
        if (!newnode) return NULL;
        node->right = newnode;
    }
    return node;
}

int u_kdtree_insert(u_kdtree_t* tree, const float* key, const void* value) {
    u_kdtree_node_t* retnode = u_kdtree_node_insert(tree->root, tree->k, tree->vsize, 0, key, value);
    if (!retnode) return -1;
    tree->root = retnode;
    return 0;
}

static float distance(const float* a, const float* b, size_t k) {
    size_t i;
    float sum_squared_diff = 0;
    for (i = 0; i < k; i++) {
        sum_squared_diff += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return sum_squared_diff;
}

static const void* u_kdtree_node_get(const u_kdtree_node_t* node, size_t k, size_t vsize, const float* key, float epsilon) {
    if (!node) return NULL;
    if (distance(node->key, key, k) < epsilon)
        return node->value ? node->value : (const void*)1 /* if vsize=0 */;

    if (key[node->d] < node->key[node->d]) {
        return u_kdtree_node_get(node->left, k, vsize, key, epsilon);
    } else {
        return u_kdtree_node_get(node->right, k, vsize, key, epsilon);
    }
}

const void* u_kdtree_get(const u_kdtree_t* tree, const float* key, float epsilon) {
    return u_kdtree_node_get(tree->root, tree->k, tree->vsize, key, epsilon);
}

static void u_kdtree_node_nearest(const u_kdtree_node_t* node, size_t k, size_t vsize, const float* key, float* best_distance, const float** best_key, const void** best_val) {
    if (!node) return;
    const u_kdtree_node_t* first_subtree;
    const u_kdtree_node_t* second_subtree;
    float difference = node->key[node->d] - key[node->d]; /* Distance from key to split line of node. */
    if (difference > 0) {
        first_subtree = node->left;
        second_subtree = node->right;
    } else {
        first_subtree = node->right;
        second_subtree = node->left;
        difference = -difference;
    }
    float distance_to_this_nodes_key = distance(node->key, key, k);
    if (distance_to_this_nodes_key < *best_distance) {
        *best_distance = distance_to_this_nodes_key;
        *best_key = node->key;
        *best_val = node->value;
    }
    u_kdtree_node_nearest(first_subtree, k, vsize, key, best_distance, best_key, best_val);
    if (*best_distance >= difference) {
        /* We need to check the second subtree */
        u_kdtree_node_nearest(second_subtree, k, vsize, key, best_distance, best_key, best_val);
    }
}

const void* u_kdtree_nearest(u_kdtree_t* tree, const float* key, const float** nearest_key) {
    if (tree->vsize == 0) return NULL;
    const float* nk;
    if (!nearest_key) nearest_key = &nk;
    float best_dist = FLT_MAX;
    const void* best_val;
    u_kdtree_node_nearest(tree->root, tree->k, tree->vsize, key, &best_dist, nearest_key, &best_val);
    return best_val;
}

void u_kdtree_destroy(u_kdtree_t* tree) {
    u_kdtree_node_free(tree->root);
}
