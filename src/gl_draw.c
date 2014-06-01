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

#include "bezier.h"
#include "err.h"
#include "filereader.h"
#include "gl_draw.h"
#include "half_edge.h"
#include "print.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <math.h>
#include <limits.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

#define XY_WIRE_COUNT 10.0f

#define FPS_OUT_SIZE 17

#define ROT_FACTOR_PLANET_SUN (360.0 / yearabs)
#define ROT_FACTOR_PLANET (360.0 / 1.0)
#define ROT_FACTOR_MOON (360.0 / dayabs)

#define SYSTEM_POS_Z -15.0f
#define SYSTEM_POS_Z_BACK 15.0f
#define VISIBILITY_FACTOR 5.0f
#define FAR_CLIPPING_PLANE 60.0f
#define NEAR_CLIPPING_PLANE 1.0f
#define CAMERA_ANGLE 60.0f


/*
 * globals
 */
int year;
int yearabs = 365;
int day;
int dayabs = 30;
HE_obj *obj;
bool show_normals = false;
bool shademodel = true;
bool draw_frame = false;
float ball_speed = 1.0f;


/*
 * static function declaration
 */
static void draw_bez(const bez_curv *bez, float step_factor_inc);
static void draw_bez_frame(const bez_curv *bez,
		float pos);
static void draw_ball(const bez_curv *bez,
		const float pos);
static void draw_obj(int32_t const myxrot,
		int32_t const myyrot,
		int32_t const myzrot,
		float bez_inc);
static void draw_Planet_1(void);
static void draw_Planet_2(void);
static void gl_destroy(void);
static void draw_normals(HE_obj const * const obj,
		float const scale_inc);
static void draw_vertices(HE_obj const * const obj,
		bool disco);
static float calculateFPS();


/**
 * Draws the vertex normals of the object.
 *
 * @param obj the object to draw the vertex normals of
 * @param scale_inc the incrementor for scaling the normals
 */
static void draw_normals(HE_obj const * const obj,
		float const scale_inc)
{
	static float normals_scale_factor = 0.1f;
	static float line_width = 2;
	vector vec;

	normals_scale_factor += scale_inc;

	glPushMatrix();

	glLineWidth(line_width);
	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINES);
	for (uint32_t i = 0; i < obj->vc; i++) {
		/* be fault tolerant here, so we don't just
		 * kill the whole thing, because the normals failed to draw */
		if (!vec_normal(&(obj->vertices[i]), &vec))
			break;

		glVertex3f(obj->vertices[i].vec->x,
				obj->vertices[i].vec->y,
				obj->vertices[i].vec->z);
		glVertex3f(obj->vertices[i].vec->x + (vec.x * normals_scale_factor),
				obj->vertices[i].vec->y + (vec.y * normals_scale_factor),
				obj->vertices[i].vec->z + (vec.z * normals_scale_factor));
	}
	glEnd();
	glPopMatrix();
}

/**
 * Draws all vertices of the object by
 * assembling a polygon for each face.
 *
 * @param obj the object of which we will draw the vertices
 * @param disco_set determines whether we are in disco mode
 */
static void draw_vertices(HE_obj const * const obj,
		bool disco_set)
{
	/* color */
	static float red = 90,
				 blue = 90,
				 green = 90;
	static bool disco = false;

	if (disco_set)
		disco = !disco;

	glPushMatrix();

	for (uint32_t i = 0; i < obj->fc; i++) { /* for all faces */
		HE_edge *tmp_edge = obj->faces[i].edge;

		/* add random values */
		red += rand() / (RAND_MAX / ((rand() % 11) / 10.0f));
		blue += rand() / (RAND_MAX / ((rand() % 11) / 10.0f));
		green += rand() / (RAND_MAX / ((rand() % 11) / 10.0f));

		/* values above 180 will cause negative color values */
		red = fmodf(red, 181.0f);
		blue = fmodf(blue, 181.0f);
		green = fmodf(green, 181.0f);

		if (disco) {
			tmp_edge->vert->col->red =
				(sin(red * i * (M_PI / 180)) / 2) + 0.5;
			tmp_edge->vert->col->green =
				(sin(green * i * (M_PI / 180)) / 2) + 0.5;
			tmp_edge->vert->col->blue =
				(sin(blue * i * (M_PI / 180)) / 2) + 0.5;
		} else {
			if (tmp_edge->vert->col->red == -1)
				tmp_edge->vert->col->red =
					(sin(red * i * (M_PI / 180)) / 2) + 0.5;

			if (tmp_edge->vert->col->green == -1)
				tmp_edge->vert->col->green =
					(sin(green * i * (M_PI / 180)) / 2) + 0.5;

			if (tmp_edge->vert->col->blue == -1)
				tmp_edge->vert->col->blue =
					(sin(blue * i * (M_PI / 180)) / 2) + 0.5;
		}

		glBegin(GL_POLYGON);
		do { /* for all edges of the face */
			glColor3f(tmp_edge->vert->col->red,
					tmp_edge->vert->col->green,
					tmp_edge->vert->col->blue);

			glVertex3f(tmp_edge->vert->vec->x,
					tmp_edge->vert->vec->y,
					tmp_edge->vert->vec->z);

		} while ((tmp_edge = tmp_edge->next) != obj->faces[i].edge);
		glEnd();
	}
	glPopMatrix();
}

/**
 * Draw the bezier curve.
 *
 * @param bez the bezier curve to draw
 * @param step_factor_inc the step factor between calculated control points
 */
static void draw_bez(const bez_curv *bez, float step_factor_inc)
{
	static float line_width = 2;
	static float point_size = 10;
	static float step_factor = 0.1;

	if (step_factor + step_factor_inc > 0.002 &
			step_factor + step_factor_inc < 0.50)
		step_factor += step_factor_inc;

	glPushMatrix();

	glLineWidth(line_width);
	glPointSize(point_size);
	glColor3f(1.0, 0.0, 0.0);

	vector *v1 = NULL,
		   *v2 = NULL;

	/*
	 * draw frame
	 */
	glBegin(GL_LINE_STRIP);
	for (uint32_t j = 0; j <= bez->deg; j++) {
		glVertex3f(bez->vec[j].x,
			bez->vec[j].y,
			bez->vec[j].z);
	}
	glEnd();

	/*
	 * draw control points
	 */
	glBegin(GL_POINTS);
	for (uint32_t j = 0; j <= bez->deg; j++) {
		glVertex3f(bez->vec[j].x,
			bez->vec[j].y,
			bez->vec[j].z);
	}
	glEnd();

	glBegin(GL_LINES);

	/*
	 * line segments: first line
	 */
	v1 = calculate_bezier_point(bez, step_factor);
	glVertex3f(bez->vec[0].x,
			bez->vec[0].y,
			bez->vec[0].z);
	glVertex3f(v1->x,
			v1->y,
			v1->z);

	for (float k = step_factor; k < 1 - step_factor; k += step_factor) {
		free(v1);
		free(v2);

		v1 = calculate_bezier_point(bez, k);
		v2 = calculate_bezier_point(bez, k +
				step_factor);

		/*
		 * line segments: middle lines
		 */
		glVertex3f(v1->x,
				v1->y,
				v1->z);
		glVertex3f(v2->x,
				v2->y,
				v2->z);

	}

	/*
	 * line segments: last line
	 */
	glVertex3f(v2->x,
			v2->y,
			v2->z);
	glVertex3f(bez->vec[bez->deg].x,
			bez->vec[bez->deg].y,
			bez->vec[bez->deg].z);

		free(v1);
		free(v2);

		glEnd();


	glPopMatrix();
}

/**
 * Draw the bezier frame of the given bezier curve
 * which will cut the curve at the given position.
 *
 * @param bez the bezier curve to draw the frame of
 * @param pos the position where the curve and the frame
 * will cut
 */
static void draw_bez_frame(const bez_curv *bez,
		float pos)
{
	bez_curv cur_bez = *bez;
	bez_curv next_bez = { NULL, 0 };

	while ((get_reduced_bez_curv(&cur_bez, &next_bez, pos))) {

		glBegin(GL_LINES);

		for (uint32_t j = 0; j < next_bez.deg; j++) {
			glVertex3f(next_bez.vec[j].x,
					next_bez.vec[j].y,
					next_bez.vec[j].z);
			glVertex3f(next_bez.vec[j + 1].x,
					next_bez.vec[j + 1].y,
					next_bez.vec[j + 1].z);
		}

		/* don't free the original one! */
		if (cur_bez.deg < bez->deg)
			free(cur_bez.vec);
		cur_bez = next_bez;

		glEnd();
	}
	free(cur_bez.vec);

}

/**
 * Draws a ball on the bezier curve at the given position.
 *
 * @param bez the bezier curve to draw the ball on
 * @param pos the position of the ball
 */
static void draw_ball(const bez_curv *bez,
		const float pos)
{
	const float ball_pos = pos;

	glPushMatrix();
	glColor3f(0.0, 1.0, 0.0);
	vector *point = calculate_bezier_point(bez,
			ball_pos);
	glTranslatef(point->x, point->y, point->z);
	glutWireSphere(0.02f, 100, 100);
	glPopMatrix();
}


/**
 * Draws an object.
 *
 * @param myxrot rotation increment around x-axis
 * @param myyrot rotation increment around x-axis
 * @param myzrot rotation increment around x-axis
 * @param bez_inc the step factor between calculated control points
 * for the bezier curve
 */
static void draw_obj(int32_t const myxrot,
		int32_t const myyrot,
		int32_t const myzrot,
		float bez_inc)
{
	/* rotation */
	static int32_t xrot = 0,
					yrot = 0,
					zrot = 0;
	static float ball_inc = 0;
	static bool ball_to_right = true;

	if (ball_inc > 0.98)
		ball_to_right = false;
	else if (ball_inc < 0.02)
		ball_to_right = true;

	if (ball_to_right)
		ball_inc += 0.01f * ball_speed;
	else
		ball_inc -= 0.01f * ball_speed;

	vector center_vert;

	FIND_CENTER(obj, &center_vert);

	/* increment rotation, if any */
	xrot += myxrot;
	yrot += myyrot;
	zrot += myzrot;

	glPushMatrix();

	/* rotate according to static members */
	glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z);
	glScalef(VISIBILITY_FACTOR,
			VISIBILITY_FACTOR,
			VISIBILITY_FACTOR);
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z_BACK);

	/* pull into middle of universe */
	glTranslatef(-center_vert.x,
			-center_vert.y,
			-center_vert.z + SYSTEM_POS_Z);

	if (obj->ec != 0) {
		if (show_normals)
			draw_normals(obj, 0);

		draw_vertices(obj, false);
	}

	if (obj->bzc != 0) {
		draw_bez(&(obj->bez_curves[0]), bez_inc);
		draw_ball(&(obj->bez_curves[0]), ball_inc);
		if(draw_frame)
			draw_bez_frame(&(obj->bez_curves[0]), ball_inc);
	}

	glPopMatrix();
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
	 * too fast you wouldn't see a thing */
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
 * Calculates the frames per second rate.
 *
 * @return the actual frames per second rate
 */
static float calculateFPS(void)
{
	static int frameCount = 0;
	static int currentTime = 0;
	static int previousTime = 0;
	static float fps = 0;

	frameCount++;
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	int timeInterval = currentTime - previousTime;
	if (timeInterval > 1000) {
		fps = frameCount / (timeInterval / 1000.0f);
		previousTime = currentTime;
		frameCount = 0;
	}
	return fps;
}

/**
 * Displays the whole setup with the sun, planet one,
 * planet two and the frame rate
 */
void display(void)
{
	char fps_out[FPS_OUT_SIZE];
	char *tmp_ptr = fps_out;
	float fps = calculateFPS();
	int i;

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	draw_obj(0, 0, 0, 0);
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

	sprintf(fps_out, "FPS: %f", fps);

	for (i = 0; i < FPS_OUT_SIZE; ++i)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *tmp_ptr++);

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

	NORMALIZE_OBJECT(obj);

	day = 0;
	year = 0;
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
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
	gluPerspective(CAMERA_ANGLE,
			(GLfloat) w / (GLfloat) h,
			NEAR_CLIPPING_PLANE,
			FAR_CLIPPING_PLANE);
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

	glutTimerFunc(TIMERMSECS, animate, 0);
	glutPostRedisplay();
}

/**
 * Keyboard callback function,
 *
 * press t to increase the day
 *
 * press T to decrease the day
 *
 * press j to increase the year
 *
 * press J to decrease the year
 *
 * press b to toggle GL_CULL_FACE
 *
 * press D to toggle disco mode
 *
 * press S to toggle shade model between GL_SMOOTH and GL_FLAT
 *
 * press n to toggle normals
 *
 * press k to increase length of normals
 *
 * press l to decrease length of normals
 *
 * press x to rotate the middle object in x direction
 *
 * press X to rotate the middle object in -x direction
 *
 * press y to rotate the middle object in y direction
 *
 * press Y to rotate the middle object in -y direction
 *
 * press c to rotate the middle object in z direction
 *
 * press C to rotate the middle object in -z direction
 *
 * press w to translate the whole scene in y direction
 *
 * press s to translate the whole scene in -y direction
 *
 * press a to translate the whole scene in -x direction
 *
 * press d to translate the whole scene in x direction
 *
 * press q to quit
 *
 * @param key which was pressed
 * @param x coordinate
 * @param y coordinate
 */
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	/* case 'b': */
		/* if (glIsEnabled(GL_CULL_FACE)) */
			/* glDisable(GL_CULL_FACE); */
		/* else */
			/* glEnable(GL_CULL_FACE); */
		/* glutPostRedisplay(); */
		/* break; */
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
		draw_obj(2, 0, 0, 0);
		glutPostRedisplay();
		break;
	case 'X':
		draw_obj(-2, 0, 0, 0);
		glutPostRedisplay();
		break;
	case 'y':
		draw_obj(0, 2, 0, 0);
		glutPostRedisplay();
		break;
	case 'Y':
		draw_obj(0, -2, 0, 0);
		glutPostRedisplay();
		break;
	case 'c':
		draw_obj(0, 0, 2, 0);
		glutPostRedisplay();
		break;
	case 'C':
		draw_obj(0, 0, -2, 0);
		glutPostRedisplay();
		break;
	case 'D':
		draw_vertices(obj, true);
		glutPostRedisplay();
		break;
	case 'S':
		if (shademodel) {
			glShadeModel(GL_FLAT);
			shademodel = false;
		} else {
			glShadeModel(GL_SMOOTH);
			shademodel = true;
		}
		glutPostRedisplay();
		break;
	case 'b':
		draw_obj(0, 0, 0, 0.02);
		glutPostRedisplay();
		break;
	case 'B':
		draw_obj(0, 0, 0, -0.02);
		glutPostRedisplay();
		break;
	case 'k':
		ball_speed += 0.2f;
		glutPostRedisplay();
		break;
	case 'K':
		ball_speed -= 0.2f;
		glutPostRedisplay();
		break;
	case 'f':
		draw_frame = !draw_frame;
		glutPostRedisplay();
		break;

	/* case 'k': */
		/* draw_normals(obj, 0.01f); */
		/* glutPostRedisplay(); */
		/* break; */
	/* case 'l': */
		/* draw_normals(obj, -0.01f); */
		/* glutPostRedisplay(); */
		/* break; */
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
	case 'n':
		show_normals = !show_normals;
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
	delete_object(obj);
	free(obj);
	glutDestroyWindow(glutGetWindow());
}
