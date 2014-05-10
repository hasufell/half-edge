/*
 * Copyright 2011-2014 hasufell
 *
 * This file is part of a hasufell project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 2 of the License only.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file vector.h
 * Header for the external API of vector.c,
 * also holding the vector data structure.
 * @brief header of vector.c
 */

#ifndef _DROW_ENGINE_VEC_MATH_H
#define _DROW_ENGINE_VEC_MATH_H


#include <stdbool.h>


typedef struct vector vector;


/**
 * Represents a 3-dimensional vector.
 */
struct vector {
	float x;
	float y;
	float z;
};


bool vector_product(vector *a, vector *b, vector *c);
bool add_vectors(vector *a, vector *b, vector *c);
bool sub_vectors(vector *a, vector *b, vector *c);
bool normalize_vector(vector *a, vector *b);
bool copy_vector(vector *a, vector *b);


#endif /* _DROW_ENGINE_VEC_MATH_H */
