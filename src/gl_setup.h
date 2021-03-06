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
 * @file gl_setup.h
 * Header for the external API of gl_setup.c
 * @brief header of gl_setup.c
 */

#ifndef _DROW_ENGINE_SETUP_H
#define _DROW_ENGINE_SETUP_H


void init_object(char const * const sun,
		char const * const object,
		char const * const bez);
void init_sdl_loop(void);


#endif /* _DROW_ENGINE_SETUP_H */
