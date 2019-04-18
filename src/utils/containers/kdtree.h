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
\file kdtree.h
\brief k-d trees

The running time for kdtree_nearest (and kdtree_get) is approximately O(d log N),
but (for kdtree_nearest, not kdtree_get) in higher dimensions it might be higher
than this? See https://en.wikipedia.org/wiki/Curse_of_dimensionality#Nearest_neighbor_search

If you are inserting any data into a k-d tree **make sure it is randomized, or
at least not sorted**. Inserting sorted data into a k-d tree will drastically
reduce its performance.
*/
#ifndef CUTILS_CONTAINERS_KDTREE_H
#define CUTILS_CONTAINERS_KDTREE_H

#include <stddef.h>
#include <float.h>

typedef struct u_kdtree_node {
    size_t d; /**< Which dimension this tree splits on */
    struct u_kdtree_node* left; /**< The "left" child of this tree (items such that item[d] < data[d]). NULL for no left child.*/
    struct u_kdtree_node* right; /**< The "right" child of this tree (items such that item[d] >= data[d]). NULL for no right child. */
    float* key; /**< The key of this node. */
    void* value; /**< The value stored at this node. */
} u_kdtree_node_t;

typedef struct {
    u_kdtree_node_t* root; /**< The root node of the tree */
    size_t k; /**< How many dimensions data in this tree has. */
    size_t vsize; /**< The size of a value in this tree. */
} u_kdtree_t;

/** Constructs an empty k-d tree. vsize is the size of the values stored in the tree. Use 0 if you don't want to associate values with keys. */
void   u_kdtree_construct(u_kdtree_t* tree, size_t k, size_t vsize);
/** Removes all elements in a k-d tree. */
void   u_kdtree_clear(u_kdtree_t* tree);
/** Insert item into k-d tree. Makes a copy of key and value.
    \returns 0 on success, or non-zero if insertion failed (maybe you ran out of
     memory), and sets \ref u_error_message. */
int    u_kdtree_insert(u_kdtree_t* tree, const float* key, const void* value);
/** \returns value associated with key, or NULL iff the key is not present.
    If vsize == 0 and the key is present, returns (const void*)1.
    Actual key can differ by up to epsilon. For this function to work properly,
    epsilon must be small. For larger values of epsilon, use nearest neighbor. */
const void* u_kdtree_get(const u_kdtree_t* tree, const float* key, float epsilon);
 /** \returns the value associated with the closest key in the tree to key.
        Returns NULL iff either `vsize` == 0 or the tree is empty.
        Puts the nearest key in \p nearest_key (can be NULL). */
const void* u_kdtree_nearest(u_kdtree_t* tree, const float* key, const float** nearest_key);
/** Frees memory in tree. Does not call `free` on \p tree itself. */
void   u_kdtree_destroy(u_kdtree_t* tree);

#endif /* CUTILS_CONTAINERS_KDTREE_H */
