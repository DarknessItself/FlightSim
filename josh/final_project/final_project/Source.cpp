#include <freeglut.h>
#include <stdio.h>

#define PI 3.14159265
#define DEG_TO_RAD PI/180.0

GLfloat eyex = 0, eyey = 0, eyez = 5, centerx = 0, centery = 0, centerz = 0;
int wire = 1;

void init(void) {
	glViewport(0, 0, 1080, 500);
	//glShadeModel(GL_FLAT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(10.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1080 / 500, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, 0.0, 1.0, 0.0);
}

void idle(void) {

	glutPostRedisplay();
}

void drawBaseScene(void) {
	GLfloat x, y;
	glPushMatrix();
	glTranslatef(0.0, -2.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	for (y = -50.0; y < 50; y++) {
		for (x = -50.0; x < 50; x++) {
			glVertex3f(x, y, 0.0);
			glVertex3f(x + 1.0, y, 0.0);
			glVertex3f(x + 1.0, y + 1.0, 0.0);
			glVertex3f(x, y + 1.0, 0.0);
		}
	}
	glEnd();
	glPopMatrix();
}
void drawWireSea(void) {
	GLUquadricObj* quadricPtr;
	quadricPtr = gluNewQuadric();
	gluQuadricDrawStyle(quadricPtr, GLU_LINE);
	gluQuadricNormals(quadricPtr, GLU_NONE);
	gluQuadricTexture(quadricPtr, GL_FALSE);
	glPushMatrix();
	glTranslatef(0.0, -2.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	gluDisk(quadricPtr, 0, 20, 20, 50);
	glPopMatrix();
}

void drawWireSky(void) {
	GLUquadricObj* quadricPtr;
	quadricPtr = gluNewQuadric();
	gluQuadricDrawStyle(quadricPtr, GLU_LINE);
	gluQuadricNormals(quadricPtr, GLU_NONE);
	gluQuadricTexture(quadricPtr, GL_FALSE);
	glPushMatrix();
	glTranslatef(0.0, 18.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	gluCylinder(quadricPtr, 20, 20, 20, 20, 20);
	glPopMatrix();
}

void display(void) {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	if (wire) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	
	glutSwapBuffers();
}

void speKeyboard(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		eyey += 0.1; centery += 0.1;
		break;
	case GLUT_KEY_DOWN:
		eyey -= 0.1; centery -= 0.1;
		break;
	case GLUT_KEY_LEFT:
		eyex -= 0.1; centerx -= 0.1;
		break;
	case GLUT_KEY_RIGHT:
		eyex += 0.1; centerx += 0.1;
		break;
	case GLUT_KEY_PAGE_UP:
		eyez -= 0.1, centerz -= 0.1;
		break;
	case GLUT_KEY_PAGE_DOWN:
		eyez += 0.1, centerz += 0.1;
		break;
	default:
		break;
	}
	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, 0.0, 1.0, 0.0);
	glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		wire = !wire;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1080, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Assignment 2");
	glutDisplayFunc(display);
	glutSpecialFunc(speKeyboard);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	init();
	glutMainLoop();
	return 0;
}