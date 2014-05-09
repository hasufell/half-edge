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
#include "parser.h"
#include "types.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


/**
 * Read an obj file and return a HE_obj
 * if parsing worked.
 *
 * @param filename file to open
 * @return the HE_obj or NULL for failure
 */
HE_obj *read_obj_file(char const * const filename)
{
	char *string = NULL; /* file content */
	HE_obj *obj = NULL;

	/* read the whole file into string */
	string = read_file(filename);
	if (!filename || !*filename || !string || !*string)
		return NULL;

	obj = parse_obj(string);
	free(string);
	return obj;
}

/**
 * Reads a file and returns a newly allocated string.
 *
 * @param filename file to open
 * @return newly allocated string, must be freed by the caller
 */
char *read_file(char const * const filename)
{
	char buf[STD_FILE_BUF],
		 *string = NULL;
	int objfile = 0;
	size_t str_size = 0;
	ssize_t n;

	objfile = open(filename, O_RDONLY);

	if (objfile != -1) {
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
