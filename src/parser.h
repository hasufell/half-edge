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

#ifndef _DROW_ENGINE_PARSER_H
#define _DROW_ENGINE_PARSER_H

#include "half_edge.h"

#include <stdint.h>


/**
 * Typedef for the plain faces
 * that are not yet converted to real HE_face.
 */
typedef uint32_t** FACE;

HE_obj *parse_obj(char const * const filename);


#endif /* _DROW_ENGINE_PARSER_H */
