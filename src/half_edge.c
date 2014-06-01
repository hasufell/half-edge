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
 * @file half_edge.c
 * This file provides operations on half-edge data structures
 * which are defined in half_edge.h, as well as assembling
 * such a struct.
 * @brief operations on half-edge data structs
 */

#include "common.h"
#include "err.h"
#include "filereader.h"
#include "half_edge.h"
#include "vector.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


/**
 * Fault intolerant macro. Will abort the program if the called
 * function failed.
 */
#define GET_ALL_EMANATING_EDGES(...) \
{ \
	if (!get_all_emanating_edges(__VA_ARGS__)) { \
		fprintf(stderr, "Failure in get_all_emanating_edges()!\n"); \
		abort(); \
	} \
}


/*
 * static declarations
 */
static bool get_all_emanating_edges(HE_vert const * const vert,
		HE_edge ***edge_array_out,
		uint32_t *ec_out);


/**
 * Get all edges that emanate from vertice and return a pointer
 * to that array with the size of ec_out.
 *
 * @param vert the vertice to get the emanating edges of
 * @param edge_array_out address of the 2d edge array to save
 * the result in [out]
 * @param ec_out the edge counter is saved here [out]
 * @return true/false for success/failure
 */
static bool get_all_emanating_edges(HE_vert const * const vert,
		HE_edge ***edge_array_out,
		uint32_t *ec_out)
{
	uint32_t ec = 0; /* edge count */
	uint32_t max_edges = 500; /* good guess to avoid infinite loop */
	HE_edge **edge_array = NULL;

	if (!edge_array_out || !vert || !ec_out)
		return false;

	HE_edge *edge = vert->edge;

	/* build an array of emanating edges */
	do {
		REALLOC(edge_array,
				sizeof(HE_edge*) * (ec + 1));

		edge_array[ec] = edge;

		edge = edge->pair->next;
		ec++;

		/* sanity check */
		if (ec > max_edges)
			goto loop_fail_cleanup;
	} while (edge && edge != vert->edge);

	/* set out-pointers */
	*edge_array_out = edge_array;
	*ec_out = ec; /* this is the real size, not the x[ec] value */

	return true;

loop_fail_cleanup:
	free(edge_array);
	return false;
}


/**
 * Calculate the normal of a face that corresponds
 * to edge.
 *
 * @param edge to align the normalization
 * @param vec the vector to store the result in [out]
 * @return true/false for success/failure
 */
bool face_normal(HE_edge const * const edge,
		vector *vec)
{
	vector he_vec1,
		   he_vec2,
		   he_base;

	if (!edge || !vec)
		return false;

	COPY_VECTOR(edge->next->vert->vec, &he_base);

	/* calculate vectors between the vertices */
	SUB_VECTORS(edge->next->next->vert->vec, &he_base, &he_vec1);
	SUB_VECTORS(edge->vert->vec, &he_base, &he_vec2);

	VECTOR_PRODUCT(&he_vec1, &he_vec2, vec);

	NORMALIZE_VECTOR(vec, vec);

	return true;
}

/**
 * Calculate the approximated normal of a vertex.
 *
 * @param vert the vertex
 * @param vec the vector to store the result in [out]
 * @return true/false for success/failure
 */
bool vec_normal(HE_vert const * const vert, vector *vec)
{
	HE_edge **edge_array = NULL;
	uint32_t ec;
	vector he_base;

	if (!vert || !vec)
		return false;

	/* fault tolerance if we didn't get any
	 * normal */
	if (!get_all_emanating_edges(vert, &edge_array, &ec))
		return false;

	COPY_VECTOR(edge_array[0]->vert->vec, &he_base);
	SET_NULL_VECTOR(vec); /* set to null for later summation */

	/* iterate over all edges, get the normalized
	 * face vector and add those up */
	for (uint32_t i = 0; i < ec; i++) {
		vector new_vec;

		if (edge_array[i]->face) {
			FACE_NORMAL(edge_array[i], &new_vec);
			ADD_VECTORS(vec, &new_vec, vec);
		}
	}

	/* normalize the result */
	NORMALIZE_VECTOR(vec, vec);

	free(edge_array);
	return true;
}

/**
 * Find the center of an object and store the coordinates
 * in a HE_vert struct.
 *
 * @param obj the object we want to find the center of
 * @param vec the vector to store the result in [out]
 * @return true/false for success/failure
 */
bool find_center(HE_obj const * const obj, vector *vec)
{
	float x = 0,
		  y = 0,
		  z = 0;
	uint32_t i;

	if (!obj || !vec)
		return false;


	for (i = 0; i < obj->vc; i++) {
		x += obj->vertices[i].vec->x;
		y += obj->vertices[i].vec->y;
		z += obj->vertices[i].vec->z;
	}

	vec->x = x / i;
	vec->y = y / i;
	vec->z = z / i;

	return true;
}

/**
 * Calculates the factor that can be used to scale down the object
 * to the size of 1.
 *
 * @param obj the object we want to scale
 * @return the corresponding scale factor, -1 on error
 */
float get_normalized_scale_factor(HE_obj const * const obj)
{
	float max;
	float min;
	uint32_t i;

	if (!obj)
		return -1;

	max = obj->vertices[0].vec->x +
		obj->vertices[0].vec->y + obj->vertices[0].vec->z;
	min = obj->vertices[0].vec->x +
		obj->vertices[0].vec->y + obj->vertices[0].vec->z;

	for (i = 0; i < obj->vc; i++) {
		if ((obj->vertices[i].vec->x +
				obj->vertices[i].vec->y +
				obj->vertices[i].vec->z) > max)
			max = obj->vertices[i].vec->x +
				obj->vertices[i].vec->y +
				obj->vertices[i].vec->z;
		else if ((obj->vertices[i].vec->x +
				obj->vertices[i].vec->y +
				obj->vertices[i].vec->z) < min)
			min = obj->vertices[i].vec->x +
				obj->vertices[i].vec->y +
				obj->vertices[i].vec->z;
	}

	return 1 / (max - min);
}

/**
 * Scales down the object to the size of 1. The parameter
 * is modified!
 *
 * @param obj the object we want to scale [mod]
 * @return true/false for success/failure
 */
bool normalize_object(HE_obj *obj)
{
	float scale_factor;

	if (!obj)
		return false;

	scale_factor = get_normalized_scale_factor(obj);

	for (uint32_t i = 0; i < obj->vc; i++) {
		obj->vertices[i].vec->x *= scale_factor;
		obj->vertices[i].vec->y *= scale_factor;
		obj->vertices[i].vec->z *= scale_factor;
	}

	for (uint32_t i = 0; i < obj->bzc; i++) {
		for (uint32_t j = 0; j <= obj->bez_curves[i].deg; j++) {
			obj->bez_curves[i].vec[j].x *= scale_factor;
			obj->bez_curves[i].vec[j].y *= scale_factor;
			obj->bez_curves[i].vec[j].z *= scale_factor;
		}
		i++;
	}

	return true;
}

/**
 * Free the inner structures of an object.
 *
 * @param obj the object to free
 */
void delete_object(HE_obj *obj)
{
	if (!obj)
		return;

	for (uint32_t i = 0; i < obj->vc; i++) {
		free(obj->vertices[i].vec);
		free(obj->vertices[i].col);
	}
	for (uint32_t i = 0; i < obj->bzc; i++)
		free(obj->bez_curves[i].vec);

	free(obj->edges);
	free(obj->vertices);
	free(obj->faces);
	free(obj->bez_curves);
}
