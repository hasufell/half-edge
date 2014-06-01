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


/**
 * Calculate a point on the bezier curve according to the
 * bezier vertices. If section is set to 0.5 then it will
 * return the vector to the point in the middle of the curve.
 *
 * @param obj the object holding the bezier vertices information
 * @param section the section which will be applied to all
 * lines between the bezier vertices
 * @return the vector to the calculated point
 */
vector *calculate_bezier_point(bez_curv *bez, float section)
{
	vector vec_arr[bez->deg];
	bez_curv new_bez;

	for (uint32_t i = 0; i < bez->deg; i++) {
		vector new_vec;

		SUB_VECTORS(&(bez->vec[i + 1]), &(bez->vec[i]), &new_vec);
		VECTOR_LEN_SCAL_MUL(&new_vec, section, &new_vec);
		ADD_VECTORS(&new_vec, &(bez->vec[i]), &new_vec);

		vec_arr[i] = new_vec;
	}

	new_bez.vec = vec_arr;
	new_bez.deg = bez->deg - 1;

	if (new_bez.deg > 0) {
		return calculate_bezier_point(&new_bez, section);
	} else {
		vector *result_vector = malloc(sizeof(*result_vector));
		*result_vector = vec_arr[0];
		return result_vector;
	}
}


