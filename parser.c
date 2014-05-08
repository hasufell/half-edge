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

#include "err.h"
#include "filereader.h"
#include "types.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
	char *string = malloc(sizeof(char) * strlen(obj_string)),
		 *str_ptr_space = NULL, /* for strtok */
		 *str_ptr_newline = NULL, /* for strtok */
		 *str_tmp_ptr = NULL; /* for strtok */
	HE_vert *vertices = NULL;
	HE_edge *edges = NULL;
	HE_face *faces = NULL;
	HE_obj *obj = NULL;
	FACE face_v = NULL;

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

	faces = (HE_face*) malloc(fc);
	CHECK_PTR_VAL(faces);

	/* create HE_edges and real HE_faces */
	for (uint32_t i = 0; i < fc; i++) {
		uint32_t j = 0;

		/* for all vertices of the face */
		while (face_v[i][j]) {
			HE_edge *tmp_edge_ptr = NULL;

			tmp_edge_ptr = (HE_edge*) realloc(edges,
					sizeof(HE_edge) * (ec + 1));
			CHECK_PTR_VAL(tmp_edge_ptr);
			edges = tmp_edge_ptr;

			edges[ec].vert = &(vertices[face_v[i][j] - 1]);
			edges[ec].face = &(faces[j]);
			edges[ec].pair = NULL; /* preliminary */

			if (face_v[i][j + 1])
				edges[ec].next = &(edges[ec + 1]);
			else
				edges[ec].next = &(edges[ec - j]);

			ec++;
			j++;
		}

		faces[i].edge = &(edges[ec - 1]); /* "last" edge */
	}

	/* find pairs */
	/* TODO: acceleration */
	for (uint32_t i = 0; i < fc; i++) {
		HE_vert *next_vert = edges[i].next->vert;

		for (uint32_t j = 0; j < fc; j++)
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

	return obj;
}
