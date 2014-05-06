#include "err.h"
#include "types.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


/*
 * static function declarations
 */
static char *read_file(char *filename);


/**
 * Parse an .obj file and return a NULL terminated
 * HE_face array that represents the object.
 *
 * @param filename .obj file
 * @return the HE_face array that represents the object
 */
HE_face *parse_obj(char *filename)
{
	unsigned int vc;
	char *string,
		 *str_ptr_space,
		 *str_ptr_newline,
		 *str_tmp_ptr;
	const size_t vert_size = sizeof(HE_vert);
	HE_vert *vertices = malloc(vert_size);

	/* read the whole file into string */
	if (!filename || !*filename || !(string = read_file(filename)))
		return NULL;

	printf("file content\n%s\n\n", string);

	vc = 1;
	str_tmp_ptr = strtok_r(string, "\n", &str_ptr_newline);
	while (str_tmp_ptr && *str_tmp_ptr) {

		str_tmp_ptr = strtok_r(str_tmp_ptr, " ", &str_ptr_space);

		if (!strcmp(str_tmp_ptr, "v")) { /* parse vertices */
			char *myfloat = NULL;

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
			vertices = realloc(vertices,
					vert_size * vc);
			CHECK_PTR_VAL(vertices);
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
static char *read_file(char *filename)
{
	char buf[STD_FILE_BUF],
		 *string = malloc(STD_FILE_BUF);
	int objfile = 0;
	size_t str_size = 0;
	ssize_t n;
	uint8_t i = 0;

	objfile = open(filename, O_RDONLY);

	if (objfile) {
		/* read and copy chunks */
		while ((n = read(objfile, buf, STD_FILE_BUF)) > 0) {
			char *tmp_ptr = string + str_size;
			memcpy(tmp_ptr, buf, STD_FILE_BUF);
			str_size += n;
			string = realloc(string, str_size);
			i++;
		}
		/* add trailing NULL byte */
		string[str_size + 1] = '\0';

		close(objfile);

		return string;
	} else {
		return NULL;
	}
}
