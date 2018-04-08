/*
 * Common.h
 * @author	: Matthew Thomas, mthomas@cs.dal.ca, B00558461
 * CSCI 3161 Project Flight Sim
 */

#ifndef COMMON_H_
#define COMMON_H_

const GLfloat seaDetailAccuracy = 100.0f;
GLuint mountainDetailAccuracy = 81;
const GLfloat gridSize = 200.0f;
GLint appWidth = 1600;
GLint appHeight = 900;
//used to return to previous size when exiting fullscreen
GLint storedWidth = 1600;
GLint storedHeight = 900;
GLint storedX = 100;
GLint storedY = 100;
int appX = 100;
int appY = 100;
GLfloat nearValue = 0.1f;
GLfloat farValue = 100000.0f;
GLfloat universeTime = 0.0f;
GLfloat delta = 1.0f;
GLfloat explosionScale=1.0f;
GLuint exploding=0;
GLint fov = 60;
GLfloat originalFogDensity = 0.005f;

GLint lastMouseX;
GLint lastMouseY;
GLuint alive = 1;
GLdouble eyeX = 0;
GLdouble eyeY = 5;
GLdouble eyeZ = -15;
GLdouble atX = 0;
GLdouble atY = 5;
GLdouble atZ = -5;

GLint toggleWireframe = 0;
GLint toggleFullscreen = 0;
GLint toggleFog = 1;
GLint toggleGrid = 0;
GLint toggleMountains = 1;
GLint toggleAltControls = 0;
GLint toggleAltWeather = 0;
GLint toggleMountainTextures = 1;

GLint boolAccelerate = 0;
GLint boolDeaccelerate = 0;
GLint boolMoveUp = 0;
GLint boolMoveDown = 0;
GLint boolShoot=0;
GLfloat planeSpeed = 1.0f;
GLfloat planeMaxSpeed = 4.0f;
GLfloat planeMinSpeed = 1.0f;
GLfloat planeAcc = 0.2f;
GLfloat propellerAcc = 1.01;
GLfloat planeDeacc = 0.1f;
GLfloat planeRotation = 0.0f;
GLfloat planeYawRotation = 0.0f;
GLfloat planeTilt = 0.0f;
GLfloat propellerSpeed = -8.0f;

GLfloat planeRiseSpeed = 0.2f;
GLfloat planeFallSpeed = 0.2f;
GLfloat propRotation=0.0f;
GLuint axisId;
GLuint planeId;
GLuint propId;
GLuint targetId;

GLuint mountainId;
GLuint gridId;
GLuint seaId;
GLuint skyId;

GLuint mountainTexture;
GLuint seaTexture;
GLuint skyTexture;

GLfloat ambientLight0[] = { 0.5, 0.5, 0.5};
GLfloat diffuseLight0[] = { 1, 1, 1};
GLfloat specularLight0[] = { 1, 1, 1};
GLfloat lightPosition0[] = { 0, 0.3, -1, 0 };
GLfloat none[] ={0,0,0,0};
GLfloat dull[] = {1};
GLfloat littleshiny[] = {10};
GLfloat shiny[] = {50};
GLfloat littlespecular[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat darkgreen[] = { 0,1,0, 1.0 };
GLfloat lightgreen[] = { 0.6, 0.8, 0.5, 1.0 };
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat grey[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat yellow[] = { 0.8, 0.8, 0.0, 1.0 };
GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat blue[] = { 0.0, 0.0, 0.8, 1.0 };
GLfloat purple[] = { 0.7, 0.5, 0.9, 1.0 };
typedef struct Point {
	float x;
	float y;
	float z;
	float nx;
	float ny;
	float nz;
} Point;

Point calcNormal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);
void calcVertexNormal(float map[mountainDetailAccuracy][mountainDetailAccuracy], float jitterX[mountainDetailAccuracy][mountainDetailAccuracy], float jitterZ[mountainDetailAccuracy][mountainDetailAccuracy],int x, int z);
void raiseMountain(float map[mountainDetailAccuracy][mountainDetailAccuracy], int left, int right, int top, int bottom, int iteration);


typedef struct Bullet Bullet;
struct Bullet {
	float x;
	float y;
	float z;
	float rotation;
	float yaw;
	Bullet* nextBullet;
};
Bullet* firstBullet;
Bullet* currBullet;
GLuint numBullets;
GLuint currentBullet;
GLuint maxNumBullets;
GLfloat targetScale;
GLfloat bulletSpeed=4.0f;
#endif /* COMMON_H_ */
