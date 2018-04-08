#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <iostream>
#include <GL\freeglut.h>
#include <GL\glut.h>

using namespace std;

#define PI = 3.14159265;

#define DEG_TO_RAD = PI / 180.0;

const float GRID_SIZE = 100;

vector<GLfloat> cvectors(3);
vector<GLfloat> cnormals(3);
vector<GLfloat> cfaces(3);

GLfloat theta = 0;

void loadCessna() {
	//load plane's body
	ifstream cessna("cessna.txt");
	char line[256];

	while (!cessna.eof()) {

	}
	

	FILE *propeller = fopen("propeller.txt", "r");

}


/*
* Draws the cordinate lines in the wireframe
*/
void vectors() {
	//draw white ball
	glColor3f(1, 1, 1);
	glutWireSphere(1, 100, 100);

	//draw x
	glBegin(GL_LINES);
	glLineWidth(5);
	glColor3f(1.0, 0, 0);
	glVertex3f(0.0, 0, 0);
	glVertex3f(10, 0, 0);
	glEnd;

	//draw y
	glBegin(GL_LINES);
	glLineWidth(5);
	glColor3f(1.0, 0, 0);
	glVertex3f(0.0, 0, 0);
	glVertex3f(0, 10, 0);
	glEnd;

	//draw z
	glBegin(GL_LINES);
	glLineWidth(5);
	glColor3f(1.0, 0, 0);
	glVertex3f(0.0, 0, 0);
	glVertex3f(0, 0, 10);
	glEnd;
}

void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glDisable(GL_DEPTH_TEST);
	vectors();

	glFlush();
}

void myIdle() {

	glutPostRedisplay();
}

void mouseFunction(int x, int y) {

}

void handleKeypress(unsigned char key, int x, int y) {

}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 10.0, 100);
}

void initializeGL(void) {
	// enable depth test
	glEnable(GL_DEPTH_TEST);
	// set background color
	glClearColor(0.0, 0.0, 0.0, 1.0);
	// set matrix mode
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// use 3d
	glOrtho(-100.0, 100.0, -100.0, 100.0, 10.0, -100.0);

}


void printInstructions() {
	printf("Scene Controls\n");
	printf("------------------------------\n");
	printf("f: toggle fullscreen\n");
	printf("g: toggle fog \n");
	printf("m: toggle mountains\n");
	printf("t: toggle mountains texture\n");
	printf("s: toggle sea & sky\n");
	printf("w: toggle wire frame \n");
	printf("q: quit \n");

	printf("\n");
	printf("Camera Controls\n");
	printf("------------------------------\n");
	printf("Page Up    : faster\n");
	printf("Page Down  : slower\n");
	printf("Up Arrow   : move up\n");
	printf("Mouse Left : move up\n");
	printf("Mouse Right: move right\n");
	printf("Mouse Left : move left\n");
}

void main(int argc, char** argv) {

	//load cessna
	loadCessna();
	//load textures


	//initialize
	glutInit(&argc, argv);
	//display mode
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//window size
	glutInitWindowSize(1000, 800);
	//window position
	glutInitWindowPosition(100, 150);
	//creat window with title
	glutCreateWindow("Cessna: Maiden Flight");
	//handle window resize
	glutReshapeFunc(handleResize);
	//set display function
	glutDisplayFunc(drawScene);
	//idle function
	glutIdleFunc(myIdle);
	//handle keyboard
	glutKeyboardFunc(handleKeypress);
	//handle mouse
	glutPassiveMotionFunc(mouseFunction);
	//print instructions to console
	printInstructions();
	initializeGL();
	//start even loop
	glutMainLoop();
}
