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
 * @file half_edge_AS.c
 * This files only purpose is to assemble the half-edge
 * data structure from an obj string.
 * @brief Half-edge assembler
 */

#include "common.h"
#include "err.h"
#include "filereader.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static bool assemble_obj_arrays(char const * const obj_string,
		VERTICES *obj_v_out,
		FACES *obj_f_out,
		uint32_t *vc_out,
		uint32_t *fc_out,
		uint32_t *ec_out);
static void assemble_HE_stage1(VERTICES obj_v,
		HE_vert *vertices,
		uint32_t *vc);
static void assemble_HE_stage2(FACES obj_f,
		HE_vert *vertices,
		HE_face *faces,
		HE_edge *edges,
		uint32_t *fc,
		uint32_t *ec);
static void assemble_HE_stage3(HE_edge *edges,
		uint32_t *ec,
		uint32_t *dec);


/**
 * Parse the obj_string for obj related arrays such as
 * "f 1 4 3 2" or "v 0.3 0.2 -1.2" and fill the related
 * raw obj_* structures which are not yet HE_* structures.
 * This function is a blob to take advantage of subsequent
 * strtok_r calls which allow us to parse the whole string only
 * once.
 *
 * @param obj_string the string that is in obj format
 * @param obj_v_out where to save the vertices, the rear
 * dimension will always have 3 elements for x,y,z [out]
 * @param obj_f_out where to save the faces, the rear dimension
 * will be terminated with a "0" [out]
 */
static bool assemble_obj_arrays(char const * const obj_string,
		VERTICES *obj_v_out,
		FACES *obj_f_out,
		uint32_t *vc_out,
		uint32_t *fc_out,
		uint32_t *ec_out)
{
	uint32_t vc = 0,
			 fc = 0,
			 ec = 0;
	char *string,
		 *str_ptr_space = NULL, /* for strtok */
		 *str_ptr_newline = NULL, /* for strtok */
		 *str_tmp_ptr = NULL; /* for strtok */
	VERTICES obj_v = NULL;
	FACES obj_f = NULL;

	if (!obj_string || !obj_v_out || !obj_f_out)
		return false;

	/* avoid side effects */
	string = malloc(sizeof(char) * strlen(obj_string) + 1);
	strcpy(string, obj_string);

	/* start parsing the string line by line */
	str_tmp_ptr = strtok_r(string, "\n", &str_ptr_newline);

	while (str_tmp_ptr && *str_tmp_ptr) {

		/* parse word by word */
		str_tmp_ptr = strtok_r(str_tmp_ptr, " ", &str_ptr_space);

		/*
		 * VERTICES
		 */
		if (!strcmp(str_tmp_ptr, "v")) {
			char *myint = NULL;
			uint8_t i = 0;

			REALLOC(obj_v, sizeof(*obj_v) * (vc + 2));
			obj_v[vc] = NULL;
			while ((myint = strtok_r(NULL, " ", &str_ptr_space))) {
				i++;

				REALLOC(obj_v[vc],
						sizeof(**obj_v) * (i + 1));
				obj_v[vc][i - 1] = atof(myint);

				if (i > 3)
					ABORT("Malformed vertice exceeds 3 dimensions!\n");
			}
			vc++;
			obj_v[vc] = NULL; /* trailing NULL pointer */

		/*
		 * FACES
		 */
		} else if (!strcmp(str_tmp_ptr, "f")) {
			char *myint = NULL;
			uint8_t i = 0;

			REALLOC(obj_f, sizeof(*obj_f) * (fc + 2));
			obj_f[fc] = NULL;
			while ((myint = strtok_r(NULL, " ", &str_ptr_space))) {
				i++;
				ec++;

				REALLOC(obj_f[fc],
						sizeof(**obj_f) * (i + 1));
				obj_f[fc][i - 1] = atoi(myint);
				obj_f[fc][i] = 0; /* so we can iterate over it more easily */
			}
			fc++;
			obj_f[fc] = NULL; /* trailing NULL pointer */
		}

		str_tmp_ptr = strtok_r(NULL, "\n", &str_ptr_newline);
	}

	free(string);

	*obj_v_out = obj_v;
	*obj_f_out = obj_f;
	*vc_out = vc;
	*fc_out = fc;
	*ec_out = ec;

	return true;
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
		uint32_t *vc)
{
	uint8_t const xpos = 0;
	uint8_t	const ypos = 1;
	uint8_t	const zpos = 2;
	int8_t const default_col = -1;

	*vc = 0;

	while (obj_v[*vc]) {
		vector *tmp_vec;

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
		uint32_t *fc,
		uint32_t *ec)
{
	*ec = 0;
	/* create HE_edges and real HE_faces */
	for (uint32_t i = 0; i < (uint32_t)(*fc); i++) { /* for all faces */
		uint32_t j = 0;
		/* for all vertices of the face */
		while (obj_f[i][j]) {
			uint32_t fv_arr_id =
				obj_f[i][j] - 1; /* fv_id starts at 1 */

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

				if (!obj_f[i][j + 1]) { /* no vertice left */
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
			j++;
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
		uint32_t *ec,
		uint32_t *dec)
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
 * Parse an .obj string and return a HE_obj
 * that represents the whole object.
 *
 * @param obj_string the whole string from the .obj file
 * @return the HE_face array that represents the object, NULL
 * on failure
 */
HE_obj *parse_obj(char const * const obj_string)
{
	uint32_t vc = 0, /* vertices count */
			 fc = 0, /* face count */
			 ec = 0, /* edge count */
			 dec = 0; /* dummy edge count */
	char *string = NULL,
		 *str_ptr;
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
	str_ptr = string;

	if (!assemble_obj_arrays(string, &obj_v, &obj_f, &vc, &fc, &ec))
		return NULL;


	/* if ((ec = get_edge_count(obj_f)) == -1) */
		/* ABORT("Invalid edge count!\n"); */
	/* if ((fc = get_face_count(obj_f)) == -1) */
		/* ABORT("Invalid face count!\n"); */

	vertices = malloc(sizeof(HE_vert) *
			(vc + 1));
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
