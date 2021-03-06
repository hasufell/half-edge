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
 * @file vector.c
 * Various operations on the vector struct defined
 * in vector.h, including simple maths.
 * @brief vector operations
 */

#include "err.h"
#include "vector.h"

#include <errno.h>
#include <fenv.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>


/**
 * Calculate the vector which lengths is reduced by the
 * factor scal and store it in c.
 * This function is aliasing safe.
 *
 * @param a vector
 * @param scal scalar to multiply the vector with
 * @param c vector [out]
 * @return true/false for success/failure
 */
bool vector_len_scal_mul(const vector *a, const float scal, vector *c)
{
	vector a_tmp;
	float vector_length;
	float factor;

	if (!a || !scal || !c)
		return false;

	copy_vector(a, &a_tmp);

	vector_length = sqrt((a_tmp.x * a_tmp.x) +
			(a_tmp.y * a_tmp.y) + (a_tmp.z * a_tmp.z));

	factor = vector_length * scal;

	c->x = a_tmp.x / vector_length;
	c->y = a_tmp.y / vector_length;
	c->z = a_tmp.z / vector_length;

	c->x = c->x * factor;
	c->y = c->y * factor;
	c->z = c->z * factor;

	return true;
}

/**
 * Calculate the vector product of a and b
 * and store it in c. This function is aliasing safe.
 *
 * @param a vector
 * @param b vector
 * @param c vector [out]
 * @return true/false for success/failure
 */
bool vector_product(const vector *a, const vector *b, vector *c)
{
	vector a_tmp,
		   b_tmp;

	if (!a || !b || !c)
		return false;

	copy_vector(a, &a_tmp);
	copy_vector(b, &b_tmp);

	c->x = a_tmp.y * b_tmp.z - a_tmp.z * b_tmp.y;
	c->y = a_tmp.z * b_tmp.x - a_tmp.x * b_tmp.z;
	c->z = a_tmp.x * b_tmp.y - a_tmp.y * b_tmp.x;

	return true;
}

/**
 * Add vector a to vector b and store the result in c,
 * such as: c = a + b.
 * This function is aliasing safe.
 *
 * @param a vector
 * @param b vector
 * @param c vector [out]
 * @return true/false for success/failure
 */
bool add_vectors(const vector *a, const vector *b, vector *c)
{
	vector a_tmp,
		   b_tmp;

	if (!a || !b || !c)
		return false;

	copy_vector(a, &a_tmp);
	copy_vector(b, &b_tmp);

	c->x = a_tmp.x + b_tmp.x;
	c->y = a_tmp.y + b_tmp.y;
	c->z = a_tmp.z + b_tmp.z;

	return true;
}

/**
 * Substract vector b from vector a and store the result in c,
 * such as: c = a - b.
 * This function is aliasing safe.
 *
 * @param a vector
 * @param b vector
 * @param c vector [out]
 * @return true/false for success/failure
 */
bool sub_vectors(const vector *a, const vector *b, vector *c)
{
	vector a_tmp,
		   b_tmp;

	if (!a || !b || !c)
		return false;

	copy_vector(a, &a_tmp);
	copy_vector(b, &b_tmp);

	c->x = a_tmp.x - b_tmp.x;
	c->y = a_tmp.y - b_tmp.y;
	c->z = a_tmp.z - b_tmp.z;

	return true;
}

/**
 * Normalize a vector into a unit vector
 * of length 1. This function is aliasing safe.
 *
 * @param a vector
 * @param b vector [out]
 * @return true/false for success/failure
 */
bool normalize_vector(const vector *a, vector *b)
{
	if (!a || !b)
		return false;

	vector a_tmp;
	float vector_length;

	copy_vector(a, &a_tmp);

	vector_length = sqrt((a_tmp.x * a_tmp.x) +
			(a_tmp.y * a_tmp.y) + (a_tmp.z * a_tmp.z));

	if (errno == EDOM)
		return false;

	b->x = a_tmp.x / vector_length;
	b->y = a_tmp.y / vector_length;
	b->z = a_tmp.z / vector_length;

	return true;
}

/**
 * Copy coordinates from vector a to vector b.
 *
 * @param a vector
 * @param b vector [out]
 * @return true/false for success/failure
 */
bool copy_vector(const vector *a, vector *b)
{
	if (!a || !b || (a == b))
		return false;

	b->x = a->x;
	b->y = a->y;
	b->z = a->z;

	return true;
}

/**
 * Set the vector to a null vector.
 *
 * @param a vector [out]
 * @return true/false for success/failure
 */
bool set_null_vector(vector *a)
{
	if (!a)
		return false;

	a->x = 0;
	a->y = 0;
	a->z = 0;

	return true;

}

/**
 * Check if vector a is a null vector,
 * so all coordinates are 0.
 *
 * @param a vector
 * @return true if vector is a null vector, false
 * otherwise, -1 on failure
 */
int is_null_vector(const vector *a)
{
	if (!a)
		return -1;

	if (a->x == 0 &&
			a->y == 0 &&
			a->z == 0)
		return true;
	else
		return false;
}
