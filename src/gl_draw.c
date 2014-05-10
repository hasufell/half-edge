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
 * @file gl_draw.c
 * This file does the actual OpenGL and GLUT logic,
 * drawing the objects, handling keyboard input and
 * various GLUT-related callback functions.
 * @brief OpenGL drawing
 */

#include "err.h"
#include "filereader.h"
#include "half_edge.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <math.h>
#include <limits.h>
#include <unistd.h>

#include <stdio.h>

#define XY_WIRE_COUNT 10.0f

#define ROT_FACTOR_PLANET_SUN (360.0 / yearabs)
#define ROT_FACTOR_PLANET (360.0 / 1.0)
#define ROT_FACTOR_MOON (360.0 / dayabs)

#define SYSTEM_POS_Z -15.0f
#define SYSTEM_POS_Z_BACK 15.0f
#define VISIBILITY_FACTOR 5.0f

int year;
int yearabs = 365;
int day;
int dayabs = 30;
HE_obj *obj;

/*
 * static function declaration
 */
static void draw_obj(uint32_t xrot, uint32_t yrot, uint32_t zrot);
static void draw_Planet_1(void);
static void draw_Planet_2(void);
static void gl_destroy(void);
static HE_vert *find_center(HE_obj const * const obj);
static float get_normalized_scale_factor(HE_obj const * const obj);


/**
 * Calculates the factor that can be used to scaled down the object
 * to the size of 1.
 *
 * @param obj the object we want to scale
 * @return the corresponding scale factor
 */
static float get_normalized_scale_factor(HE_obj const * const obj)
{
	float max = obj->vertices[0].x +
		obj->vertices[0].y + obj->vertices[0].z;
	float min = obj->vertices[0].x +
		obj->vertices[0].y + obj->vertices[0].z;

	uint32_t i;

	for (i = 0; i < obj->vc; i++) {
		if ((obj->vertices[i].x +
				obj->vertices[i].y +
				obj->vertices[i].z) > max)
			max = obj->vertices[i].x +
				obj->vertices[i].y +
				obj->vertices[i].z;
		else if ((obj->vertices[i].x +
				obj->vertices[i].y +
				obj->vertices[i].z) < min)
			min = obj->vertices[i].x +
				obj->vertices[i].y +
				obj->vertices[i].z;
	}

	return 1 / (max - min);
}

/**
 * Find the center of an object and store the coordinates
 * in a HE_vert struct.
 *
 * @param obj the object we want to find the center of
 * @return newly allocated HE_vert with empty edge member
 * and coordinates which represent the middle of the object
 */
static HE_vert *find_center(HE_obj const * const obj)
{
	float x = 0,
		  y = 0,
		  z = 0;
	uint32_t i;
	HE_vert *newvert = malloc(sizeof(HE_vert));

	for (i = 0; i < obj->vc; i++) {
		x += obj->vertices[i].x;
		y += obj->vertices[i].y;
		z += obj->vertices[i].z;
	}

	newvert->x = x / i;
	newvert->y = y / i;
	newvert->z = z / i;
	newvert->edge = NULL;

	return newvert;
}

/**
 * Call glVertex3f on all of the vertices of the object,
 * in appropriate order.
 *
 * @param obj the object of which we will draw the vertices
 */
static void draw_vertices(HE_obj const * const obj)
{
	for (uint32_t i = 0; i < obj->fc; i++) { /* for all faces */
		HE_edge *tmp_edge = obj->faces[i].edge;

		do { /* for all edges of the face */
			glVertex3f(tmp_edge->vert->x,
					tmp_edge->vert->y,
					tmp_edge->vert->z);
		} while ((tmp_edge = tmp_edge->next) != obj->faces[i].edge);
	}
}

/**
 * Draws an object.
 *
 * @param myxrot rotation increment around x-axis
 * @param myyrot rotation increment around x-axis
 * @param myzrot rotation increment around x-axis
 */
static void draw_obj(uint32_t const myxrot,
		const uint32_t myyrot,
		const uint32_t myzrot)
{
	static uint32_t xrot = 0,
					yrot = 0,
					zrot = 0;
	HE_vert *center_vert = find_center(obj);
	float scalefactor = get_normalized_scale_factor(obj) * VISIBILITY_FACTOR;

	xrot += myxrot;
	yrot += myyrot;
	zrot += myzrot;

	glPushMatrix();

	/* rotate according to static members */
	glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z);
	glScalef(scalefactor,
			scalefactor,
			scalefactor);
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z_BACK);

	/* pull into middle of universe */
	glTranslatef(-center_vert->x,
			-center_vert->y,
			-center_vert->z + SYSTEM_POS_Z);

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 1.0f, 0.0f);
	draw_vertices(obj);
	glEnd();

	glPopMatrix();

	free(center_vert);
}

/**
 * Draws a planet with some Saturn like rings, two moons and an axis.
 * Rotates the planet around the sun, the moons around the planet
 * and the planet around its axis.
 */
static void draw_Planet_1(void)
{
	GLUquadric* quadric = NULL;
	quadric = gluNewQuadric();
	const int rot_fac_day = 15;

	glPushMatrix();

	/* Rotate around the sun */
	glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z);
	glRotatef(90, 1.0f, 0.0f, 0.0f);
	glRotatef((ROT_FACTOR_PLANET_SUN * day), 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 4.0f, 0.0f);
	glRotatef((ROT_FACTOR_PLANET_SUN * day), 0.0f, 0.0f, -1.0f);
	glRotatef(315, 0.0f, 1.0f, 0.0f);

	glColor3f(1.0f, 0.0f, 0.0f);

	/* A rotation (full 360°) once a day is much
	 * too fast you woulden'd see a thing */
	glRotatef((ROT_FACTOR_PLANET * day) / rot_fac_day, 0.0f, 0.0f, 1.0f);
	glutWireSphere(1.0f, XY_WIRE_COUNT, XY_WIRE_COUNT);
	glRotatef((ROT_FACTOR_PLANET * day) / rot_fac_day, 0.0f, 0.0f, -1.0f);

	/* Center axis */
	glPushMatrix();
	glLineWidth(3);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
	glVertex3f(0, 0, -5);
	glVertex3f(0, 0, 5);
	glEnd();
	glPopMatrix();

	/* circle1 */
	glPushMatrix();
	glColor3f(0.8f, 0.0f, 0.2f);
	/* glRotatef(90, 0.0f, 1.0f, 0.0f); [> "senkrecht zur Planetenachse" <] */
	gluDisk(quadric, 1.2f, 1.3f, 32, 1);
	glPopMatrix();

	/* circle2 */
	glPushMatrix();
	glColor3f(0.0f, 1.0f, 0.0f);
	/* glRotatef(90, 0.0f, 1.0f, 0.0f); [> "senkrecht zur Planetenachse" <] */
	gluDisk(quadric, 1.4f, 1.7f, 32, 1);
	glPopMatrix();

	/* Moon1 */
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);
	/* glRotatef((ROT_FACTOR_MOON * day), 1.0f, 0.0f, 0.0f); [> "senkrecht zur Planetenachse" <] */
	glRotatef((ROT_FACTOR_MOON * day), 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 2.0f, 0.0f);
	glutWireSphere(0.1f, XY_WIRE_COUNT, XY_WIRE_COUNT);
	glPopMatrix();

	/* Moon2 */
	glPushMatrix();
	glColor3f(0.0f, 1.0f, 1.0f);
	/* glRotatef((ROT_FACTOR_MOON * day), 1.0f, 0.0f, 0.0f); [> "senkrecht zur Planetenachse" <] */
	glRotatef((ROT_FACTOR_MOON * day), 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, -2.0f, 0.0f);
	glutWireSphere(0.1f, XY_WIRE_COUNT, XY_WIRE_COUNT);
	glPopMatrix();

	glPopMatrix();

}

/**
 * Draws a Planet with three moons.
 * The planet rotates around the sun and the moons around the planet
 * and the planet around its axis.
 */
static void draw_Planet_2(void)
{
	glPushMatrix();
	const float moon_pos_fac = 2.5;

	/* Rotate around the sun */
	glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z);
	glRotatef(90, 1.0f, 0.0f, 0.0f);
	glRotatef((ROT_FACTOR_PLANET_SUN * day), 0.0f, 0.0f, 1.0f);
	glTranslatef(-2.0f, -8.0f, 0.0f);

	glColor3f(0.0f, 0.0f, 1.0f);

	/* A rotation (full 360°) once a day is much
	 * too fast you woulden'd see a thing */
	const int rot_fac_day = 15;
	glRotatef((ROT_FACTOR_PLANET * day) / rot_fac_day, 0.0f, 0.0f, 1.0f);
	glutWireSphere(1.3f, XY_WIRE_COUNT, XY_WIRE_COUNT);
	glRotatef((ROT_FACTOR_PLANET * day) / rot_fac_day, 0.0f, 0.0f, -1.0f);

	/* Moon3 */
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glRotatef((ROT_FACTOR_MOON * day), 0.0f, 0.0f, 1.0f);
	glTranslatef(cos(0 * (M_PI / 180)) * moon_pos_fac,
			sin(0 * (M_PI / 180)) * moon_pos_fac, 0.0f);
	glutWireSphere(0.1f, XY_WIRE_COUNT, XY_WIRE_COUNT);
	glPopMatrix();

	/* Moon4 */
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 1.0f);
	glRotatef((ROT_FACTOR_MOON * day), 0.0f, 0.0f, 1.0f);
	glTranslatef(cos(120 * (M_PI / 180)) * moon_pos_fac,
			sin(120 * (M_PI / 180)) * moon_pos_fac, 0.0f);
	glutWireSphere(0.1f, XY_WIRE_COUNT, XY_WIRE_COUNT);
	glPopMatrix();

	/* Moon5 */
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glRotatef((ROT_FACTOR_MOON * day), 0.0f, 0.0f, 1.0f);
	glTranslatef(cos(240 * (M_PI / 180)) * moon_pos_fac,
			sin(240 * (M_PI / 180)) * moon_pos_fac, 0.0f);
	glutWireSphere(0.1f, XY_WIRE_COUNT, XY_WIRE_COUNT);
	glPopMatrix();

	glPopMatrix();
}

/**
 * Displays the whole setup with the sun, planet one,
 * planet two and the frame rate
 */
void display(void)
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	draw_obj(0, 0, 0);
	draw_Planet_1();
	draw_Planet_2();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, 500, 0.0, 500);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2i(5, 10);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
	glutSwapBuffers();
}

/**
 * Sets the initial values to start the program.
 */
void init(char const * const filename)
{
	obj = read_obj_file(filename);
	if (!obj)
		ABORT("Failed to read object file \"%s\"!", filename);
	day = 0;
	year = 0;
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);

	/* vector *array = malloc(sizeof(vector) * obj->vc); */
	/* vector vec; */

	/* vec.x = obj->vertices->x; */
	/* vec.y = obj->vertices->y; */
	/* vec.z = obj->vertices->z; */

	/* for (uint32_t i = 0; i < obj->vc; i++) { */

	/* } */
}

/**
 * Is called when the window size changes.
 * Fits the viewport to the new dimension.
 *
 * @param w the new width of the window
 * @param h the new height of the window
 */
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat) w / (GLfloat) h, 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
}

/**
 * Is called over and over again.
 * Counts the years and days.
 */
void animate()
{
	day++;
	if (day >= yearabs) {
		day = 0;
		year++;
	}
	if (year >= (INT_MAX - 1000) || year < 0) {
		year = 0;
	}
	if (day < 0) {
		day = 0;
	}

	/* cout << day << endl; */
	usleep(30000); // some very basic VScny to see the movement better
	glutPostRedisplay();
}

/**
 * Keyboard callback function,
 *
 * press t to increase the day
 * press T to decrease the day
 * press j to increase the year
 * press J to decrease the year
 * press x to rotate the middle object in x direction
 * press y to rotate the middle object in y direction
 * press c to rotate the middle object in z direction
 * press w to translate the whole scene in y direction
 * press s to translate the whole scene in -y direction
 * press a to translate the whole scene in -x direction
 * press d to translate the whole scene in x direction
 * press q to translate the whole scene in z direction
 * press e to translate the whole scene in -z direction
 *
 * @param key which was pressed
 * @param x coordinate
 * @param y coordinate
 */
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 't':
		dayabs += 15;
		glutPostRedisplay();
		break;
	case 'j':
		yearabs += 50;
		glutPostRedisplay();
		break;
	case 'T':
		dayabs -= 15;
		glutPostRedisplay();
		break;
	case 'J':
		yearabs -= 50;
		glutPostRedisplay();
		break;
	case 'x':
		draw_obj(1, 0, 0);
		glutPostRedisplay();
		break;
	case 'y':
		draw_obj(0, 1, 0);
		glutPostRedisplay();
		break;
	case 'c':
		draw_obj(0, 0, 1);
		glutPostRedisplay();
		break;
	case 'w':
		glTranslatef(0.0f, 1.0f, 0.0f);
		break;
	case 'a':
		glTranslatef(-1.0f, 0.0f, 0.0f);
		break;
	case 's':
		glTranslatef(0.0f, -1.0f, 0.0f);
		break;
	case 'd':
		glTranslatef(1.0f, 0.0f, 0.0f);
		break;
	case '+':
		glTranslatef(0.0f, 0.0f, 1.0f);
		break;
	case '-':
		glTranslatef(0.0f, 0.0f, -1.0f);
		break;
	case 'q':
		gl_destroy();
		break;
	}
}

/**
 * Destroy the gl session/window.
 */
static void gl_destroy(void)
{
	free(obj->edges);
	free(obj->vertices);
	free(obj->faces);
	free(obj);
	glutDestroyWindow(glutGetWindow());
}
