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

#include "vector.h"

#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <CUnit/Automated.h>
#include <stdbool.h>
#include <stdlib.h>


/**
 * Test calculating vector product.
 */
void test_vector_product1(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 };
	vector c;

	bool retval = vector_product(&a, &b, &c);

	CU_ASSERT_EQUAL(retval, true);
	CU_ASSERT_EQUAL(c.x, 35);
	CU_ASSERT_EQUAL(c.y, 8);
	CU_ASSERT_EQUAL(c.z, -25);
}

/**
 * Test calculating vector product, aliasing-safe.
 */
void test_vector_product2(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 };

	bool retval = vector_product(&a, &b, &a);

	CU_ASSERT_EQUAL(retval, true);
	CU_ASSERT_EQUAL(a.x, 35);
	CU_ASSERT_EQUAL(a.y, 8);
	CU_ASSERT_EQUAL(a.z, -25);
}

/**
 * Test calculating vector product, aliasing-safe.
 */
void test_vector_product3(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 };

	bool retval = vector_product(&a, &b, &b);

	CU_ASSERT_EQUAL(retval, true);
	CU_ASSERT_EQUAL(b.x, 35);
	CU_ASSERT_EQUAL(b.y, 8);
	CU_ASSERT_EQUAL(b.z, -25);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_vector_product4(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 },
		   c;

	bool retval = vector_product(&a, &b, NULL);

	CU_ASSERT_EQUAL(retval, false);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_vector_product5(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 },
		   c;

	bool retval = vector_product(NULL, &b, &c);

	CU_ASSERT_EQUAL(retval, false);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_vector_product6(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 },
		   c;

	bool retval = vector_product(&a, NULL, &c);

	CU_ASSERT_EQUAL(retval, false);
}

/**
 * Test adding vectors.
 */
void test_add_vectors1(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 },
		   c;

	bool retval = add_vectors(&a, &b, &c);

	CU_ASSERT_EQUAL(retval, true);

	CU_ASSERT_EQUAL(c.x, 6);
	CU_ASSERT_EQUAL(c.y, 5);
	CU_ASSERT_EQUAL(c.z, 10);
}

/**
 * Test adding vectors, aliasing-safe.
 */
void test_add_vectors2(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 };

	bool retval = add_vectors(&a, &b, &a);

	CU_ASSERT_EQUAL(retval, true);

	CU_ASSERT_EQUAL(a.x, 6);
	CU_ASSERT_EQUAL(a.y, 5);
	CU_ASSERT_EQUAL(a.z, 10);
}

/**
 * Test adding vectors, aliasing-safe.
 */
void test_add_vectors3(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 };

	bool retval = add_vectors(&a, &b, &b);

	CU_ASSERT_EQUAL(retval, true);

	CU_ASSERT_EQUAL(b.x, 6);
	CU_ASSERT_EQUAL(b.y, 5);
	CU_ASSERT_EQUAL(b.z, 10);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_add_vectors4(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 },
		   c;

	bool retval = add_vectors(&a, NULL, &c);

	CU_ASSERT_EQUAL(retval, false);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_add_vectors5(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 },
		   c;

	bool retval = add_vectors(&a, &b, NULL);

	CU_ASSERT_EQUAL(retval, false);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_add_vectors6(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 },
		   c;

	bool retval = add_vectors(NULL, &b, &c);

	CU_ASSERT_EQUAL(retval, false);
}

/**
 * Test substracting vectors.
 */
void test_sub_vectors1(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 },
		   c;

	bool retval = sub_vectors(&a, &b, &c);

	CU_ASSERT_EQUAL(retval, true);

	CU_ASSERT_EQUAL(c.x, -4);
	CU_ASSERT_EQUAL(c.y, 5);
	CU_ASSERT_EQUAL(c.z, -4);
}

/**
 * Test substracting vectors, aliasing-safe.
 */
void test_sub_vectors2(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 };

	bool retval = sub_vectors(&a, &b, &a);

	CU_ASSERT_EQUAL(retval, true);

	CU_ASSERT_EQUAL(a.x, -4);
	CU_ASSERT_EQUAL(a.y, 5);
	CU_ASSERT_EQUAL(a.z, -4);
}

/**
 * Test substracting vectors, aliasing-safe.
 */
void test_sub_vectors3(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 };

	bool retval = sub_vectors(&a, &b, &b);

	CU_ASSERT_EQUAL(retval, true);

	CU_ASSERT_EQUAL(b.x, -4);
	CU_ASSERT_EQUAL(b.y, 5);
	CU_ASSERT_EQUAL(b.z, -4);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_sub_vectors4(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 },
		   c;

	bool retval = sub_vectors(&a, NULL, &c);

	CU_ASSERT_EQUAL(retval, false);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_sub_vectors5(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 },
		   c;

	bool retval = sub_vectors(&a, &b, NULL);

	CU_ASSERT_EQUAL(retval, false);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_sub_vectors6(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 },
		   c;

	bool retval = sub_vectors(NULL, &b, &c);

	CU_ASSERT_EQUAL(retval, false);
}

/**
 * Test normalizing a vector.
 */
void test_normalize_vector1(void)
{
	vector a = { 24, 0, 0 },
		   b;

	bool retval = normalize_vector(&a, &b);

	CU_ASSERT_EQUAL(retval, true);

	CU_ASSERT_EQUAL(b.x, 1.0f);
	CU_ASSERT_EQUAL(b.y, 0.0f);
	CU_ASSERT_EQUAL(b.z, 0.0f);
}

/**
 * Test normalizing a vector, aliasing-safe.
 */
void test_normalize_vector2(void)
{
	vector a = { 24, 0, 0 };

	bool retval = normalize_vector(&a, &a);

	CU_ASSERT_EQUAL(retval, true);

	CU_ASSERT_EQUAL(a.x, 1.0f);
	CU_ASSERT_EQUAL(a.y, 0.0f);
	CU_ASSERT_EQUAL(a.z, 0.0f);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_normalize_vector3(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 };

	bool retval = normalize_vector(&a, NULL);

	CU_ASSERT_EQUAL(retval, false);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_normalize_vector4(void)
{
	vector a = { 1, 5, 3 },
		   b = { 5, 0, 7 };

	bool retval = normalize_vector(NULL, &b);

	CU_ASSERT_EQUAL(retval, false);
}

/**
 * Test copying vectors.
 */
void test_copy_vector1(void)
{
	vector a = { 1, 5, 3 },
		   b;

	bool retval = copy_vector(&a, &b);

	CU_ASSERT_EQUAL(retval, true);

	CU_ASSERT_EQUAL(b.x, 1);
	CU_ASSERT_EQUAL(b.y, 5);
	CU_ASSERT_EQUAL(b.z, 3);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_copy_vector2(void)
{
	vector a = { 1, 5, 3 },
		   b;

	bool retval = copy_vector(&a, NULL);

	CU_ASSERT_EQUAL(retval, false);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_copy_vector3(void)
{
	vector a = { 1, 5, 3 },
		   b;

	bool retval = copy_vector(NULL, &b);

	CU_ASSERT_EQUAL(retval, false);
}
