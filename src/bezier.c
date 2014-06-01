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
 * static function declarations
 */
static bool get_section_vec(vector *a, vector *b, vector *c, float section);


static bool get_section_vec(vector *a, vector *b, vector *c, float section)
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
bool get_reduced_bez_curv(bez_curv *bez, bez_curv *new_bez, float section)
{
	vector *vec_arr = malloc(sizeof(*vec_arr) * bez->deg);

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

	return new_bez;
}

/**
 * Calculate a point on the bezier curve according to the
 * bezier vertices. If section is set to 0.5 then it will
 * return the vector to the point in the middle of the curve.
 *
 * @param bez the bezier curve the calcluate the point from
 * @param section the section which will be applied to all
 * lines between the bezier vertices
 * @return the vector to the calculated point, newly allocated
 */
vector *calculate_bezier_point(bez_curv *bez, float section)
{
	bez_curv new_bez = { NULL, 0 };

	free(new_bez.vec);

	if (!get_reduced_bez_curv(bez, &new_bez, section))
		return NULL;

	if (new_bez.deg > 0) {
		return calculate_bezier_point(&new_bez, section);
	} else {
		vector *result_vector = malloc(sizeof(*result_vector));
		*result_vector = new_bez.vec[0];

		return result_vector;
	}
}


