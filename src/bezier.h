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
 * @file bezier.h
 * Header for the external API of bezier.c.
 * @brief header of bezier.c
 */

#ifndef _DROW_ENGINE_BEZIER_H
#define _DROW_ENGINE_BEZIER_H


#include "vector.h"

#include <stdint.h>


typedef struct bez_curv bez_curv;


/**
 * Bezier Curve.
 */
struct bez_curv {
	/**
	 * Array of vectors.
	 */
	vector *vec;
	/**
	 * Degree of the curve.
	 */
	uint32_t deg;
};


bool get_reduced_bez_curv(const bez_curv *bez,
		bez_curv *new_bez,
		const float section);
vector *calculate_bezier_point(const bez_curv *bez,
		const float section);

#endif /* _DROW_ENGINE_BEZIER_H */
