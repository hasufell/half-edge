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
 * @file common.h
 * Header for common types, macros and constants
 * shared amongst the codebase. Error macros go in err.h.
 * @brief common types, macros, constants
 */

#ifndef _DROW_ENGINE_TYPES_H
#define _DROW_ENGINE_TYPES_H


#include <stdio.h>
#include <stdint.h>


/**
 * Standard file buffer
 */
#define STD_FILE_BUF 4096

/**
 * Realloc macro which checks if reallocation
 * worked via a temporary pointer.
 */
#define REALLOC(ptr, size) \
{ \
	void *tmp_ptr = NULL; \
	tmp_ptr = realloc(ptr, size); \
	if (tmp_ptr == NULL) { \
		fprintf(stderr,"NULL Pointer in %s [%d]",__FILE__,__LINE__); \
		abort(); \
	} \
	ptr = tmp_ptr; \
}


#endif /* _DROW_ENGINE_TYPES_H */
