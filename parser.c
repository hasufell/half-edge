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
#include "types.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


/*
 * static function declarations
 */
static char *read_file(char const * const filename);
static void print_edges(HE_obj *obj);
static void print_vertices(HE_obj *obj);
static void print_faces(HE_obj *obj);
static void print_plain_faces(FACE face, uint8_t fc);


/**
 * Parse an .obj file and return a NULL terminated
 * HE_face array that represents the object.
 *
 * @param filename .obj file
 * @return the HE_face array that represents the object
 */
HE_obj *parse_obj(char const * const filename)
{
	uint32_t vc = 0, /* vertices count */
			 fc = 0, /* face count */
			 ec = 0; /* edge count */
	char *string = NULL, /* file content */
		 *str_ptr_space = NULL, /* for strtok */
		 *str_ptr_newline = NULL, /* for strtok */
		 *str_tmp_ptr = NULL; /* for strtok */
	HE_vert *vertices = NULL;
	HE_edge *edges = NULL;
	HE_face *faces = NULL;
	HE_obj *obj = NULL;
	FACE face_v = NULL;

	/* read the whole file into string */
	string = read_file(filename);
	if (!filename || !*filename || !string || !*string)
		return NULL;

	printf("file content\n%s\n\n", string);

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

	obj->vertices = vertices;
	obj->vc = vc;

	print_plain_faces(face_v, fc);
	print_vertices(obj);

	return NULL;
}

/**
 * Reads a file and returns a newly allocated string.
 *
 * @param filename file to open
 * @return newly allocated string, must be freed by the caller
 */
static char *read_file(char const * const filename)
{
	char buf[STD_FILE_BUF],
		 *string = NULL;
	int objfile = 0;
	size_t str_size = 0;
	ssize_t n;

	objfile = open(filename, O_RDONLY);

	if (objfile) {
		/* read and copy chunks */
		while ((n = read(objfile, buf, STD_FILE_BUF)) > 0) {
			char *tmp_ptr = NULL;

			str_size += n; /* count total bytes read */

			tmp_ptr = (char*) realloc( /* allocate correct size */
					string, /* pointer to realloc */
					str_size /* total bytes read */
					+ 1); /* space for trailing NULL byte */
			CHECK_PTR_VAL(tmp_ptr);
			string = tmp_ptr;

			/* append buffer to string */
			memcpy(string + (str_size - n), buf, n);
		}
		/* add trailing NULL byte */
		string[str_size] = '\0';

		close(objfile);

		return string;
	} else {
		return NULL;
	}
}

static void print_edges(HE_obj *obj)
{

}

static void print_vertices(HE_obj *obj)
{
	printf("vertices: %d\n", obj->vc);
	for (unsigned int i = 0; i < obj->vc; i++) {
		printf("x[%d]: %f\n", i, obj->vertices[i].x);
		printf("y[%d]: %f\n", i, obj->vertices[i].y);
		printf("z[%d]: %f\n", i, obj->vertices[i].z);
		printf("\n");
	}
}

static void print_faces(HE_obj *obj)
{

}

static void print_plain_faces(FACE face, uint8_t fc)
{
	printf("plain faces:\n");
	for (uint8_t i = 0; i < fc - 1; i++) {
		uint8_t j = 0;
		printf("f:");
		while (face[i][j]) {
			printf(" %d", face[i][j]);
			j++;
		}
		printf("\n");
	}
	printf("\n");
}
