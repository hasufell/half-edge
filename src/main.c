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
 * @file main.c
 * Takes argv[1] as parameter and passes
 * it to the glut functions for drawing the file
 * in a predefined scene.
 * @brief program entry point
 */

#include "gl_setup.h"
#include "half_edge.h"
#include "print.h"
#include "vector.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>

#include <SDL.h>


/**
 * Program help text.
 */
char const * const helptext = "Usage: drow-engine <center.obj>"
" <float.obj> <bez.obj>\n";


int main(int argc, char *argv[])
{
	if (argc != 4) {
		printf("%s", helptext);
		return 1;
	}

	glutInit(&argc, argv);

	init_object(argv[1], argv[2], argv[3]);
	init_sdl_loop();

	return 0;
}
