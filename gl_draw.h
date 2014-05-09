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

#ifndef _DROW_ENGINE_DRAW_H
#define _DROW_ENGINE_DRAW_H


#include "types.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>


void init(HE_obj *myobj);
void display(void);
void reshape(GLsizei w, GLsizei h);
void animate();
void keyboard(unsigned char key, int x, int y);


#endif /* _DROW_ENGINE_DRAW_H */