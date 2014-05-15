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
 * @file filereader.c
 * Reading of arbitrary files into strings as well as
 * reading specific ones via various parsers.
 * @brief reading of different filetypes
 */

#include "common.h"
#include "err.h"
#include "filereader.h"
#include "half_edge.h"

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

	if (!filename || !*filename)
		return NULL;

	/* read the whole file into string */
	string = read_file(filename);

	if (!string)
		return NULL;

	obj = parse_obj(string);
	free(string);
	return obj;
}

/**
 * Reads a file and returns a newly allocated string.
 *
 * @param filename file to open
 * @return a newly allocated string which must be freed by the caller
 * or NULL on failure
 */
char *read_file(char const * const filename)
{
	char buf[STD_FILE_BUF],
		 *string = NULL;
	int fd = 0;
	size_t str_size = 0;
	ssize_t n;

	fd = open(filename, O_RDONLY);

	if (fd != -1) {
		/* read and copy chunks */
		while ((n = read(fd, buf, STD_FILE_BUF)) != 0) {

			if (n == -1)
				ABORT("Failed while reading file descriptor %d\n", fd);

			str_size += n; /* count total bytes read */

			REALLOC( /* allocate correct size */
					string, /* pointer to realloc */
					str_size /* total bytes read */
					+ 1); /* space for trailing NULL byte */

			/* append buffer to string */
			memcpy(string + (str_size - n), buf, (size_t)n);
		}
		/* add trailing NULL byte */
		string[str_size] = '\0';

		if (close(fd))
			ABORT("Failed to close file descripter %d\n", fd);

		return string;
	} else {
		return NULL;
	}
}
