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
 * @file bezier.c
 * This file provides operations and informational functions
 * on the bezier data type which is defined in bezier.h.
 * @brief operations on bezier curves
 */

#include "bezier.h"
#include "vector.h"

#include <stdint.h>
#include <stdlib.h>


/*
 * static function declaration
 */
static bool get_section_vec(const vector *a,
		const vector *b,
		vector *c,
		const float section);
static vector *calculate_bezier_point_rec(const bez_curv *bez,
		const float section,
		const uint32_t deg);


/**
 * Get the vector that is in between the to control
 * points a and b and in the specified section from 0 to 1.
 *
 * @param a first control point
 * @param b second control point
 * @param c store the result here [out]
 * @param section the section between 0 and 1
 * @return true/false for success/failure
 */
static bool get_section_vec(const vector *a,
		const vector *b,
		vector *c,
		const float section)
{
	vector a_tmp,
		   b_tmp;

	if (!a || !b || !c)
		return false;

	copy_vector(a, &a_tmp);
	copy_vector(b, &b_tmp);

	set_null_vector(c);

	SUB_VECTORS(&a_tmp, &b_tmp, c);
	VECTOR_LEN_SCAL_MUL(c, section, c);
	ADD_VECTORS(c, b, c);

	return true;
}

/**
 * Calculate a point on the bezier curve according to the
 * bezier vertices. If section is set to 0.5 then it will
 * return the vector to the point in the middle of the curve.
 *
 * @param bez the bezier curve to calcluate the point from
 * @param section the section which will be applied to all
 * lines between the bezier vertices
 * @param deg the degree of the bezier curve, this needs to be passed
 * because the function is recursive
 * @return the vector to the calculated point, newly allocated
 */
static vector *calculate_bezier_point_rec(const bez_curv *bez,
		const float section,
		const uint32_t deg)
{
	bez_curv new_bez = { NULL, 0 };

	if (!get_reduced_bez_curv(bez, &new_bez, section))
		return NULL;

	if (bez->deg < deg)
		free(bez->vec);

	if (new_bez.deg > 0) {
		return calculate_bezier_point_rec(&new_bez, section, deg);
	} else {
		vector *result_vector = malloc(sizeof(*result_vector));
		*result_vector = new_bez.vec[0];
		free(new_bez.vec);

		return result_vector;
	}
}

/**
 * Calculate a point on the bezier curve according to the
 * bezier vertices. If section is set to 0.5 then it will
 * return the vector to the point in the middle of the curve.
 *
 * @param bez the bezier curve to calcluate the point from
 * @param section the section which will be applied to all
 * lines between the bezier vertices
 * @return the vector to the calculated point, newly allocated
 */
vector *calculate_bezier_point(const bez_curv *bez,
		const float section)
{
	return calculate_bezier_point_rec(bez, section, bez->deg);
}

/**
 * Get the reduced bezier curve which is of one degree less
 * and strained between the defined sections of the old curve.
 *
 * @param bez the bezier curve the calcluate the next from
 * @param new_bez the new bezier curve, with a newly
 * allocated vector member [out]
 * @param section the section which will be applied to all
 * lines between the bezier vertices
 * @return true/false for success/failure
 */
bool get_reduced_bez_curv(const bez_curv *bez,
		bez_curv *new_bez,
		const float section)
{
	vector *vec_arr;

	if (bez->deg < 1)
		return false;

	vec_arr = malloc(sizeof(*vec_arr) * bez->deg);
	for (uint32_t i = 0; i < bez->deg; i++) {
		vector new_vec;

		get_section_vec(&(bez->vec[i + 1]),
				&(bez->vec[i]),
				&new_vec,
				section);

		vec_arr[i] = new_vec;
	}

	new_bez->vec = vec_arr;
	new_bez->deg = bez->deg - 1;

	return true;
}
