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

#include <stdbool.h>
#include <stdlib.h>


/**
 * Calculate the vector product of a and b
 * and store it in c.
 *
 * @param a vector
 * @param b vector
 * @param c vector [out]
 * @return true/false for success/failure
 */
bool vector_product(vector *a, vector *b, vector *c)
{
	if (!a || !b)
		return false;

	c->x = a->y * b->z - a->z * b->y;
	c->y = a->z * b->x - a->x * b->z;
	c->z = a->x * b->y - a->y * b->x;

	return true;
}
