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
 * @file gl_setup.c
 * Operations for setting up SDL and the OpenGL context.
 * @brief set up SDL/OpenGL
 */

#include "err.h"
#include "filereader.h"
#include "gl_draw.h"
#include "gl_setup.h"
#include "half_edge.h"

#include <GL/glut.h>
#include <GL/gl.h>

#include <SDL.h>


/*
 * static function declaration
 */
static void init_opengl(void);
static bool process_events(SDL_Window *win, SDL_GLContext glctx);
static void reshape(SDL_Window *win);
static bool process_window_events(SDL_Window *win,
		SDL_GLContext glctx,
		SDL_WindowEvent *win_event);
static bool process_keypress(SDL_KeyboardEvent *key_event);
static void gl_destroy(SDL_Window *win, SDL_GLContext glctx);


/**
 * Sets the initial values to start the program.
 */
static void init_opengl(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	glViewport(0, 0, 680, 480);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(CAMERA_ANGLE,
			(GLfloat) 640 / (GLfloat) 480,
			NEAR_CLIPPING_PLANE,
			FAR_CLIPPING_PLANE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0, 0.0, -5.0);
}

/**
 * Processes all sort of Events the SDL system catches.
 *
 * @param win the SDL window
 * @param glctx the OpenGL context
 * @return true if the app is still running, false if it should
 * terminate
 */
static bool process_events(SDL_Window *win, SDL_GLContext glctx)
{
	SDL_Event e;
	bool running = true;

	while (SDL_PollEvent(&e))
		if (e.type == SDL_QUIT) {
			running = false;
		} else if (e.type == SDL_WINDOWEVENT) {
			running = process_window_events(win, glctx, &(e.window));
		} else if (e.type == SDL_KEYDOWN) {
			running = process_keypress(&(e.key));
		}

	return running;
}

/**
 * Is called whenever the window size changes, so
 * the object is reshaped into the new window size.
 *
 * @param win the SDL window
 */
static void reshape(SDL_Window *win)
{
	int w, h;

	SDL_GetWindowSize(win, &w, &h);

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(CAMERA_ANGLE,
			(GLfloat) w / (GLfloat) h,
			NEAR_CLIPPING_PLANE,
			FAR_CLIPPING_PLANE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0, 0.0, -5.0);
}

/**
 * Processes all sort of window events the SDL system catches.
 *
 * @param win the SDL window
 * @param glctx the OpenGL context
 * @param win_event the window event to process
 * @return true if the app is still running, false if it should
 * terminate
 */
static bool process_window_events(SDL_Window *win,
		SDL_GLContext glctx,
		SDL_WindowEvent *win_event)
{
	bool running = true;

	if (win_event->event == SDL_WINDOWEVENT_RESIZED)
		reshape(win);
	else if (win_event->event == SDL_WINDOWEVENT_CLOSE)
		running = false;

	return running;
}

/**
 * Processes all sort of keyboard events the SDL system catches.
 *
 * press t to increase the day
 *
 * press T to decrease the day
 *
 * press j to increase the year
 *
 * press J to decrease the year
 *
 * press b to toggle GL_CULL_FACE
 *
 * press D to toggle disco mode
 *
 * press S to toggle shade model between GL_SMOOTH and GL_FLAT
 *
 * press n to toggle normals
 *
 * press k to increase length of normals
 *
 * press l to decrease length of normals
 *
 * press x to rotate the middle object in x direction
 *
 * press X to rotate the middle object in -x direction
 *
 * press y to rotate the middle object in y direction
 *
 * press Y to rotate the middle object in -y direction
 *
 * press c to rotate the middle object in z direction
 *
 * press C to rotate the middle object in -z direction
 *
 * press w to translate the whole scene in y direction
 *
 * press s to translate the whole scene in -y direction
 *
 * press a to translate the whole scene in -x direction
 *
 * press d to translate the whole scene in x direction
 *
 * press q to quit
 *
 * @param key_event the key event to process
 * @return true if the app is still running, false if it should
 * terminate
 */
static bool process_keypress(SDL_KeyboardEvent *key_event)
{
	SDL_Keycode key = key_event->keysym.sym;
	SDL_Keymod mod = key_event->keysym.mod;
	bool running = true;

	switch (key) {
	/* case 'b': */
		/* if (glIsEnabled(GL_CULL_FACE)) */
			/* glDisable(GL_CULL_FACE); */
		/* else */
			/* glEnable(GL_CULL_FACE); */
		/* break; */
	case 't':
		if (mod & KMOD_SHIFT) {
			dayabs -= 15;
		} else {
			dayabs += 15;
		}
		break;
	case 'j':
		if (mod & KMOD_SHIFT) {
			yearabs -= 50;
		} else {
			yearabs += 50;
		}
		break;
	case 'x':
		if (mod & KMOD_SHIFT) {
			draw_obj(-2, 0, 0, 0);
		} else {
			draw_obj(2, 0, 0, 0);
		}
		break;
	case 'y':
		if (mod & KMOD_SHIFT) {
			draw_obj(0, -2, 0, 0);
		} else {
			draw_obj(0, 2, 0, 0);
		}
		break;
	case 'c':
		if (mod & KMOD_SHIFT) {
			draw_obj(0, 0, -2, 0);
		} else {
			draw_obj(0, 0, 2, 0);
		}
		break;
	case 'd':
		if (mod & KMOD_SHIFT) {
			draw_vertices(obj, true);
		} else {
			glTranslatef(1.0f, 0.0f, 0.0f);
		}
		break;
	case 's':
		if (mod & KMOD_SHIFT) {
			if (shademodel) {
				glShadeModel(GL_FLAT);
				shademodel = false;
			} else {
				glShadeModel(GL_SMOOTH);
				shademodel = true;
			}
		} else {
			glTranslatef(0.0f, -1.0f, 0.0f);
		}
		break;
	case 'b':
		if (mod & KMOD_SHIFT) {
			draw_obj(0, 0, 0, -0.02);
		} else {
			draw_obj(0, 0, 0, 0.02);
		}
		break;
	case 'k':
		if (mod & KMOD_SHIFT) {
			if (ball_speed - 0.2f > 0)
				ball_speed -= 0.2f;
		} else {
			ball_speed += 0.2f;
		}
		break;
	case 'f':
		draw_frame = !draw_frame;
		break;
	case 'l':
		if (mod & KMOD_SHIFT) {
			draw_normals(obj, 0.01f);
		} else {
			draw_normals(obj, -0.01f);
		}
		break;
	case 'w':
		glTranslatef(0.0f, 1.0f, 0.0f);
		break;
	case 'a':
		glTranslatef(-1.0f, 0.0f, 0.0f);
		break;
	case 'n':
		show_normals = !show_normals;
		break;
	case '+':
		glTranslatef(0.0f, 0.0f, 1.0f);
		break;
	case '-':
		glTranslatef(0.0f, 0.0f, -1.0f);
		break;
	case 'q':
		running = false;
		break;
	}

	return running;
}

/**
 * Destroy the gl session/window.
 */
static void gl_destroy(SDL_Window *win, SDL_GLContext glctx)
{
	delete_object(obj);
	free(obj);

	SDL_GL_DeleteContext(glctx);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

/**
 * Initialize the global obj object.
 *
 * @param filename the file to parse and build the object from
 */
void init_object(char const * const filename)
{
	obj = read_obj_file(filename);

	if (!obj)
		ABORT("Failed to read object file \"%s\"!", filename);

	NORMALIZE_OBJECT(obj);
}

/**
 * Starts the main SDL loop which runs until the user
 * ends the program.
 */
void init_sdl_loop(void)
{
	SDL_Window *win;
	SDL_GLContext glctx;
	const int window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Failed initalizing SDL!\n");
		abort();
	}

	if (!(win = SDL_CreateWindow("Drow Engine", 100, 100, 640, 480,
					window_flags))) {
		fprintf(stderr, "Failed creating SDL window!\n");
		abort();
	}

	if (!(glctx = SDL_GL_CreateContext(win))) {
		fprintf(stderr, "Failed creating OpenGL context!\n");
		abort();
	}

	SDL_GL_SetSwapInterval(1);

	init_opengl();

	while (1) {
		bool running = process_events(win, glctx);

		if (!running)
			break;

		draw_scene();
		SDL_GL_SwapWindow(win);
	}

	gl_destroy(win, glctx);
}
