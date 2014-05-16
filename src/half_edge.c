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
static int32_t get_row_count(int32_t const **array);
static int32_t get_face_count(FACES const faces);
static int32_t get_edge_count(FACES const faces);
static double **parse_2d_array(char const * const obj_string,
		char *item);
static FACES parse_face_array(char const * const obj_string);
static void assemble_HE_stage1(VERTICES obj_v,
		HE_vert *vertices,
		int32_t *vc);
static void assemble_HE_stage2(FACES obj_f,
		HE_vert *vertices,
		HE_face *faces,
		HE_edge *edges,
		int32_t *fc,
		int32_t *ec);
static void assemble_HE_stage3(HE_edge *edges,
		int32_t *ec,
		int32_t *dec);

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
 * Get the row count of the 2d array.
 *
 * @param array the 2 dimensional array
 * @return the row count
 */
static int32_t get_row_count(int32_t const **array)
{
	uint32_t rc = 0;

	if (!array)
		return -1;

	while (array[rc])
		rc++;

	return rc;

}

/**
 * Get the amount of faces as they are in the
 * .obj file.
 *
 * @param faces the faces array
 * @return the count of faces, -1 on failure
 */
static int32_t get_face_count(FACES const faces)
{
	if (!faces)
		return 0;

	return get_row_count((int32_t const**)faces);
}

/**
 * Get the amount of edges.
 *
 * @param faces the faces array which will be used
 * to calculate the amount of edges
 * @return the count of edges, -1 on failure
 */
static int32_t get_edge_count(FACES const faces)
{
	uint32_t ec = 0;
	uint32_t fc;

	if (!faces)
		return -1;

	fc = get_face_count(faces);

	for (uint32_t i = 0; i < fc; i++)
		ec += faces[i][0];

	return ec;
}

/**
 * Parse a string which supposedly is a 2-dimensional
 * array in the .obj file, describing all faces, all vertices
 * and such. The parsing depends on the item string, such as "f"
 * or "v".
 *
 * @param obj_string the string that is in obj format
 * @param item the item to look for, such as "f" or "v"
 * @return a newly allocated 2-dimensional array, NULL on failure
 */
static double **parse_2d_array(char const * const obj_string,
		char *item)
{
	uint32_t lc = 0;
	char *string,
		 *str_ptr_space = NULL, /* for strtok */
		 *str_ptr_newline = NULL, /* for strtok */
		 *str_tmp_ptr = NULL; /* for strtok */
	double **arr = NULL;

	if (!obj_string || !item)
		return NULL;

	string = malloc(sizeof(char) * strlen(obj_string) + 1);
	strcpy(string, obj_string);

	str_tmp_ptr = strtok_r(string, "\n", &str_ptr_newline);
	while (str_tmp_ptr && *str_tmp_ptr) {

		str_tmp_ptr = strtok_r(str_tmp_ptr, " ", &str_ptr_space);

		if (!strcmp(str_tmp_ptr, item)) {
			char *myint = NULL;
			uint8_t i = 1;

			REALLOC(arr, sizeof(double*) * (lc + 2));
			arr[lc] = NULL;
			while ((myint = strtok_r(NULL, " ", &str_ptr_space))) {
				i++;

				REALLOC(arr[lc],
						sizeof(double**) * (i + 1));
				arr[lc][i - 1] = atof(myint);
			}
			arr[lc][0] = i - 1; /* save length at first position */
			lc++;
			arr[lc] = NULL; /* trailing NULL pointer */
		}

		str_tmp_ptr = strtok_r(NULL, "\n", &str_ptr_newline);
	}

	free(string);

	return arr;
}

/**
 * Parses the face arrays. Since these contain slashes, such as
 * "f 1/4/3 8/4/4 9/8/3" we cannot use parse_2d_array() since
 * we need extra logic.
 *
 * @param obj_string the string that is in obj format
 * @return a newly allocated FACES array, NULL on failure
 */
static FACES parse_face_array(char const * const obj_string)
{
	uint32_t lc = 0;
	char *string,
		 *str_ptr_space = NULL, /* for strtok */
		 *str_ptr_newline = NULL, /* for strtok */
		 *str_tmp_ptr = NULL; /* for strtok */
	FACES arr = NULL;

	if (!obj_string)
		return NULL;

	string = malloc(sizeof(char) * strlen(obj_string) + 1);
	strcpy(string, obj_string);

	str_tmp_ptr = strtok_r(string, "\n", &str_ptr_newline);
	while (str_tmp_ptr && *str_tmp_ptr) {

		str_tmp_ptr = strtok_r(str_tmp_ptr, " ", &str_ptr_space);

		if (!strcmp(str_tmp_ptr, "f")) {
			char *myint = NULL;
			uint8_t i = 1;

			REALLOC(arr, sizeof(uint32_t*) * (lc + 2));
			arr[lc] = NULL;
			while ((myint = strtok_r(NULL, " ", &str_ptr_space))) {
				i++;

				REALLOC(arr[lc],
						sizeof(uint32_t**) * (i + 1));
				arr[lc][i - 1] = atoi(myint);
			}
			arr[lc][0] = i - 1; /* save length at first position */
			lc++;
			arr[lc] = NULL; /* trailing NULL pointer */
		}

		str_tmp_ptr = strtok_r(NULL, "\n", &str_ptr_newline);
	}

	free(string);

	return arr;
}

/**
 * First stage of assembling the half-edge data structure.
 * Here we allocate vertices and fill their coordinates
 * with the information we have from parsing the obj file.
 * All other yet unknown members such as edge are set to
 * NULL. This function isn't really modular, but makes
 * reading parse_obj() a bit less painful.
 *
 * @param obj_v the vertices in the raw form after they are
 * parsed from the obj file
 * @param vertices pointer the actual half-edge vertices
 * @param vc pointer to the vertice count
 */
static void assemble_HE_stage1(VERTICES obj_v,
		HE_vert *vertices,
		int32_t *vc)
{
	uint8_t const xpos = 1;
	uint8_t	const ypos = 2;
	uint8_t	const zpos = 3;
	int8_t const default_col = -1;

	*vc = 0;

	while (obj_v[*vc]) {
		vector *tmp_vec;

		if (obj_v[*vc][0] > 3)
			ABORT("Failure in parse_obj(),\n"
					"malformed vertice, exceeds 3 dimensions!\n");

		tmp_vec = malloc(sizeof(vector));
		CHECK_PTR_VAL(tmp_vec);

		tmp_vec->x = obj_v[*vc][xpos];
		tmp_vec->y = obj_v[*vc][ypos];
		tmp_vec->z = obj_v[*vc][zpos];

		vertices[*vc].vec = tmp_vec;

		/* set unused/unknown values to NULL */
		vertices[*vc].edge = NULL;
		vertices[*vc].edge_array = NULL;
		vertices[*vc].eac = 0;
		vertices[*vc].dc = 0;

		/* allocate color struct and set preliminary colors */
		vertices[*vc].col = malloc(sizeof(color));
		vertices[*vc].col->red = default_col;
		vertices[*vc].col->green = default_col;
		vertices[*vc].col->blue = default_col;

		(*vc)++;
	}
}

/**
 * Second stage of assembling the half-edge data structure.
 * Here we start creating the HE_edges and HE_faces and also
 * fill some missing information to the HE_verts along with it.
 * The edge pairs are still unknown, as well as some other
 * acceleration-structure related members like vertice->dummys.
 * This function isn't really modular, but makes
 * reading parse_obj() a bit less painful.
 *
 * @param obj_f the raw faces as they are after parsing the obj file
 * @param vertices the half-edge vertices
 * @param faces the half-edge faces
 * @param edges the half-edge edges
 * @param fc the count of half-edge faces
 * @param ec the count of half-edge edges
 */
static void assemble_HE_stage2(FACES obj_f,
		HE_vert *vertices,
		HE_face *faces,
		HE_edge *edges,
		int32_t *fc,
		int32_t *ec)
{
	*ec = 0;
	/* create HE_edges and real HE_faces */
	for (uint32_t i = 0; i < (uint32_t)(*fc); i++) { /* for all faces */

		/* for all vertices of the face */
		for (uint32_t j = 0; j < (uint32_t)obj_f[i][0]; j++) {
			uint32_t obj_f_pos = j + 1; /* first pos is reserved for length */
			uint32_t fv_arr_id =
				obj_f[i][obj_f_pos] - 1; /* fv_id starts at 1 */

			edges[*ec].vert = &(vertices[fv_arr_id]);
			edges[*ec].face = &(faces[i]);
			edges[*ec].pair = NULL; /* preliminary */
			vertices[fv_arr_id].edge = &(edges[*ec]); /* last one wins */
			vertices[fv_arr_id].dummys = NULL; /* preliminary */

			/* Skip j == 0 here, so we don't underrun the arrays,
			 * since we always look one edge back. The first edge
			 * element is taken care of below as well. */
			if (j > 0) {
				uint32_t *eac = &(edges[*ec].vert->eac);

				/* connect previous edge to current edge */
				edges[*ec - 1].next = &(edges[*ec]);

				/* Acceleration struct:
				 * add previous edge to edge_array of current vertice */
				REALLOC(edges[*ec].vert->edge_array,
						sizeof(HE_edge*) * (*eac + 1));
				edges[*ec].vert->edge_array[*eac] = &(edges[*ec - 1]);
				(*eac)++;

				if (obj_f_pos == (uint32_t)obj_f[i][0]) { /* no vertice left */
					uint32_t *eac;
					/* connect last edge to first edge */
					edges[*ec].next = &(edges[*ec - j]);
					eac = &(edges[*ec].next->vert->eac);

					/* Acceleration struct:
					 * add last edge to edge_array element of first vertice */
					REALLOC(edges[*ec].next->vert->edge_array,
							sizeof(HE_edge*) * (*eac + 1));
					edges[*ec].next->vert->edge_array[*eac] = &(edges[*ec]);
					(*eac)++;
				}
			}

			(*ec)++;
		}

		faces[i].edge = &(edges[*ec - 1]); /* "last" edge */
	}
}

/**
 * Third stage of assembling the half-edge data structure.
 * Here we find the pairs of edges and also account for the
 * possibility of border-edges, where we have to set up
 * dummy edges and connect them properly.
 *
 * @param edges the half-edge edges
 * @param ec the half-edge edges count
 * @param dec the dummy edges count
 */
static void assemble_HE_stage3(HE_edge *edges,
		int32_t *ec,
		int32_t *dec)
{
	/* find pairs */
	for (uint32_t i = 0; i < (uint32_t)(*ec); i++) { /* for all edges */
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
			edges[*ec + *dec].face = NULL;
			/* we don't know this one yet */
			edges[*ec + *dec].next = NULL;
			/* set both pairs */
			edges[*ec + *dec].pair = &(edges[i]);
			edges[i].pair = &(edges[*ec + *dec]);
			/* set vertex */
			edges[*ec + *dec].vert = edges[i].next->vert;
			/* add the dummy edge to the dummys array of the vertex */
			edges[*ec + *dec].vert->dummys[*vert_dc] = &(edges[*ec + *dec]);
			(*vert_dc)++;

			(*dec)++;
		}
	}

	/* now we have to connect the dummy edges together */
	for (uint32_t i = 0; i < (uint32_t) (*dec); i++) { /* for all dummy edges */
		/* vertex the dummy edge points to */
		HE_vert *vert = edges[*ec + i].pair->vert;

		/* iterate over the dummy array */
		for (uint32_t j = 0; j < vert->dc; j++) {
			if (vert == vert->dummys[j]->vert)
				edges[*ec + i].next = vert->dummys[j];
			j++;
		}
	}
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
 * @return the HE_face array that represents the object, NULL
 * on failure
 */
HE_obj *parse_obj(char const * const obj_string)
{
	int32_t vc = 0, /* vertices count */
			 fc = 0, /* face count */
			 ec = 0, /* edge count */
			 dec = 0; /* dummy edge count */
	char *string = NULL;
	HE_vert *vertices = NULL;
	HE_edge *edges = NULL;
	HE_face *faces = NULL;
	HE_obj *obj = NULL;
	FACES obj_f = NULL;
	/* V_TEXTURES obj_vt = NULL; */
	VERTICES obj_v = NULL;

	if (!obj_string || !*obj_string)
		return NULL;

	string = malloc(sizeof(char) * strlen(obj_string) + 1);
	strcpy(string, obj_string);

	obj_v = parse_2d_array(string, "v");
	/* obj_vt = parse_2d_array(obj_string, "vt"); */
	obj_f = parse_face_array(string);

	if ((ec = get_edge_count(obj_f)) == -1)
		ABORT("Invalid edge count!\n");
	if ((fc = get_face_count(obj_f)) == -1)
		ABORT("Invalid face count!\n");

	vertices = malloc(sizeof(HE_vert) *
			get_row_count((int32_t const**)obj_v) + 1);
	CHECK_PTR_VAL(vertices);
	faces = (HE_face*) malloc(sizeof(HE_face) * fc);
	CHECK_PTR_VAL(faces);
	/* hold enough space for possible dummy edges */
	edges = (HE_edge*) malloc(sizeof(HE_edge) * ec * 2);
	CHECK_PTR_VAL(edges);

	assemble_HE_stage1(obj_v, vertices, &vc);
	assemble_HE_stage2(obj_f, vertices, faces, edges, &fc, &ec);
	assemble_HE_stage3(edges, &ec, &dec);

	obj = (HE_obj*) malloc(sizeof(HE_obj));
	CHECK_PTR_VAL(obj);
	obj->edges = edges;
	obj->vertices = vertices;
	obj->faces = faces;
	obj->ec = ec;
	obj->vc = vc;
	obj->fc = fc;

	/* cleanup */
	for (uint32_t i = 0; i < (uint32_t)fc; i++)
		free(obj_f[i]);
	free(obj_f);
	for (uint32_t i = 0; i < (uint32_t)vc; i++) {
		free(vertices[i].dummys);
		free(vertices[i].edge_array);
		free(obj_v[i]);
	}
	free(obj_v);
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
