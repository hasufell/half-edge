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
#include <stdio.h>


/**
 * Fault intolerant macro. Will abort the program if the called
 * function failed.
 */
#define VECTOR_LEN_SCAL_MUL(...) \
{ \
	if (!vector_len_scal_mul(__VA_ARGS__)) { \
		fprintf(stderr, "Failure in vector_product()!\n"); \
		abort(); \
	} \
}

/**
 * Fault intolerant macro. Will abort the program if the called
 * function failed.
 */
#define VECTOR_PRODUCT(...) \
{ \
	if (!vector_product(__VA_ARGS__)) { \
		fprintf(stderr, "Failure in vector_product()!\n"); \
		abort(); \
	} \
}

/**
 * Fault intolerant macro. Will abort the program if the called
 * function failed.
 */
#define ADD_VECTORS(...) \
{ \
	if (!add_vectors(__VA_ARGS__)) { \
		fprintf(stderr, "Failure in add_vectors()!\n"); \
		abort(); \
	} \
}

/**
 * Fault intolerant macro. Will abort the program if the called
 * function failed.
 */
#define SUB_VECTORS(...) \
{ \
	if (!sub_vectors(__VA_ARGS__)) { \
		fprintf(stderr, "Failure in sub_vectors()!\n"); \
		abort(); \
	} \
}

/**
 * Fault intolerant macro. Will abort the program if the called
 * function failed.
 */
#define NORMALIZE_VECTOR(...) \
{ \
	if (!normalize_vector(__VA_ARGS__)) { \
		fprintf(stderr, "Failure in normalize_vector()!\n"); \
		abort(); \
	} \
}

/**
 * Fault intolerant macro. Will abort the program if the called
 * function failed.
 */
#define COPY_VECTOR(...) \
{ \
	if (!copy_vector(__VA_ARGS__)) { \
		fprintf(stderr, "Failure in copy_vector()!\n"); \
		abort(); \
	} \
}

/**
 * Fault intolerant macro. Will abort the program if the called
 * function failed.
 */
#define SET_NULL_VECTOR(...) \
{ \
	if (!set_null_vector(__VA_ARGS__)) { \
		fprintf(stderr, "Failure in set_null_vector()!\n"); \
		abort(); \
	} \
}


typedef struct vector vector;


/**
 * Represents a 3-dimensional vector.
 */
struct vector {
	float x;
	float y;
	float z;
};


bool vector_len_scal_mul(vector *a, float scal, vector *c);
bool vector_product(vector *a, vector *b, vector *c);
bool add_vectors(vector *a, vector *b, vector *c);
bool sub_vectors(vector *a, vector *b, vector *c);
bool normalize_vector(vector *a, vector *b);
bool copy_vector(vector *a, vector *b);
bool set_null_vector(vector *a);
int is_null_vector(vector *a);


#endif /* _DROW_ENGINE_VEC_MATH_H */
