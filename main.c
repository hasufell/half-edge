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

#include "filereader.h"
#include "print.h"
#include "types.h"

#include <stdio.h>

/**
 * Program help text.
 */
char const * const helptext = "Usage: drow-engine <file.obj>\n";


int main(int argc, char *argv[])
{
	HE_obj *obj;

	if (argc > 1) {
		obj = read_obj_file(argv[1]);
		print_vertices(obj);
		print_edges(obj);
		print_faces(obj);
	} else {
		printf("%s", helptext);
	}

	return 0;
}
