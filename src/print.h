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
 * @file print.h
 * Header for the external API of print.c.
 * @brief header of print.c
 */

#ifndef _DROW_ENGINE_PRINT_H
#define _DROW_ENGINE_PRINT_H


#include "half_edge.h"
#include "vector.h"


void print_edges(HE_obj *obj);
void print_vertices(HE_obj *obj);
void print_faces(HE_obj *obj);
void print_plain_faces(FACE face, uint32_t fc);
void print_vector(vector *vec);


#endif /* _DROW_ENGINE_PRINT_H */
