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
#include <string.h>


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

	} while (edge && edge != vert->edge);

	/* set out-pointers */
	*edge_array_out = edge_array;
	*ec_out = ec; /* this is the real size, not the x[ec] value */

	return true;
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

	GET_ALL_EMANATING_EDGES(vert, &edge_array, &ec);
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
 */
bool normalize_object(HE_obj *obj)
{
	float scale_factor;

	if (!obj)
		return false;

	scale_factor = get_normalized_scale_factor(obj);

	for (uint32_t i = 0; i < obj->vc; i++) {
		obj->vertices[i].vec->x = obj->vertices[i].vec->x * scale_factor;
		obj->vertices[i].vec->y = obj->vertices[i].vec->y * scale_factor;
		obj->vertices[i].vec->z = obj->vertices[i].vec->z * scale_factor;
	}

	return true;
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
			 ec = 0, /* edge count */
			 dec = 0; /* dummy edge count */
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

		/* parse vertices and fill them */
		if (!strcmp(str_tmp_ptr, "v")) {
			char *myfloat = NULL;
			vector *tmp_vec = malloc(sizeof(vector));
			CHECK_PTR_VAL(tmp_vec);

			REALLOC(vertices,
					sizeof(HE_vert) * (vc + 1));

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

			/* set unused/unknown values to NULL */
			vertices[vc].edge = NULL;
			vertices[vc].edge_array = NULL;
			vertices[vc].eac = 0;
			vertices[vc].dc = 0;

			/* allocate color struct and set preliminary colors */
			vertices[vc].col = malloc(sizeof(color));
			vertices[vc].col->red = -1;
			vertices[vc].col->green = -1;
			vertices[vc].col->blue = -1;
			vc++;

			/* exceeds 3 dimensions, malformed vertice */
			if (strtok_r(NULL, " ", &str_ptr_space))
				ABORT("Failure in parse_obj(),\n"
						"malformed vertice, exceeds 3 dimensions!\n");

		/* parse plain faces and fill them (not HE_face!) */
		} else if (!strcmp(str_tmp_ptr, "f")) {
			char *myint = NULL;
			uint8_t i = 0;

			REALLOC(face_v, sizeof(FACE*) * (fc + 1));
			face_v[fc] = NULL;
			while ((myint = strtok_r(NULL, " ", &str_ptr_space))) {
				i++;
				ec++;

				REALLOC(face_v[fc],
						sizeof(FACE**) * (i + 1));
				face_v[fc][i - 1] = (uint32_t) atoi(myint);
				face_v[fc][i] = 0; /* so we can iterate over it */
			}
			fc++;
		}

		str_tmp_ptr = strtok_r(NULL, "\n", &str_ptr_newline);
	}

	faces = (HE_face*) malloc(sizeof(HE_face) * fc);
	CHECK_PTR_VAL(faces);
	/* hold enough space for possible dummy edges */
	edges = (HE_edge*) malloc(sizeof(HE_edge) * ec * 2);
	CHECK_PTR_VAL(edges);

	ec = 0;
	/* create HE_edges and real HE_faces */
	for (uint32_t i = 0; i < fc; i++) { /* for all faces */
		uint32_t j = 0,
				 fv_id; /* reference of the face vertex */

		/* for all vertices of the face */
		while ((fv_id = face_v[i][j])) {
			uint32_t fv_arr_id = fv_id - 1; /* fv_id starts at 1 */

			edges[ec].vert = &(vertices[fv_arr_id]);
			edges[ec].face = &(faces[i]);
			edges[ec].pair = NULL; /* preliminary */
			vertices[fv_arr_id].edge = &(edges[ec]); /* last one wins */
			vertices[fv_arr_id].dummys = NULL; /* preliminary */

			/* Skip j == 0 here, so we don't underrun the arrays,
			 * since we always look one edge back. The first edge
			 * element is taken care of below as well. */
			if (j > 0 ) {
				uint32_t *eac = &(edges[ec].vert->eac);

				/* connect previous edge to current edge */
				edges[ec - 1].next = &(edges[ec]);

				/* Acceleration struct:
				 * add previous edge to edge_array of current vertice */
				REALLOC(edges[ec].vert->edge_array,
						sizeof(HE_edge*) * (*eac + 1));
				edges[ec].vert->edge_array[*eac] = &(edges[ec - 1]);
				(*eac)++;

				if (!face_v[i][j + 1]) { /* no vertice left */
					uint32_t *eac;
					/* connect last edge to first edge */
					edges[ec].next = &(edges[ec - j]);
					eac = &(edges[ec].next->vert->eac);

					/* Acceleration struct:
					 * add last edge to edge_array element of first vertice */
					REALLOC(edges[ec].next->vert->edge_array,
							sizeof(HE_edge*) * (*eac + 1));
					edges[ec].next->vert->edge_array[*eac] = &(edges[ec]);
					(*eac)++;
				}
			}

			ec++;
			j++;
		}

		faces[i].edge = &(edges[ec - 1]); /* "last" edge */
	}

	/* find pairs */
	for (uint32_t i = 0; i < ec; i++) { /* for all edges */
		uint32_t eac = edges[i].vert->eac;
		bool pair_found = false;

		for (uint32_t j = 0; j < eac; j++) { /* for all potential pairs */
			if (edges[i].vert->edge_array[j] &&
					(edges[i].next->vert ==
					 edges[i].vert->edge_array[j]->vert)) {
				edges[i].pair = edges[i].vert->edge_array[j];
				edges[i].vert->edge_array[j] = NULL;

				pair_found = true;
				break;
			}
		}

		/* create dummy pair edge if we have a border edge */
		if (!pair_found) {
			uint32_t *vert_dc = &(edges[i].next->vert->dc);

			REALLOC(edges[i].next->vert->dummys,
					sizeof(HE_edge*) * (*vert_dc + 1));

			/* NULL-face indicates border-edge */
			edges[ec + dec].face = NULL;
			/* we don't know this one yet */
			edges[ec + dec].next = NULL;
			/* set both pairs */
			edges[ec + dec].pair = &(edges[i]);
			edges[i].pair = &(edges[ec + dec]);
			/* set vertex */
			edges[ec + dec].vert = edges[i].next->vert;
			/* add the dummy edge to the dummys array of the vertex */
			edges[ec + dec].vert->dummys[*vert_dc] = &(edges[ec + dec]);
			(*vert_dc)++;

			dec++;
		}
	}

	/* now we have to connect the dummy edges together */
	for (uint32_t i = 0; i < dec; i++) { /* for all dummy edges */
		/* vertex the dummy edge points to */
		HE_vert *vert = edges[ec + i].pair->vert;

		/* iterate over the dummy array */
		for (uint32_t j = 0; j < vert->dc; j++) {
			if (vert == vert->dummys[j]->vert)
				edges[ec + i].next = vert->dummys[j];
			j++;
		}
	}


	obj = (HE_obj*) malloc(sizeof(HE_obj));
	CHECK_PTR_VAL(obj);

	obj->edges = edges;
	obj->vertices = vertices;
	obj->faces = faces;
	obj->ec = ec;
	obj->vc = vc;
	obj->fc = fc;

	for (uint32_t i = 0; i < fc; i++)
		free(face_v[i]);
	free(face_v);
	for (uint32_t i = 0; i < vc; i++) {
		free(vertices[i].dummys);
		free(vertices[i].edge_array);
	}
	free(string);

	return obj;
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
	free(obj->edges);
	free(obj->vertices);
	free(obj->faces);
}
