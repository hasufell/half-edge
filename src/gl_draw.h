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
 * @file gl_draw.h
 * Header for the external API of gl_draw.c
 * @brief header of gl_draw.c
 */

#ifndef _DROW_ENGINE_DRAW_H
#define _DROW_ENGINE_DRAW_H


#include "bezier.h"
#include "half_edge.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <SDL.h>

#include <stdbool.h>


/**
 * Vsync timer for animate() function
 */
#define TIMERMSECS 25

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


extern int yearabs;
extern int dayabs;
extern HE_obj *obj;
extern HE_obj *float_obj;
extern HE_obj *bez_obj;
extern bool show_normals;
extern bool shademodel;
extern bool draw_frame;
extern bool draw_bezier;
extern float ball_speed;


void draw_normals(HE_obj const * const obj,
		float const scale_inc);
void draw_vertices(HE_obj const * const obj,
		bool disco_set);
void draw_bez(const bez_curv *bez, float step_factor_inc);
void draw_bez_frame(const bez_curv *bez,
		float pos);
void draw_ball(const bez_curv *bez,
		const float pos);
void draw_obj(int32_t const myxrot,
		int32_t const myyrot,
		int32_t const myzrot,
		float bez_inc);
void draw_Planet_1(void);
void draw_Planet_2(void);
void draw_scene(void);


#endif /* _DROW_ENGINE_DRAW_H */
