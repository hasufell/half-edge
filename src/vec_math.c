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

#include "err.h"
#include "types.h"
#include "vec_math.h"

#include <stdbool.h>
#include <stdlib.h>


/**
 * Calculate the vector product of a and b
 * and store it in c. This function is aliasing safe.
 *
 * @param a vector
 * @param b vector
 * @param c vector [out]
 * @return true/false for success/failure
 */
bool vector_product(vector *a, vector *b, vector *c)
{
	vector a_tmp,
		   b_tmp;

	if (!a || !b)
		return false;

	copy_vector(a, &a_tmp);
	copy_vector(b, &b_tmp);

	c->x = a_tmp.y * b_tmp.z - a_tmp.z * b_tmp.y;
	c->y = a_tmp.z * b_tmp.x - a_tmp.x * b_tmp.z;
	c->z = a_tmp.x * b_tmp.y - a_tmp.y * b_tmp.x;

	return true;
}

/**
 * Copy coordinates from vector a to vector b.
 *
 * @param a vector
 * @param b vector [out]
 * @return true/false for success/failure
 */
bool copy_vector(vector *a, vector *b)
{
	if (!a || !b)
		return false;

	b->x = a->x;
	b->y = a->y;
	b->z = a->z;

	return true;
}
