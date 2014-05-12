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


#include "err.h"
#include "filereader.h"
#include "half_edge.h"
#include "vector.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * static declarations
 */
static HE_edge **get_all_emanating_edges(HE_vert const * const vert,
		uint32_t *ec_out);


/**
 * Get all edges that emanate from vertice and return a pointer
 * to that array with the size of ec_out.
 *
 * @param vertice the vertice to get the emanating edges of
 * @param ec the edge counter is saved here [out]
 * @return pointer to an array of half-edges, size ec_out
 */
static HE_edge **get_all_emanating_edges(HE_vert const * const vert,
		uint32_t *ec_out)
{
	uint32_t ec = 0, /* edge count */
			 rc = 0; /* realloc count */
	uint32_t const approx_ec = 20; /* allocation chunk */
	HE_edge **edge_array = malloc(sizeof(HE_edge*) * approx_ec);
	HE_edge **tmp_ptr;

	if (!vert)
		return NULL;

	HE_edge *edge = vert->edge;

	/* build an array of emanating edges */
	do {
		edge_array[ec] = edge;

		edge = edge->pair->next;
		ec++;

		/* allocate more chunks */
		if (ec >= approx_ec) {
			tmp_ptr = realloc(edge_array, sizeof(HE_edge*)
						* approx_ec * (rc + 2));
			CHECK_PTR_VAL(tmp_ptr);
			edge_array = tmp_ptr;
			rc++;
		}

	} while (edge != vert->edge);

	/* this is the real size, not the x[ec] value */
	*ec_out = ec;

	return edge_array;
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
	HE_edge **edge_array;
	uint32_t ec,
			 vc = 0,
			 j;
	vector he_base;

	if (!vert || !vec)
		return false;

	/* get all emanating edges */
	if (!(edge_array = get_all_emanating_edges(vert, &ec)))
		return false;

	copy_vector(edge_array[0]->vert->vec, &he_base);

	vector vec_array[ec];

	/* iterate over all unique(!)
	 * tuples and calculate their product */
	for (uint32_t i = 0; i < ec; i++) {
		j = (i + 1) % ec;

		vector he_vec1,
			   he_vec2,
			   new_vec;

		copy_vector(edge_array[i]->next->vert->vec, &he_vec1);
		copy_vector(edge_array[j]->next->vert->vec, &he_vec2);

		if (!(set_null_vector(&new_vec)))
			return false;

		/* calculate vector between vertices */
		sub_vectors(&he_vec1, &he_base, &he_vec1);
		sub_vectors(&he_vec2, &he_base, &he_vec2);

		/* calculate vector product */
		if (!(vector_product(&he_vec2, &he_vec1, &new_vec)))
		/* if (!(vector_product(&he_vec1, &he_vec2, &new_vec))) */
			return false;

		/* normalize vector */
		if (!(normalize_vector(&new_vec, &new_vec)))
			return false;

		/* save into array */
		copy_vector(&new_vec, &(vec_array[vc]));
		vc++;
	}

	/* avoid side effects due to junk data */
	if (!(set_null_vector(vec)))
		return false;

	/* now add all the vectors up */
	for (uint32_t i = 0; i < vc; i++)
		if (!(add_vectors(vec, &(vec_array[i]), vec)))
			return false;

	/* normalize the result */
	if (!(normalize_vector(vec, vec)))
		return false;

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
 */
void normalize_object(HE_obj *obj)
{
	float scale_factor;
	scale_factor = get_normalized_scale_factor(obj);

	for (uint32_t i = 0; i < obj->vc; i++) {
		obj->vertices[i].vec->x = obj->vertices[i].vec->x * scale_factor;
		obj->vertices[i].vec->y = obj->vertices[i].vec->y * scale_factor;
		obj->vertices[i].vec->z = obj->vertices[i].vec->z * scale_factor;
	}
}

/**
 * Parse an .obj string and return a HE_obj
 * that represents the whole object.
 *
 * @param obj_string the whole string from the .obj file
 * @return the HE_face array that represents the object
 */
HE_obj *parse_obj(char const * const obj_string)
{
	uint32_t vc = 0, /* vertices count */
			 fc = 0, /* face count */
			 ec = 0; /* edge count */
	char *string,
		 *str_ptr_space = NULL, /* for strtok */
		 *str_ptr_newline = NULL, /* for strtok */
		 *str_tmp_ptr = NULL; /* for strtok */
	HE_vert *vertices = NULL;
	HE_edge *edges = NULL;
	HE_face *faces = NULL;
	HE_obj *obj = NULL;
	FACE face_v = NULL;

	if (!obj_string || !*obj_string)
		return NULL;

	string = malloc(sizeof(char) * strlen(obj_string) + 1);
	strcpy(string, obj_string);

	str_tmp_ptr = strtok_r(string, "\n", &str_ptr_newline);
	while (str_tmp_ptr && *str_tmp_ptr) {

		str_tmp_ptr = strtok_r(str_tmp_ptr, " ", &str_ptr_space);

		/* parse vertices */
		if (!strcmp(str_tmp_ptr, "v")) {
			char *myfloat = NULL;
			HE_vert *tmp_ptr;
			vector *tmp_vec = malloc(sizeof(vector));
			CHECK_PTR_VAL(tmp_vec);

			tmp_ptr = (HE_vert*) realloc(vertices,
					sizeof(HE_vert) * (vc + 1));
			CHECK_PTR_VAL(tmp_ptr);
			vertices = tmp_ptr;

			/* fill x */
			myfloat = strtok_r(NULL, " ", &str_ptr_space);
			CHECK_PTR_VAL(myfloat);
			tmp_vec->x = atof(myfloat);

			/* fill y */
			myfloat = strtok_r(NULL, " ", &str_ptr_space);
			CHECK_PTR_VAL(myfloat);
			tmp_vec->y = atof(myfloat);

			/* fill z */
			myfloat = strtok_r(NULL, " ", &str_ptr_space);
			CHECK_PTR_VAL(myfloat);
			tmp_vec->z = atof(myfloat);

			vertices[vc].vec = tmp_vec;

			/* set edge NULL */
			vertices[vc].edge = NULL;

			vc++;

			/* exceeds 3 dimensions, malformed vertice */
			if (strtok_r(NULL, " ", &str_ptr_space))
				return NULL;

		/* parse faces */
		} else if (!strcmp(str_tmp_ptr, "f")) {
			char *myint = NULL;
			uint8_t i = 0;
			FACE tmp_ptr = NULL;

			/* fill FACE */
			tmp_ptr = (FACE) realloc(face_v, sizeof(FACE*) * (fc + 1));
			CHECK_PTR_VAL(tmp_ptr);
			face_v = tmp_ptr;
			face_v[fc] = NULL;
			while ((myint = strtok_r(NULL, " ", &str_ptr_space))) {
				uint32_t *tmp_ptr = NULL;

				i++;
				ec++;

				tmp_ptr = (uint32_t*) realloc(face_v[fc],
						sizeof(FACE**) * (i + 1));
				CHECK_PTR_VAL(tmp_ptr);
				tmp_ptr[i - 1] = (uint32_t) atoi(myint);
				tmp_ptr[i] = 0; /* so we can iterate over it */
				face_v[fc] = tmp_ptr;
			}
			fc++;
		}

		str_tmp_ptr = strtok_r(NULL, "\n", &str_ptr_newline);
	}

	faces = (HE_face*) malloc(sizeof(HE_face) * fc);
	CHECK_PTR_VAL(faces);
	edges = (HE_edge*) malloc(sizeof(HE_edge) * ec);
	CHECK_PTR_VAL(edges);

	ec = 0;
	/* create HE_edges and real HE_faces */
	for (uint32_t i = 0; i < fc; i++) {
		uint32_t j = 0;

		/* for all vertices of the face */
		while (face_v[i][j]) {
			edges[ec].vert = &(vertices[face_v[i][j] - 1]);
			edges[ec].face = &(faces[j]);
			edges[ec].pair = NULL; /* preliminary */
			vertices[face_v[i][j] - 1].edge = &(edges[ec]); /* last one wins */

			if (face_v[i][j + 1]) /* connect to next vertice */
				edges[ec].next = &(edges[ec + 1]);
			else /* no vertices left, connect to first vertice */
				edges[ec].next = &(edges[ec - j]);

			ec++;
			j++;
		}

		faces[i].edge = &(edges[ec - 1]); /* "last" edge */
	}

	/* find pairs */
	/* TODO: acceleration */
	for (uint32_t i = 0; i < ec; i++) {
		HE_vert *next_vert = edges[i].next->vert;

		for (uint32_t j = 0; j < ec; j++)
			if (next_vert == edges[j].vert
					&& edges[j].next->vert == edges[i].vert)
				edges[i].pair = &(edges[j]);
	}

	obj = (HE_obj*) malloc(sizeof(HE_obj));
	CHECK_PTR_VAL(obj);

	obj->vertices = vertices;
	obj->vc = vc;
	obj->edges = edges;
	obj->ec = ec;
	obj->faces = faces;
	obj->fc = fc;

	free(string);
	for (uint32_t i = 0; i < fc; i++)
		free(face_v[i]);
	free(face_v);

	return obj;
}
