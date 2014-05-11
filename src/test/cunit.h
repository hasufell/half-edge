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
 * @file cunit.h
 * Main cunit header, containing declarations
 * of all test functions.
 * @brief test function declarations
 */

/*
 * filereader tests
 */
void test_read_text_file1(void);
void test_read_text_file2(void);
void test_read_text_file3(void);
void test_read_text_file4(void);

void test_read_obj_file1(void);
void test_read_obj_file2(void);
void test_read_obj_file3(void);

/*
 * half_edge tests
 */
void test_parse_obj1(void);
void test_parse_obj2(void);
void test_parse_obj3(void);
void test_parse_obj4(void);

void test_find_center1(void);
void test_find_center2(void);
void test_find_center3(void);

void test_get_normalized_scale_factor1(void);
void test_get_normalized_scale_factor2(void);

/*
 * vector tests
 */
void test_vector_product1(void);
void test_vector_product2(void);
void test_vector_product3(void);
void test_vector_product4(void);
void test_vector_product5(void);
void test_vector_product6(void);

void test_add_vectors1(void);
void test_add_vectors2(void);
void test_add_vectors3(void);
void test_add_vectors4(void);
void test_add_vectors5(void);
void test_add_vectors6(void);

void test_sub_vectors1(void);
void test_sub_vectors2(void);
void test_sub_vectors3(void);
void test_sub_vectors4(void);
void test_sub_vectors5(void);
void test_sub_vectors6(void);

void test_normalize_vector1(void);
void test_normalize_vector2(void);
void test_normalize_vector3(void);
void test_normalize_vector4(void);

void test_copy_vector1(void);
void test_copy_vector2(void);
void test_copy_vector3(void);
