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
 * @file half_edge.h
 * Header for the external API of half_edge.c
 * and half_edge_AS.c,
 * primarily holding the common half-edge data
 * structures.
 * @brief header of half_edge related functions
 */

#ifndef _DROW_ENGINE_HE_OPERATIONS_H
#define _DROW_ENGINE_HE_OPERATIONS_H


#include "vector.h"

#include <stdbool.h>
#include <stdint.h>


/**
 * Fault intolerant macro. Will abort the program if the called
 * function failed.
 */
#define FACE_NORMAL(...) \
{ \
	if (!face_normal(__VA_ARGS__)) { \
		fprintf(stderr, "Failure in face_normal()!\n"); \
		abort(); \
	} \
}

/**
 * Fault intolerant macro. Will abort the program if the called
 * function failed.
 */
#define VEC_NORMAL(...) \
{ \
	if (!vec_normal(__VA_ARGS__)) { \
		fprintf(stderr, "Failure in vec_normal()!\n"); \
		abort(); \
	} \
}

/**
 * Fault intolerant macro. Will abort the program if the called
 * function failed.
 */
#define FIND_CENTER(...) \
{ \
	if (!find_center(__VA_ARGS__)) { \
		fprintf(stderr, "Failure in find_center()!\n"); \
		abort(); \
	} \
}

/**
 * Fault intolerant macro. Will abort the program if the called
 * function failed.
 */
#define NORMALIZE_OBJECT(...) \
{ \
	if (!normalize_object(__VA_ARGS__)) { \
		fprintf(stderr, "Failure in normalize_object()!\n"); \
		abort(); \
	} \
}


typedef struct obj_items obj_items;
/**
 * 2d array which can hold the vertices
 * as they are in the .obj file,
 */
typedef double** VERTICES;
typedef struct FACES FACES;
/**
 * 2d array which can hold the vertices texture
 * coordinates as they are in the .obj file,
 */
typedef double** V_TEXTURES;
typedef struct HE_edge HE_edge;
typedef struct HE_vert HE_vert;
typedef struct HE_vert_acc HE_vert_acc;
typedef struct HE_face HE_face;
typedef struct HE_obj HE_obj;
typedef struct color color;


/**
 * This will hold all supported items,
 * such as "v" (for vertice), "f" (for face)
 * or "vt" (for vertex texture) as they are
 * in the .obj file, not converted to anything.
 */
struct obj_items {
	/**
	 * Raw vertices array.
	 */
	VERTICES v;
	/**
	 * Raw faces array.
	 */
	FACES *f;
	/**
	 * Raw texture coordinates array.
	 */
	V_TEXTURES vt;
};

/**
 * Represents the raw faces as they are in the .obj
 * file with their references to vertices etc.
 */
struct FACES {
	/**
	 * References to the vertices which
	 * will form the face.
	 */
	uint32_t **v;
	/**
	 * Reference to the texture coordinates
	 * which will form the polygon texture.
	 */
	uint32_t **vt;
};

/**
 * Represents a half-edge.
 */
struct HE_edge {
	/**
	 * Start-vertex of the halfe-edge.
	 */
	HE_vert *vert;
	/**
	 * Oppositely oriented adjacent half-edge.
	 * Border edges have a dummy pair which have "face"
	 * and "next" set to NULL.
	 */
	HE_edge *pair;
	/**
	 * Face the half-edge borders.
	 */
	HE_face *face;
	/**
	 * Next half-edge around the face.
	 */
	HE_edge *next;
};

/**
 * Represents a vertex with a random attached
 * half-edge.
 */
struct HE_vert {
	/**
	 * A vector pointing
	 * to the coordinates of the vertex.
	 */
	vector *vec;
	/**
	 * One of the half-edges emanating from the vertex.
	 * It is made sure that this is never a border edge,
	 * unless there are only border edges.
	 */
	HE_edge *edge;
	/**
	 * Color of the vertex.
	 */
	color *col;
	/**
	 * The acceleration structure, used to speed up
	 * assembling the half-edge structures.
	 */
	HE_vert_acc *acc;
};

/**
 * Structure only needed for various
 * acceleration techniques during assembling of the
 * half-edge.
 */
struct HE_vert_acc {
	/**
	 * Array that saves all potential
	 * pair edges that point TO this vertex. It is used
	 * for finding the pairs when assembling
	 * the HE_edge struct. Note that this does not reliably
	 * save all border edges (which is a non-issue for the
	 * specific purpose of this struct since border-edges
	 * don't qualify for pairs anyway).
	 */
	HE_edge **edge_array;
	/**
	 * Similar as the edge_array acceleration structure,
	 * except that it is used for connecting the
	 * dummy edges.
	 */
	HE_edge **dummys;
	/**
	 * Element count of the edge_array.
	 */
	uint32_t eac;
	/**
	 * Element count of dummys.
	 */
	uint32_t dc;
};

/**
 * Represents a face
 */
struct HE_face {
	/**
	 * One of the half-edges bordering the face.
	 */
	HE_edge *edge;
};

/**
 * Represents a collection of HE_edge, HE_vert and HE_face
 * which are all attached to one object. This is useful
 * for algorithms that need to iterate over one or another.
 */
struct HE_obj {
	/**
	 * Array of edges.
	 */
	HE_edge *edges;
	/**
	 * Array of vertices.
	 */
	HE_vert *vertices;
	/**
	 * Array of faces.
	 */
	HE_face *faces;
	/**
	 * Count of edges.
	 */
	uint32_t ec;
	/**
	 * Count of vertices.
	 */
	uint32_t vc;
	/**
	 * Count of faces.
	 */
	uint32_t fc;
	/**
	 * Count of the vertex texture coordinates.
	 */
	uint32_t vtc;
	/**
	 * Count of dummy edges.
	 */
	uint32_t dec;
};

/**
 * Color.
 */
struct color {
	double red;
	double green;
	double blue;
};


bool face_normal(HE_edge const * const edge,
		vector *vec);
bool vec_normal(HE_vert const * const vert, vector *vec);
bool find_center(HE_obj const * const obj, vector *vec);
float get_normalized_scale_factor(HE_obj const * const obj);
bool normalize_object(HE_obj *obj);
HE_obj *parse_obj(char const * const filename);
void delete_object(HE_obj *obj);


#endif /* _DROW_ENGINE_HE_OPERATIONS_H */
