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

#ifndef _DROW_ENGINE_HE_OPERATIONS_H
#define _DROW_ENGINE_HE_OPERATIONS_H


#include <stdint.h>


typedef struct HE_edge HE_edge;
typedef struct HE_vert HE_vert;
typedef struct HE_face HE_face;
typedef struct HE_obj HE_obj;


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
 * hald-edge.
 */
struct HE_vert {
	float x;
	float y;
	float z;

	/**
	 * One of the half-edges emanating from the vertex.
	 */
	HE_edge *edge;
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
};


HE_edge **get_all_emanating_edges(HE_vert *vertice);


#endif /* _DROW_ENGINE_HE_OPERATIONS_H */
