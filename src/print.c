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

#include "types.h"

#include <stdio.h>


/**
 * Print the coordinates of the starting vertices
 * of all edges.
 *
 * @param obj the 3d object
 */
void print_edges(HE_obj *obj)
{
	for (uint32_t i = 0; i < obj->ec; i++) {
		printf("edge vertices %i:\n", i);
		printf("  x: %f\n", obj->edges[i].vert->x);
		printf("  y: %f\n", obj->edges[i].vert->y);
		printf("  z: %f\n", obj->edges[i].vert->z);
		printf("\n");
	}
}

/**
 * Print the coordinates of all vertices.
 *
 * @param obj the 3d object
 */
void print_vertices(HE_obj *obj)
{
	printf("vertices: %d\n", obj->vc);
	for (uint32_t i = 0; i < obj->vc; i++) {
		printf("x[%d]: %f\n", i, obj->vertices[i].x);
		printf("y[%d]: %f\n", i, obj->vertices[i].y);
		printf("z[%d]: %f\n", i, obj->vertices[i].z);
		printf("\n");
	}
}

/**
 * Print the coordinates of all edges
 * that are saved in the HE_face structs.
 *
 * @param obj the 3d object
 */
void print_faces(HE_obj *obj)
{
	for (uint32_t i = 0; i < obj->fc; i++) {
		printf("face edge vertice %i:\n", i);
		printf("  x: %f\n", obj->faces[i].edge->vert->x);
		printf("  y: %f\n", obj->faces[i].edge->vert->y);
		printf("  z: %f\n", obj->faces[i].edge->vert->z);
		printf("\n");
	}
}

/**
 * Print all plain unconverted faces
 * as they are saved in the .obj file.
 *
 * @param face the plain face struct
 * @param fc the count of faces
 */
void print_plain_faces(FACE face, uint32_t fc)
{
	printf("plain faces:\n");
	for (uint32_t i = 0; i < fc - 1; i++) {
		uint32_t j = 0;
		printf("f:");
		while (face[i][j]) {
			printf(" %d", face[i][j]);
			j++;
		}
		printf("\n");
	}
	printf("\n");
}

/**
 * Print all coordinates of a vector.
 *
 * @param the vector we want to print
 */
void print_vector(vector *vec)
{
	printf("vector:\n"
			"x %f\n"
			"y %f\n"
			"z %f\n\n",
			vec->x,
			vec->y,
			vec->z);
}
