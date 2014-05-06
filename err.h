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
