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


/**
 * Get all edges that emanate from vertice.
 *
 * @param vertice the vertice to get the emanating edges of
 * @return pointer to a NULL-terminated array of half-edges
 */
HE_edge **get_all_emanating_edges(HE_vert *vertice)
{
	return NULL;
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
		x += obj->vertices[i].x;
		y += obj->vertices[i].y;
		z += obj->vertices[i].z;
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

	max = obj->vertices[0].x +
		obj->vertices[0].y + obj->vertices[0].z;
	min = obj->vertices[0].x +
		obj->vertices[0].y + obj->vertices[0].z;

	for (i = 0; i < obj->vc; i++) {
		if ((obj->vertices[i].x +
				obj->vertices[i].y +
				obj->vertices[i].z) > max)
			max = obj->vertices[i].x +
				obj->vertices[i].y +
				obj->vertices[i].z;
		else if ((obj->vertices[i].x +
				obj->vertices[i].y +
				obj->vertices[i].z) < min)
			min = obj->vertices[i].x +
				obj->vertices[i].y +
				obj->vertices[i].z;
	}

	return 1 / (max - min);
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

			tmp_ptr = (HE_vert*) realloc(vertices,
					sizeof(HE_vert) * (vc + 1));
			CHECK_PTR_VAL(tmp_ptr);
			vertices = tmp_ptr;

			/* fill x */
			myfloat = strtok_r(NULL, " ", &str_ptr_space);
			CHECK_PTR_VAL(myfloat);
			vertices[vc].x = atof(myfloat);

			/* fill y */
			myfloat = strtok_r(NULL, " ", &str_ptr_space);
			CHECK_PTR_VAL(myfloat);
			vertices[vc].y = atof(myfloat);

			/* fill z */
			myfloat = strtok_r(NULL, " ", &str_ptr_space);
			CHECK_PTR_VAL(myfloat);
			vertices[vc].z = atof(myfloat);

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
			if (next_vert == edges[j].vert)
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
