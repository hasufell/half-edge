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
 * @file cunit_half_edge.c
 * Test functions for the half edge data structure
 * and operations based on it.
 * @brief half-edge test functions
 */

#include "half_edge.h"

#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <CUnit/Automated.h>
#include <stdlib.h>

/**
 * Use a valid string representing an .obj file
 * and test the whole HE_obj structure for correctness.
 */
void test_parse_obj1(void)
{
	char const * const string = ""
		"v 9.0 10.0 11.0\n"
		"v 11.0 10.0 11.0\n"
		"v 9.0 11.0 11.0\n"
		"v 11.0 11.0 11.0\n"
		"v 9.0 11.0 9.0\n"
		"v 11.0 11.0 9.0\n"
		"v 9.0 10.0 9.0\n"
		"v 11.0 10.0 9.0\n"
		"f 1 2 4 3\n"
		"f 3 4 6 5\n"
		"f 5 6 8 7\n"
		"f 7 8 2 1\n"
		"f 2 8 6 4\n"
		"f 7 1 3 5\n";

	HE_obj *obj = parse_obj(string);

	CU_ASSERT_PTR_NOT_NULL(obj);

	CU_ASSERT_EQUAL(obj->vertices[0].x, 9.0);
	CU_ASSERT_EQUAL(obj->vertices[0].y, 10.0);
	CU_ASSERT_EQUAL(obj->vertices[0].z, 11.0);

	CU_ASSERT_EQUAL(obj->vertices[1].x, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[1].y, 10.0);
	CU_ASSERT_EQUAL(obj->vertices[1].z, 11.0);

	CU_ASSERT_EQUAL(obj->vertices[2].x, 9.0);
	CU_ASSERT_EQUAL(obj->vertices[2].y, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[2].z, 11.0);

	CU_ASSERT_EQUAL(obj->vertices[3].x, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[3].y, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[3].z, 11.0);

	CU_ASSERT_EQUAL(obj->vertices[4].x, 9.0);
	CU_ASSERT_EQUAL(obj->vertices[4].y, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[4].z, 9.0);

	CU_ASSERT_EQUAL(obj->vertices[5].x, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[5].y, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[5].z, 9.0);

	CU_ASSERT_EQUAL(obj->vertices[6].x, 9.0);
	CU_ASSERT_EQUAL(obj->vertices[6].y, 10.0);
	CU_ASSERT_EQUAL(obj->vertices[6].z, 9.0);

	CU_ASSERT_EQUAL(obj->vertices[7].x, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[7].y, 10.0);
	CU_ASSERT_EQUAL(obj->vertices[7].z, 9.0);

	CU_ASSERT_EQUAL(obj->vc, 8);

	/*
	 * The following relies on faces always saving the
	 * last listed vertice as their edge member.
	 */
	CU_ASSERT_EQUAL(obj->faces[0].edge->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[0].edge->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[1].edge->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[1].edge->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[2].edge->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[2].edge->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[3].edge->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[3].edge->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[4].edge->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[4].edge->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[5].edge->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[5].edge->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->fc, 6);

	CU_ASSERT_EQUAL(obj->ec, 24);
}

/**
 * Test if the parser correctly ignores comments and other
 * junk.
 */
void test_parse_obj2(void)
{
	char const * const string = ""
		"v 9.0 10.0 11.0\n"
		"v 11.0 10.0 11.0\n"
		"v 9.0 11.0 11.0\n"
		"v 11.0 11.0 11.0\n"
		"v 9.0 11.0 9.0\n"
		"v 11.0 11.0 9.0\n"
		"# blah\n"
		"v 9.0 10.0 9.0\n"
		"v 11.0 10.0 9.0\n"
		"f 1 2 4 3\n"
		"f 3 4 6 5\n"
		"kkldsakl  sakdjl\n"
		"f 5 6 8 7\n"
		"f 7 8 2 1\n"
		"f 2 8 6 4\n"
		"f 7 1 3 5\n"
		"998032lkk\n";

	HE_obj *obj = parse_obj(string);

	CU_ASSERT_PTR_NOT_NULL(obj);

	CU_ASSERT_EQUAL(obj->vertices[0].x, 9.0);
	CU_ASSERT_EQUAL(obj->vertices[0].y, 10.0);
	CU_ASSERT_EQUAL(obj->vertices[0].z, 11.0);

	CU_ASSERT_EQUAL(obj->vertices[1].x, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[1].y, 10.0);
	CU_ASSERT_EQUAL(obj->vertices[1].z, 11.0);

	CU_ASSERT_EQUAL(obj->vertices[2].x, 9.0);
	CU_ASSERT_EQUAL(obj->vertices[2].y, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[2].z, 11.0);

	CU_ASSERT_EQUAL(obj->vertices[3].x, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[3].y, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[3].z, 11.0);

	CU_ASSERT_EQUAL(obj->vertices[4].x, 9.0);
	CU_ASSERT_EQUAL(obj->vertices[4].y, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[4].z, 9.0);

	CU_ASSERT_EQUAL(obj->vertices[5].x, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[5].y, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[5].z, 9.0);

	CU_ASSERT_EQUAL(obj->vertices[6].x, 9.0);
	CU_ASSERT_EQUAL(obj->vertices[6].y, 10.0);
	CU_ASSERT_EQUAL(obj->vertices[6].z, 9.0);

	CU_ASSERT_EQUAL(obj->vertices[7].x, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[7].y, 10.0);
	CU_ASSERT_EQUAL(obj->vertices[7].z, 9.0);

	CU_ASSERT_EQUAL(obj->vc, 8);

	/*
	 * The following relies on faces always saving the
	 * last listed vertice as their edge member.
	 */
	CU_ASSERT_EQUAL(obj->faces[0].edge->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[0].edge->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[1].edge->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[1].edge->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[2].edge->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[2].edge->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[3].edge->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[3].edge->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[4].edge->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[4].edge->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->next->vert->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[5].edge->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[5].edge->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->vert->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->vert->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->next->vert->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->next->vert->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->next->vert->z, 9.0);

	CU_ASSERT_EQUAL(obj->fc, 6);

	CU_ASSERT_EQUAL(obj->ec, 24);
}

/**
 * Test if the parser correctly aborts when passed
 * an empty string.
 */
void test_parse_obj3(void)
{
	char const * const string = "";

	HE_obj *obj = parse_obj(string);

	CU_ASSERT_PTR_NULL(obj);
}

/**
 * Test if the parser correctly aborts when passed
 * a NULL pointer.
 */
void test_parse_obj4(void)
{
	HE_obj *obj = parse_obj(NULL);

	CU_ASSERT_PTR_NULL(obj);
}

/**
 * Test finding the center of an object.
 */
void test_find_center1(void)
{
	char const * const string = ""
		"v 9.0 10.0 11.0\n"
		"v 11.0 10.0 11.0\n"
		"v 9.0 11.0 11.0\n"
		"v 11.0 11.0 11.0\n"
		"v 9.0 11.0 9.0\n"
		"v 11.0 11.0 9.0\n"
		"v 9.0 10.0 9.0\n"
		"v 11.0 10.0 9.0\n"
		"f 1 2 4 3\n"
		"f 3 4 6 5\n"
		"f 5 6 8 7\n"
		"f 7 8 2 1\n"
		"f 2 8 6 4\n"
		"f 7 1 3 5\n";

	HE_obj *obj = parse_obj(string);
	vector newvec;
	bool retval = find_center(obj, &newvec);

	CU_ASSERT_PTR_NOT_NULL(obj);
	CU_ASSERT_EQUAL(retval, true);

	CU_ASSERT_EQUAL(newvec.x, 10.0);
	CU_ASSERT_EQUAL(newvec.y, 10.5);
	CU_ASSERT_EQUAL(newvec.z, 10.0);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_find_center2(void)
{
	vector newvec;
	bool retval = find_center(NULL, &newvec);

	CU_ASSERT_EQUAL(retval, false);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_find_center3(void)
{
	char const * const string = ""
		"v 9.0 10.0 11.0\n"
		"v 11.0 10.0 11.0\n"
		"v 9.0 11.0 11.0\n"
		"v 11.0 11.0 11.0\n"
		"v 9.0 11.0 9.0\n"
		"v 11.0 11.0 9.0\n"
		"v 9.0 10.0 9.0\n"
		"v 11.0 10.0 9.0\n"
		"f 1 2 4 3\n"
		"f 3 4 6 5\n"
		"f 5 6 8 7\n"
		"f 7 8 2 1\n"
		"f 2 8 6 4\n"
		"f 7 1 3 5\n";

	HE_obj *obj = parse_obj(string);
	bool retval = find_center(obj, NULL);

	CU_ASSERT_EQUAL(retval, false);
}

/**
 * Test getting correct normalized scale factor of
 * an object.
 */
void test_get_normalized_scale_factor1(void)
{
	char const * const string = ""
		"v 9.0 10.0 11.0\n"
		"v 11.0 10.0 11.0\n"
		"v 9.0 11.0 11.0\n"
		"v 11.0 11.0 11.0\n"
		"v 9.0 11.0 9.0\n"
		"v 11.0 11.0 9.0\n"
		"v 9.0 10.0 9.0\n"
		"v 11.0 10.0 9.0\n"
		"f 1 2 4 3\n"
		"f 3 4 6 5\n"
		"f 5 6 8 7\n"
		"f 7 8 2 1\n"
		"f 2 8 6 4\n"
		"f 7 1 3 5\n";

	HE_obj *obj = parse_obj(string);
	float factor = get_normalized_scale_factor(obj);

	CU_ASSERT_PTR_NOT_NULL(obj);
	CU_ASSERT_EQUAL(factor, 0.2f);
}

/**
 * Test error handling by passing a NULL pointer.
 */
void test_get_normalized_scale_factor2(void)
{
	float factor = get_normalized_scale_factor(NULL);

	CU_ASSERT_EQUAL(factor, -1);
}
