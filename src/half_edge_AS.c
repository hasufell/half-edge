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


/*
 * static function declaration
 */
static bool assemble_obj_arrays(char const * const obj_string,
		obj_items *raw_obj,
		HE_obj *he_obj);
static void assemble_HE_stage1(obj_items const * const raw_obj,
		HE_obj *he_obj);
static void assemble_HE_stage2(obj_items const * const raw_obj,
		HE_obj *he_obj);
static void assemble_HE_stage3(HE_obj *he_obj);
static void delete_accel_struct(HE_obj *he_obj);
static void delete_raw_object(obj_items *raw_obj,
		uint32_t fc,
		uint32_t vc,
		uint32_t vt,
		uint32_t bzc);


/**
 * Parse the obj_string for obj related arrays such as
 * "f 1 4 3 2" or "v 0.3 0.2 -1.2" and fill the related
 * raw obj_* structures which are not yet HE_* structures.
 * This function is a blob to take advantage of subsequent
 * strtok_r calls which allow us to parse the whole string only
 * once.
 *
 * NOTE: This function can be buggy for trailing whitespaces on
 * the end of lines or dos line endings.
 *
 * @param obj_string the string that is in obj format
 * @param raw_obj contains arrays of the items as they are in the .obj
 * file; members v, f and vt are set [out]
 * @param he_obj the half-edge object containing array-pointers
 * to all the HE_* structures; members ec, fc, vc and vtc are set [out]
 * @return true/false for success/failure
 */
static bool assemble_obj_arrays(char const * const obj_string,
		obj_items *raw_obj,
		HE_obj *he_obj)
{
	char *string;

	/* for strtok_r */
	char *str_ptr_space = NULL,
		 *str_ptr_newline = NULL,
		 *str_ptr_slash = NULL,
		 *str_tmp_ptr = NULL;

	/* these will be assigned later to the out structs */
	uint32_t vc = 0, fc = 0, ec = 0, vtc = 0, bzc = 0;
	VERTICES obj_v = NULL;
	FACES *obj_f = malloc(sizeof(*obj_f));
	uint32_t **obj_f_v = NULL; /* tmp v member of obj_f */
	uint32_t **obj_f_vt = NULL; /* tmp vt member of obj_f */
	V_TEXTURES obj_vt = NULL;
	BEZIER_CURV bez = NULL;

	if (!obj_string || !raw_obj)
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
		 * VERTEX TEXTURES
		 */
		} else if (!strcmp(str_tmp_ptr, "vt")) {
			char *myint = NULL;
			uint8_t i = 0;

			REALLOC(obj_vt, sizeof(*obj_vt) * (vtc + 2));
			obj_vt[vtc] = NULL;
			while ((myint = strtok_r(NULL, " ", &str_ptr_space))) {
				i++;

				REALLOC(obj_vt[vtc],
						sizeof(**obj_vt) * (i + 1));
				obj_vt[vtc][i - 1] = atof(myint);

				if (i > 3)
					ABORT("Malformed vertice texture exceeds 3 dimensions!\n");
			}
			vtc++;
			obj_vt[vtc] = NULL; /* trailing NULL pointer */

		/*
		 * FACES
		 */
		} else if (!strcmp(str_tmp_ptr, "f")) {
			char *myint_v = NULL,
				 *myint_vt = NULL;
			uint8_t i = 0;

			REALLOC(obj_f_v, sizeof(*obj_f_v) * (fc + 2));
			obj_f_v[fc] = NULL;

			REALLOC(obj_f_vt, sizeof(*obj_f_vt) * (fc + 2));
			obj_f_vt[fc] = NULL;

			while ((myint_v = strtok_r(NULL, " ", &str_ptr_space))) {
				/* is there a slash? */
				if ((myint_vt = strtok_r(myint_v, "/", &str_ptr_slash)))
					myint_v = myint_vt;
				else
					free(obj_f_vt); /* seems there is no vt, free the array */

				i++;
				ec++;

				REALLOC(obj_f_v[fc],
						sizeof(**obj_f_v) * (i + 1));
				obj_f_v[fc][i - 1] = atoi(myint_v);
				/* so we can iterate over it more easily */
				obj_f_v[fc][i] = 0;

				/* parse x from "0.3/x" */
				if ((myint_vt = strtok_r(NULL, "/", &str_ptr_slash))) {
					REALLOC(obj_f_vt[fc],
							sizeof(**obj_f_vt) * (i + 1));
					obj_f_vt[fc][i - 1] = atoi(myint_vt);
					/* so we can iterate over it more easily */
					obj_f_vt[fc][i] = 0;
				}
			}
			fc++;
			obj_f_v[fc] = NULL; /* trailing NULL pointer */

		/*
		 * Bezier Curve
		 */
		} else if (!strcmp(str_tmp_ptr, "curv")) {
			char *myint = NULL;
			uint8_t i = 0;

			REALLOC(bez, sizeof(*bez) * (bzc + 2));
			bez[bzc] = NULL;
			while ((myint = strtok_r(NULL, " ", &str_ptr_space))) {
				i++;

				REALLOC(bez[bzc],
						sizeof(**bez) * (i + 1));
				bez[bzc][i - 1] = atoi(myint);
				bez[bzc][i] = 0;
			}
			bzc++;
			bez[bzc] = NULL; /* trailing NULL pointer */
		}

		str_tmp_ptr = strtok_r(NULL, "\n", &str_ptr_newline);
	}

	/* assign the out variables */
	he_obj->ec = ec;
	he_obj->fc = fc;
	he_obj->vc = vc;
	he_obj->vtc = vtc;
	raw_obj->v = obj_v;
	obj_f->v = obj_f_v;
	obj_f->vt = obj_f_vt;
	raw_obj->f = obj_f;
	raw_obj->vt = obj_vt;
	raw_obj->bez = bez;

	/* cleanup */
	free(string);

	return true;
}

/**
 * First stage of assembling the half-edge data structure.
 * Here we allocate vertices and fill their coordinates
 * with the information we have from parsing the obj file,
 * as well as the bezier curves.
 * All other yet unknown members such as edge are set to
 * NULL. This function isn't really modular, but makes
 * reading parse_obj() a bit less painful.
 *
 * @param raw_obj contains arrays of the items as they are in the .obj
 * file
 * @param he_obj the half-edge object containing array-pointers
 * to all the HE_* structures; member vertices is set [out]
 */
static void assemble_HE_stage1(obj_items const * const raw_obj,
		HE_obj *he_obj)
{
	uint32_t vc = 0,
			 bzc = 0;
	uint8_t const xpos = 0;
	uint8_t	const ypos = 1;
	uint8_t	const zpos = 2;
	int8_t default_color = -1;
	HE_vert *vertices = he_obj->vertices;
	bez_curv *bez_curves = NULL;

	while (raw_obj->v[vc]) {
		vector *tmp_vec;

		tmp_vec = malloc(sizeof(vector));
		CHECK_PTR_VAL(tmp_vec);

		tmp_vec->x = raw_obj->v[vc][xpos];
		tmp_vec->y = raw_obj->v[vc][ypos];
		tmp_vec->z = raw_obj->v[vc][zpos];

		vertices[vc].vec = tmp_vec;

		/* set unused/unknown values to NULL */
		vertices[vc].edge = NULL;

		vertices[vc].col = malloc(sizeof(color));
		vertices[vc].col->red = default_color;
		vertices[vc].col->green = default_color;
		vertices[vc].col->blue = default_color;

		/* set acc structure */
		vertices[vc].acc = malloc(sizeof(HE_vert_acc));
		vertices[vc].acc->edge_array = NULL;
		vertices[vc].acc->eac = 0;
		vertices[vc].acc->dc = 0;

		vc++;
	}

	while (raw_obj->bez && raw_obj->bez[bzc]) {
		uint32_t i = 0;
		vector *bez_vec = NULL;

		REALLOC(bez_curves, sizeof(*bez_curves) * (bzc + 2));

		while (raw_obj->bez[bzc][i]) {
			REALLOC(bez_vec, sizeof(vector) * (i + 1));
			bez_vec[i] = *(vertices[raw_obj->bez[bzc][i] - 1].vec);
			i++;
		}

		bez_curves[bzc].vec = bez_vec;
		bez_curves[bzc].deg = i - 1; /* i is length */
		bzc++;
	}

	he_obj->bez_curves = bez_curves;
	he_obj->bzc = bzc;
	he_obj->vertices = vertices;
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
 * @param raw_obj contains arrays of the items as they are in the .obj
 * file
 * @param he_obj the half-edge object containing array-pointers
 * to all the HE_* structures; member vertices, edges
 * and faces are modified [out]
 */
static void assemble_HE_stage2(obj_items const * const raw_obj,
		HE_obj *he_obj)
{
	HE_vert *vertices = he_obj->vertices;
	HE_edge *edges = he_obj->edges;
	HE_face *faces = he_obj->faces;
	FACES *obj_f = raw_obj->f;

	uint32_t ec = 0,
			 fc = he_obj->fc;

	/* create HE_edges and real HE_faces */
	for (uint32_t i = 0; i < fc; i++) { /* for all faces */
		uint32_t j = 0;
		/* for all vertices of the face */
		while (obj_f->v[i][j]) {
			uint32_t fv_arr_id =
				obj_f->v[i][j] - 1; /* fv_id starts at 1 */

			edges[ec].vert = &(vertices[fv_arr_id]);
			edges[ec].face = &(faces[i]);
			edges[ec].pair = NULL; /* preliminary */
			vertices[fv_arr_id].edge = &(edges[ec]); /* last one wins */
			vertices[fv_arr_id].acc->dummys = NULL; /* preliminary */

			/* Skip j == 0 here, so we don't underrun the arrays,
			 * since we always look one edge back. The first edge
			 * element is taken care of below as well. */
			if (j > 0) {
				uint32_t *eac = &(edges[ec].vert->acc->eac);

				/* connect previous edge to current edge */
				edges[ec - 1].next = &(edges[ec]);

				/* Acceleration struct:
				 * add previous edge to edge_array of current vertice */
				REALLOC(edges[ec].vert->acc->edge_array,
						sizeof(HE_edge*) * (*eac + 1));
				edges[ec].vert->acc->edge_array[*eac] = &(edges[ec - 1]);
				(*eac)++;

				if (!obj_f->v[i][j + 1]) { /* no vertice left */
					uint32_t *eac;
					/* connect last edge to first edge */
					edges[ec].next = &(edges[ec - j]);
					eac = &(edges[ec].next->vert->acc->eac);

					/* Acceleration struct:
					 * add last edge to edge_array element of first vertice */
					REALLOC(edges[ec].next->vert->acc->edge_array,
							sizeof(HE_edge*) * (*eac + 1));
					edges[ec].next->vert->acc->edge_array[*eac] = &(edges[ec]);
					(*eac)++;
				}
			}

			ec++;
			j++;
		}

		faces[i].edge = &(edges[ec - 1]); /* "last" edge */
	}

	he_obj->vertices = vertices;
	he_obj->edges = edges;
	he_obj->faces = faces;
}

/**
 * Third stage of assembling the half-edge data structure.
 * Here we find the pairs of edges and also account for the
 * possibility of border-edges, where we have to set up
 * dummy edges and connect them properly.
 *
 * @param he_obj the half-edge object containing array-pointers;
 * member dec is set and edges is modified [out]
 */
static void assemble_HE_stage3(HE_obj *he_obj)
{
	HE_edge *edges = he_obj->edges;
	uint32_t ec = he_obj->ec;
	uint32_t dec = 0;

	/* find pairs */
	for (uint32_t i = 0; i < ec; i++) { /* for all edges */
		uint32_t eac = edges[i].vert->acc->eac;
		bool pair_found = false;

		for (uint32_t j = 0; j < eac; j++) { /* for all potential pairs */
			if (edges[i].vert->acc->edge_array[j] &&
					(edges[i].next->vert ==
					 edges[i].vert->acc->edge_array[j]->vert)) {
				edges[i].pair = edges[i].vert->acc->edge_array[j];
				edges[i].vert->acc->edge_array[j] = NULL;

				pair_found = true;
				break;
			}
		}

		/* create dummy pair edge if we have a border edge */
		if (!pair_found) {
			uint32_t *vert_dc = &(edges[i].next->vert->acc->dc);

			REALLOC(edges[i].next->vert->acc->dummys,
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
			edges[ec + dec].vert->acc->dummys[*vert_dc] = &(edges[ec + dec]);
			(*vert_dc)++;

			dec++;
		}
	}

	/* now we have to connect the dummy edges together */
	for (uint32_t i = 0; i < dec; i++) { /* for all dummy edges */
		/* vertex the dummy edge points to */
		HE_vert *vert = edges[ec + i].pair->vert;

		/* iterate over the dummy array */
		for (uint32_t j = 0; j < vert->acc->dc; j++) {
			if (vert == vert->acc->dummys[j]->vert)
				edges[ec + i].next = vert->acc->dummys[j];
		}
	}

	he_obj->edges = edges;
	he_obj->dec = dec;
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
	char *string = NULL,
		 *str_ptr;
	HE_obj *he_obj = NULL;
	obj_items raw_obj;

	if (!obj_string || !*obj_string)
		return NULL;

	string = malloc(sizeof(char) * strlen(obj_string) + 1);
	strcpy(string, obj_string);
	str_ptr = string;

	/*
	 * allocation for he_obj
	 */
	he_obj = (HE_obj*) malloc(sizeof(HE_obj));
	CHECK_PTR_VAL(he_obj);

	/*
	 * assemble pseudo-object, also sets vc, fc, ec
	 */
	if (!assemble_obj_arrays(string, &raw_obj, he_obj))
		return NULL;

	/*
	 * he_obj member allocation
	 */
	he_obj->vertices = malloc(sizeof(HE_vert) *
			(he_obj->vc + 1));
	CHECK_PTR_VAL(he_obj->vertices);
	he_obj->faces = (HE_face*) malloc(sizeof(HE_face) * he_obj->fc);
	CHECK_PTR_VAL(he_obj->faces);
	/* hold enough space for possible dummy edges */
	he_obj->edges = (HE_edge*) malloc(sizeof(HE_edge) * he_obj->ec * 2);
	CHECK_PTR_VAL(he_obj->edges);

	/*
	 * run the stages of assemblance
	 */
	assemble_HE_stage1(&raw_obj, he_obj);
	assemble_HE_stage2(&raw_obj, he_obj);
	assemble_HE_stage3(he_obj);

	/* cleanup */
	delete_raw_object(&raw_obj, he_obj->fc,
			he_obj->vc, he_obj->vtc, he_obj->bzc);
	delete_accel_struct(he_obj);
	free(string);

	return he_obj;
}

/**
 * Delete the acceleration structure of
 * HE_vert.
 */
static void delete_accel_struct(HE_obj *he_obj)
{
	for (uint32_t i = 0; i < he_obj->vc; i++) {
		free(he_obj->vertices[i].acc->dummys);
		free(he_obj->vertices[i].acc->edge_array);
		free(he_obj->vertices[i].acc);
	}
}

/**
 * Delete the raw obj pseudo struct which is only
 * used for assembling the HE_obj.
 */
static void delete_raw_object(obj_items *raw_obj,
		uint32_t fc,
		uint32_t vc,
		uint32_t vtc,
		uint32_t bzc)
{
	if (!raw_obj)
		return;

	for (uint32_t i = 0; i < bzc; i++)
		free(raw_obj->bez[i]);
	for (uint32_t i = 0; i < vtc; i++)
		free(raw_obj->vt[i]);
	for (uint32_t i = 0; i < vc; i++)
		free(raw_obj->v[i]);
	for (uint32_t i = 0; i < fc; i++) {
		free(raw_obj->f->v[i]);
		free(raw_obj->f->vt[i]);
	}
	free(raw_obj->bez);
	free(raw_obj->f->v);
	free(raw_obj->f->vt);
	free(raw_obj->v);
	free(raw_obj->vt);
	free(raw_obj->f);
}
