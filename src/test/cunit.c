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
 * @file cunit.c
 * Main cunit file, registering the test suites.
 * @brief test suite registration
 */

#include "cunit.h"

#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <CUnit/Automated.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int init_suite(void)
{
	return 0;
}

int clean_suite(void)
{
	return 0;
}

int main(void)
{
	CU_pSuite pSuite = NULL;
	int my_stderr;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("filereader tests",
		init_suite,
		clean_suite);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add the tests to the suite */
	if (
		(NULL == CU_add_test(pSuite, "test1 reading plain text file",
							 test_read_text_file1)) ||
		(NULL == CU_add_test(pSuite, "test2 reading plain text file",
							 test_read_text_file2)) ||
		(NULL == CU_add_test(pSuite, "test3 reading plain text file",
							 test_read_text_file3)) ||
		(NULL == CU_add_test(pSuite, "test4 reading plain text file",
							 test_read_text_file4)) ||
		(NULL == CU_add_test(pSuite, "test1 reading obj file",
							 test_read_obj_file1)) ||
		(NULL == CU_add_test(pSuite, "test2 reading obj file",
							 test_read_obj_file2)) ||
		(NULL == CU_add_test(pSuite, "test3 reading obj file",
							 test_read_obj_file3))

		) {

		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add a suite to the registry */
	pSuite = CU_add_suite("half edge tests",
		init_suite,
		clean_suite);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add the tests to the suite */
	if (
		(NULL == CU_add_test(pSuite, "test1 parsing .obj",
							 test_parse_obj1)) ||
		(NULL == CU_add_test(pSuite, "test2 parsing .obj",
							 test_parse_obj2)) ||
		(NULL == CU_add_test(pSuite, "test3 parsing .obj",
							 test_parse_obj3)) ||
		(NULL == CU_add_test(pSuite, "test4 parsing .obj",
							 test_parse_obj4)) ||
		(NULL == CU_add_test(pSuite, "test5 parsing .obj",
							 test_parse_obj5)) ||
		(NULL == CU_add_test(pSuite, "test6 parsing .obj",
							 test_parse_obj6)) ||
		(NULL == CU_add_test(pSuite, "test1 finding center ob obj",
							 test_find_center1)) ||
		(NULL == CU_add_test(pSuite, "test2 finding center ob obj",
							 test_find_center2)) ||
		(NULL == CU_add_test(pSuite, "test3 finding center ob obj",
							 test_find_center3)) ||
		(NULL == CU_add_test(pSuite, "test1 getting normalized scale factor",
							 test_get_normalized_scale_factor1)) ||
		(NULL == CU_add_test(pSuite, "test2 getting normalized scale factor",
							 test_get_normalized_scale_factor2))
		) {

		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add a suite to the registry */
	pSuite = CU_add_suite("vector tests",
		init_suite,
		clean_suite);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add the tests to the suite */
	if (
		(NULL == CU_add_test(pSuite, "test1 calculating vector product",
							 test_vector_product1)) ||
		(NULL == CU_add_test(pSuite, "test2 calculating vector product",
							 test_vector_product2)) ||
		(NULL == CU_add_test(pSuite, "test3 calculating vector product",
							 test_vector_product3)) ||
		(NULL == CU_add_test(pSuite, "test4 calculating vector product",
							 test_vector_product4)) ||
		(NULL == CU_add_test(pSuite, "test5 calculating vector product",
							 test_vector_product5)) ||
		(NULL == CU_add_test(pSuite, "test6 calculating vector product",
							 test_vector_product6)) ||
		(NULL == CU_add_test(pSuite, "test1 adding vectors",
							 test_add_vectors1)) ||
		(NULL == CU_add_test(pSuite, "test2 adding vectors",
							 test_add_vectors2)) ||
		(NULL == CU_add_test(pSuite, "test3 adding vectors",
							 test_add_vectors3)) ||
		(NULL == CU_add_test(pSuite, "test4 adding vectors",
							 test_add_vectors4)) ||
		(NULL == CU_add_test(pSuite, "test5 adding vectors",
							 test_add_vectors5)) ||
		(NULL == CU_add_test(pSuite, "test6 adding vectors",
							 test_add_vectors6)) ||
		(NULL == CU_add_test(pSuite, "test1 substracting vectors",
							 test_sub_vectors1)) ||
		(NULL == CU_add_test(pSuite, "test2 substracting vectors",
							 test_sub_vectors2)) ||
		(NULL == CU_add_test(pSuite, "test3 substracting vectors",
							 test_sub_vectors3)) ||
		(NULL == CU_add_test(pSuite, "test4 substracting vectors",
							 test_sub_vectors4)) ||
		(NULL == CU_add_test(pSuite, "test5 substracting vectors",
							 test_sub_vectors5)) ||
		(NULL == CU_add_test(pSuite, "test6 substracting vectors",
							 test_sub_vectors6)) ||
		(NULL == CU_add_test(pSuite, "test1 normalizing vector",
							 test_normalize_vector1)) ||
		(NULL == CU_add_test(pSuite, "test2 normalizing vector",
							 test_normalize_vector2)) ||
		(NULL == CU_add_test(pSuite, "test3 normalizing vector",
							 test_normalize_vector3)) ||
		(NULL == CU_add_test(pSuite, "test4 normalizing vector",
							 test_normalize_vector4)) ||
		(NULL == CU_add_test(pSuite, "test1 copying vector",
							 test_copy_vector1)) ||
		(NULL == CU_add_test(pSuite, "test2 copying vector",
							 test_copy_vector2)) ||
		(NULL == CU_add_test(pSuite, "test3 copying vector",
							 test_copy_vector3))
		) {

		CU_cleanup_registry();
		return CU_get_error();
	}

	/* save stderr stream and close it */
	my_stderr = dup(STDERR_FILENO);
	close(STDERR_FILENO);

	/* Run all tests using the basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	printf("\n");
	CU_basic_show_failures(CU_get_failure_list());
	printf("\n\n");

	/* Clean up registry and return */
	CU_cleanup_registry();

	/* restore stderr stream */
	dup2(my_stderr, STDERR_FILENO);

	return CU_get_error();
}
