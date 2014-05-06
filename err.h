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

#ifndef _DROW_ENGINE_ERR_H
#define _DROW_ENGINE_ERR_H

#include <stdio.h>

/**
 * @file err.h
 * Holds error handling macros.
 * @brief error handling
 */

/**
 * Abort the program with a given error message.
 */
#define ABORT(...) \
{ \
        fprintf(stderr, __VA_ARGS__); \
        abort(); \
}

/**
 * Used for checking if a pointer is non-NULL
 * after allocation.
 */
#define CHECK_PTR_VAL(ptr) \
{ \
	if (ptr == NULL) { \
		fprintf(stderr,"NULL Pointer in %s [%d]",__FILE__,__LINE__); \
		abort(); \
	} \
}


#endif /* _DROW_ENGINE_ERR_H */
