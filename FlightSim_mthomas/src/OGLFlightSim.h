/*
 * OGLFlightSim.h
 * @author	: Matthew Thomas, mthomas@cs.dal.ca, B00558461
 * CSCI 3161 Project: Flight sim
 */

#ifndef OGLFlightSim_H_
#define OGLFlightSim_H_
#include <stdio.h>
#include <math.h>
#include <tgmath.h>
#include <unistd.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#include <gl/Gl.h>
#include <gl/freeglut.h>
#define sleep(x) Sleep(1000 * x)
#elif linux
#include <string.h>
#include </usr/include/GL/gl.h>
#include </usr/include/GL/freeglut.h>
typedef unsigned char BYTE;
#endif

//Update logic methods
void update();

//Initialization methods
void init();
void initNew();
void initLight();
void initTextures();
void initSea();
void initSky();
void initGrid();
void initAxis();
void initTarget();
void initMountains();
void initGL(void);
void initializeGL(void);
void initGLUT(int argc, char** argv);
void handleResize(int w, int h);

GLuint loadModel(GLuint id, GLuint numPoints, char* fileName, int colorScheme);

//free memory
void freeApp();

//Drawing methods
void draw();
void drawGrid();
void drawSea();
void drawAxis();
void drawProps();
void drawPlane();
void drawSky();
void drawTarget();
void drawMountains();
void drawTarget();
void drawExplosion(int slices, int stacks);
void drawBullet(float x, float y, float z);
void drawMountain(float mountainDetailAccuracy, int maxIterations);

void colorMountainByHeight(float x, float y, float z, float mountainDetailAccuracy);

void explode();

//controls
void keyDown(unsigned char key, int mouseX, int mouseY);
void keyUp(unsigned char key, int mouseX, int mouseY);
void keySpecialDown(int key, int mouseX, int mouseY);
void keySpecialUp(int key, int mouseX, int mouseY);
void mouseEvent(int x, int y);
void mouseWheel(int button, int state, int x, int y);
void display(int ms);
void handleResize(int w, int h);

float randBetween(int min, int max);

GLfloat renderingOptions[] = { GL_FILL, GL_LINE };
GLUquadricObj *seaObj;
GLUquadricObj *skyObj;
GLUquadricObj *mountainObj;
#endif /* OGLFlightSim_H_ */
