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

#include "gl_draw.h"
#include "print.h"
#include "types.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>

#define filename __FILE__


/**
 * Program help text.
 */
char const * const helptext = "Usage: drow-engine <file.obj>\n";


int main(int argc, char *argv[])
{
	int windowX;
	int windowY;
	int screenX;
	int screenY;
	float factor = 0.80; /* percent of screen */

	if (argc == 1) {
		printf("%s", helptext);
		return 1;
	}

	glutInit(&argc, argv);
	screenX = glutGet(GLUT_SCREEN_WIDTH);
	windowX = (screenX * factor);
	screenY = glutGet(GLUT_SCREEN_HEIGHT);
	windowY = (screenY * factor);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(((screenX / 2) - (windowX / 2)),
			((screenY / 2) - (windowY / 2)));
	glutInitWindowSize(windowX, windowY);
	glutCreateWindow(filename);

	init(argv[1]);

	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(animate);
	glutMainLoop();

	return 0;
}
