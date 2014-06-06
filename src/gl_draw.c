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

#include <SDL.h>

#include <math.h>
#include <limits.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>


/*
 * globals
 */
int year = 0;
int yearabs = 365;
int day = 0;
int dayabs = 30;
HE_obj *obj;
bool show_normals = false;
bool shademodel = true;
bool draw_frame = false;
float ball_speed = 1.0f;



/**
 * Draws the vertex normals of the object.
 *
 * @param obj the object to draw the vertex normals of
 * @param scale_inc the incrementor for scaling the normals
 */
void draw_normals(HE_obj const * const obj,
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
void draw_vertices(HE_obj const * const obj,
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
void draw_bez(const bez_curv *bez, float step_factor_inc)
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
void draw_bez_frame(const bez_curv *bez,
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
void draw_ball(const bez_curv *bez,
		const float pos)
{
	const float ball_pos = pos;
	vector *point;

	glPushMatrix();
	glColor3f(0.0, 1.0, 0.0);
	point = calculate_bezier_point(bez,
			ball_pos);
	glTranslatef(point->x, point->y, point->z);
	glutWireSphere(0.02f, 100, 100);
	glPopMatrix();

	free(point);
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
void draw_obj(int32_t const myxrot,
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
void draw_Planet_1(void)
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
void draw_Planet_2(void)
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
void draw_scene(void)
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

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}

