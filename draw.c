#include "types.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <math.h>
#include <limits.h>
#include <unistd.h>

#include <stdio.h>

#define filename __FILE__
#define XY_WIRE_COUNT 10.0f

#define ROT_FACTOR_PLANET_SUN (360.0 / yearabs)
#define ROT_FACTOR_PLANET (360.0 / 1.0)
#define ROT_FACTOR_MOON (360.0 / dayabs)

#define SYSTEM_POS_Z -15.0f
#define SYSTEM_POS_Z_BACK 15.0f

int year;
int yearabs = 365;
int day;
int dayabs = 30;
HE_obj *obj;

/*
 * static function declaration
 */
static void draw_Sun(void);
static void draw_Planet_1(void);
static void draw_Planet_2(void);


/**
 * Draws a sun which changes its color randomly.
 */
static void draw_Sun(void)
{
	glPushMatrix();

	/* pull into middle of universe */
	glTranslatef(-obj->faces->edge->vert->x,
			-obj->faces->edge->vert->y,
			-obj->faces->edge->vert->z + SYSTEM_POS_Z);

	glBegin(GL_POLYGON);

	glColor3f(0.0f, 1.0f, 0.0f);
	for (uint32_t i = 0; i < obj->fc; i++) { /* for all faces */
		HE_edge *tmp_edge = obj->faces[i].edge;

		do { /* for all edges of the face */
			glVertex3f(tmp_edge->vert->x,
					tmp_edge->vert->y,
					tmp_edge->vert->z);

		} while ((tmp_edge = tmp_edge->next) != obj->faces[i].edge);
	}

	glEnd();

	glPopMatrix();
}

/**
 * Draws a planet with some Saturn like rings, two moons and an axis.
 * Rotates the planet around the sun, the moons around the planet
 * and the planet around its axis.
 */
static void draw_Planet_1(void)
{
	GLUquadric* quadric = NULL;
	quadric = gluNewQuadric();
	const int rot_fac_day = 15;

	glPushMatrix();

	//Rotate around the sun
	glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z);
	glRotatef(90, 1.0f, 0.0f, 0.0f);
	glRotatef((ROT_FACTOR_PLANET_SUN * day), 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 4.0f, 0.0f);
	glRotatef((ROT_FACTOR_PLANET_SUN * day), 0.0f, 0.0f, -1.0f);
	glRotatef(315, 0.0f, 1.0f, 0.0f);

	glColor3f(1.0f, 0.0f, 0.0f);

	//A rotation (full 360°) once a day is much too fast you woulden'd see a thing
	glRotatef((ROT_FACTOR_PLANET * day) / rot_fac_day, 0.0f, 0.0f, 1.0f);
	glutWireSphere(1.0f, XY_WIRE_COUNT, XY_WIRE_COUNT);
	glRotatef((ROT_FACTOR_PLANET * day) / rot_fac_day, 0.0f, 0.0f, -1.0f);

	//Center axis
	glPushMatrix();
	glLineWidth(3);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
	glVertex3f(0, 0, -5);
	glVertex3f(0, 0, 5);
	glEnd();
	glPopMatrix();

	//circle1
	glPushMatrix();
	glColor3f(0.8f, 0.0f, 0.2f);
	//glRotatef(90, 0.0f, 1.0f, 0.0f); //"senkrecht zur Planetenachse"
	gluDisk(quadric, 1.2f, 1.3f, 32, 1);
	glPopMatrix();

	//circle2
	glPushMatrix();
	glColor3f(0.0f, 1.0f, 0.0f);
	//glRotatef(90, 0.0f, 1.0f, 0.0f); //"senkrecht zur Planetenachse"
	gluDisk(quadric, 1.4f, 1.7f, 32, 1);
	glPopMatrix();

	//Moon1
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);
	//glRotatef((ROT_FACTOR_MOON * day), 1.0f, 0.0f, 0.0f); //"senkrecht zur Planetenachse"
	glRotatef((ROT_FACTOR_MOON * day), 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 2.0f, 0.0f);
	glutWireSphere(0.1f, XY_WIRE_COUNT, XY_WIRE_COUNT);
	glPopMatrix();

	//Moon2
	glPushMatrix();
	glColor3f(0.0f, 1.0f, 1.0f);
	//glRotatef((ROT_FACTOR_MOON * day), 1.0f, 0.0f, 0.0f); //"senkrecht zur Planetenachse"
	glRotatef((ROT_FACTOR_MOON * day), 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, -2.0f, 0.0f);
	glutWireSphere(0.1f, XY_WIRE_COUNT, XY_WIRE_COUNT);
	glPopMatrix();

	glPopMatrix();

}

/**
 * Draws a Planet with three moons.
 * The planet rotates around the sun and the moons around the planet
 * and the planet around its axis.
 */
static void draw_Planet_2(void)
{
	glPushMatrix();
	const float moon_pos_fac = 2.5;

	//Rotate around the sun
	glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z);
	glRotatef(90, 1.0f, 0.0f, 0.0f);
	glRotatef((ROT_FACTOR_PLANET_SUN * day), 0.0f, 0.0f, 1.0f);
	glTranslatef(-2.0f, -8.0f, 0.0f);

	glColor3f(0.0f, 0.0f, 1.0f);

	//A rotation (full 360°) once a day is much too fast you woulden'd see a thing
	const int rot_fac_day = 15;
	glRotatef((ROT_FACTOR_PLANET * day) / rot_fac_day, 0.0f, 0.0f, 1.0f);
	glutWireSphere(1.3f, XY_WIRE_COUNT, XY_WIRE_COUNT);
	glRotatef((ROT_FACTOR_PLANET * day) / rot_fac_day, 0.0f, 0.0f, -1.0f);

	//Moon3
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glRotatef((ROT_FACTOR_MOON * day), 0.0f, 0.0f, 1.0f);
	glTranslatef(cos(0 * (M_PI / 180)) * moon_pos_fac,
			sin(0 * (M_PI / 180)) * moon_pos_fac, 0.0f);
	glutWireSphere(0.1f, XY_WIRE_COUNT, XY_WIRE_COUNT);
	glPopMatrix();

	//Moon4
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 1.0f);
	glRotatef((ROT_FACTOR_MOON * day), 0.0f, 0.0f, 1.0f);
	glTranslatef(cos(120 * (M_PI / 180)) * moon_pos_fac,
			sin(120 * (M_PI / 180)) * moon_pos_fac, 0.0f);
	glutWireSphere(0.1f, XY_WIRE_COUNT, XY_WIRE_COUNT);
	glPopMatrix();

	//Moon5
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glRotatef((ROT_FACTOR_MOON * day), 0.0f, 0.0f, 1.0f);
	glTranslatef(cos(240 * (M_PI / 180)) * moon_pos_fac,
			sin(240 * (M_PI / 180)) * moon_pos_fac, 0.0f);
	glutWireSphere(0.1f, XY_WIRE_COUNT, XY_WIRE_COUNT);
	glPopMatrix();

	glPopMatrix();
}

/**
 * Displays the whole setup with the sun, planet one, planet two and the frame rate
 */
void display(void)
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	draw_Sun();
	draw_Planet_1();
	draw_Planet_2();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, 500, 0.0, 500);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2i(5, 10);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
	glutSwapBuffers();
}

/**
 * Sets the initial values to start the program.
 */
void init(HE_obj *myobj)
{
	obj = myobj;
	day = 0;
	year = 0;
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
}

/**
 * Is called when the window size changes.
 * Fits the viewport to the new dimension.
 *
 * @param w the new width of the window
 * @param h the new height of the window
 */
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat) w / (GLfloat) h, 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
}

/**
 * Is called over and over again.
 * Counts the years and days.
 */
void animate()
{
	day++;
	if (day >= yearabs) {
		day = 0;
		year++;
	}
	if (year >= (INT_MAX - 1000) || year < 0) {
		year = 0;
	}
	if (day < 0) {
		day = 0;
	}

	/* cout << day << endl; */
	usleep(30000); // some very basic VScny to see the movement better
	glutPostRedisplay();
}

/**
 * Keyboard callback function,
 *
 * press t to increase the day
 * press T to decrease the day
 * press j to increase the year
 * press J to decrease the year
 * press x to rotate the whole scene in x direction
 * press X to rotate the whole scene in -x direction
 * press y to rotate the whole scene in y direction
 * press Y to rotate the whole scene in -y direction
 * press c to rotate the whole scene in z direction
 * press C to rotate the whole scene in -z direction
 * press w to translate the whole scene in y direction
 * press s to translate the whole scene in -y direction
 * press a to translate the whole scene in -x direction
 * press d to translate the whole scene in x direction
 * press q to translate the whole scene in z direction
 * press e to translate the whole scene in -z direction
 *
 * @param key which was pressed
 * @param x coordinate
 * @param y coordinate
 */
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 't':
		dayabs += 15;
		glutPostRedisplay();
		break;
	case 'j':
		yearabs += 50;
		glutPostRedisplay();
		break;
	case 'T':
		dayabs -= 15;
		glutPostRedisplay();
		break;
	case 'J':
		yearabs -= 50;
		glutPostRedisplay();
		break;
	case 'x':
		glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z);
		glRotatef(1, 1.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z_BACK);
		glutPostRedisplay();
		break;
	case 'y':
		glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z);
		glRotatef(1, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z_BACK);
		glutPostRedisplay();
		break;
	case 'c':
		glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z);
		glRotatef(1, 0.0f, 0.0f, 1.0f);
		glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z_BACK);
		glutPostRedisplay();
		break;
	case 'X':
		glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z);
		glRotatef(1, -1.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z_BACK);
		glutPostRedisplay();
		break;
	case 'Y':
		glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z);
		glRotatef(1, 0.0f, -1.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z_BACK);
		glutPostRedisplay();
		break;
	case 'C':
		glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z);
		glRotatef(1, 0.0f, 0.0f, -1.0f);
		glTranslatef(0.0f, 0.0f, SYSTEM_POS_Z_BACK);
		glutPostRedisplay();
		break;
	case 'w':
		glTranslatef(0.0f, 1.0f, 0.0f);
		break;
	case 'a':
		glTranslatef(-1.0f, 0.0f, 0.0f);
		break;
	case 's':
		glTranslatef(0.0f, -1.0f, 0.0f);
		break;
	case 'd':
		glTranslatef(1.0f, 0.0f, 0.0f);
		break;
	case 'q':
		glTranslatef(0.0f, 0.0f, 1.0f);
		break;
	case 'e':
		glTranslatef(0.0f, 0.0f, -1.0f);
		break;
	}
}
