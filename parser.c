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


/**
 * Parse an .obj file and return a NULL terminated
 * HE_face array that represents the object.
 *
 * @param filename .obj file
 * @return the HE_face array that represents the object
 */
HE_face *parse_obj(char const * const filename)
{
	unsigned int vc, fc;
	char *string = NULL,
		 *str_ptr_space = NULL,
		 *str_ptr_newline = NULL,
		 *str_tmp_ptr = NULL;
	const size_t vert_size = sizeof(HE_vert);
	HE_vert *vertices = malloc(vert_size);

	/* read the whole file into string */
	string = read_file(filename);
	if (!filename || !*filename || !string || !*string)
		return NULL;

	printf("file content\n%s\n\n", string);

	vc = 1;
	str_tmp_ptr = strtok_r(string, "\n", &str_ptr_newline);
	while (str_tmp_ptr && *str_tmp_ptr) {

		str_tmp_ptr = strtok_r(str_tmp_ptr, " ", &str_ptr_space);

		if (!strcmp(str_tmp_ptr, "v")) { /* parse vertices */
			char *myfloat = NULL;
			HE_vert *tmp_ptr;

			/* fill x */
			myfloat = strtok_r(NULL, " ", &str_ptr_space);
			CHECK_PTR_VAL(myfloat);
			vertices[vc - 1].x = atof(myfloat);

			/* fill y */
			myfloat = strtok_r(NULL, " ", &str_ptr_space);
			CHECK_PTR_VAL(myfloat);
			vertices[vc - 1].y = atof(myfloat);

			/* fill z */
			myfloat = strtok_r(NULL, " ", &str_ptr_space);
			CHECK_PTR_VAL(myfloat);
			vertices[vc - 1].z = atof(myfloat);

			vc++;
			tmp_ptr = realloc(vertices,
					vert_size * vc);
			CHECK_PTR_VAL(tmp_ptr);
			vertices = tmp_ptr;

			/* exceeds 3 dimensions, malformed vertice */
			if (strtok_r(NULL, " ", &str_ptr_space))
				return NULL;
		} else if (!strcmp(str_tmp_ptr, "f")) { /* parse faces */
			/* TODO */
		}

		str_tmp_ptr = strtok_r(NULL, "\n", &str_ptr_newline);
	}

	printf("vertices:\n");
	for (unsigned int i = 0; i < vc - 1; i++) {
		printf("x[%d]: %f\n", i, vertices[i].x);
		printf("y[%d]: %f\n", i, vertices[i].y);
		printf("z[%d]: %f\n", i, vertices[i].z);
		printf("\n");
	}

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

			tmp_ptr = realloc( /* allocate correct size */
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
