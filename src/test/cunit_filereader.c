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
 * @file cunit_filereader.c
 * Test functions for the filereader,
 * including various file formats.
 * @brief file reader test functions
 */

#include "filereader.h"
#include "half_edge.h"

#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <CUnit/Automated.h>
#include <stdlib.h>
#include <string.h>


/**
 * Read a valid text file and compare it with the correct expected
 * string.
 */
void test_read_text_file1(void)
{
	char *actual_string = read_file("src/test/test-file.txt"),
		 *expected_string = "This test file is a test file.\n";


	CU_ASSERT_PTR_NOT_NULL(actual_string);
	CU_ASSERT_EQUAL((strcmp(actual_string, expected_string)), 0);
}

/**
 * Read a valid text file and compare it with an uncorrect expected
 * string.
 */
void test_read_text_file2(void)
{
	char *actual_string = read_file("src/test/test-file.txt"),
		 *expected_string = "foo";

	CU_ASSERT_PTR_NOT_NULL(actual_string);
	CU_ASSERT_NOT_EQUAL((strcmp(actual_string, expected_string)), 0);
}

/**
 * Read an invalid text file.
 */
void test_read_text_file3(void)
{
	char *actual_string = actual_string = read_file("asd");

	CU_ASSERT_PTR_NULL(actual_string);
}

/**
 * Pass NULL to the read_file() function.
 */
void test_read_text_file4(void)
{
	char *actual_string = actual_string = read_file(NULL);

	CU_ASSERT_PTR_NULL(actual_string);
}

/**
 * Read a valid .obj file and test the whole HE_obj structure
 * for correctness.
 */
void test_read_obj_file1(void)
{
	HE_obj *obj = read_obj_file("obj/testcube_trans.obj");

	CU_ASSERT_PTR_NOT_NULL(obj);

	CU_ASSERT_EQUAL(obj->vertices[0].vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->vertices[0].vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->vertices[0].vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->vertices[1].vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[1].vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->vertices[1].vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->vertices[2].vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->vertices[2].vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[2].vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->vertices[3].vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[3].vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[3].vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->vertices[4].vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->vertices[4].vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[4].vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->vertices[5].vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[5].vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[5].vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->vertices[6].vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->vertices[6].vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->vertices[6].vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->vertices[7].vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->vertices[7].vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->vertices[7].vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->vc, 8);

	/*
	 * The following relies on faces always saving the
	 * last listed vertice as their edge member.
	 */
	CU_ASSERT_EQUAL(obj->faces[0].edge->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->vert->vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[0].edge->next->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->vert->vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->vert->vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->vert->vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->vert->vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->vert->vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->vert->vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->vert->vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->next->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->next->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[0].edge->next->next->next->next->vert->vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[1].edge->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->vert->vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[1].edge->next->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->vert->vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->vert->vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->vert->vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->vert->vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->vert->vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->next->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->next->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[1].edge->next->next->next->next->vert->vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[2].edge->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->vert->vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->vert->vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[2].edge->next->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->vert->vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->vert->vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->vert->vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->vert->vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->vert->vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->vert->vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->next->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->next->vert->vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[2].edge->next->next->next->next->vert->vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[3].edge->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->vert->vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->vert->vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[3].edge->next->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->vert->vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->vert->vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->vert->vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->vert->vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->vert->vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->vert->vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->vert->vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->vert->vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->next->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->next->vert->vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[3].edge->next->next->next->next->vert->vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[4].edge->vert->vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->vert->vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[4].edge->next->vert->vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->vert->vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->vert->vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->vert->vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->vert->vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->vert->vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->vert->vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->vert->vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->next->vert->vec->x, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->next->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[4].edge->next->next->next->next->vert->vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[5].edge->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->vert->vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[5].edge->next->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->vert->vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->vert->vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->vert->vec->y, 10.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->vert->vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->vert->vec->z, 11.0);

	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->next->vert->vec->x, 9.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->next->vert->vec->y, 11.0);
	CU_ASSERT_EQUAL(obj->faces[5].edge->next->next->next->next->vert->vec->z, 9.0);

	CU_ASSERT_EQUAL(obj->fc, 6);

	CU_ASSERT_EQUAL(obj->ec, 24);
}

/**
 * Read an invalid .obj file.
 */
void test_read_obj_file2(void)
{
	HE_obj *obj = read_obj_file("objfoo");

	CU_ASSERT_PTR_NULL(obj);
}

/**
 * Pass NULL to read_obj_file().
 */
void test_read_obj_file3(void)
{
	HE_obj *obj = read_obj_file(NULL);

	CU_ASSERT_PTR_NULL(obj);
}
