/************************************************************************************

FlightSimulator.cpp

Adam Al-Jumaily
A00362836

*************************************************************************************/



/* include the library header files */
#include <stdlib.h>
#include <freeglut.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

/* Definitions */
#define PI 3.14159265
#define DEG_TO_RAD PI/180.0

vector<vector<float>> planeVertices = vector<vector<float>>();
vector<vector<float>> planeNormals = vector<vector<float>>();
vector<vector<int>> planePolygons = vector<vector<int>>();
vector<int> planeColorControl = vector<int>();

vector<vector<float>> propVertices = vector<vector<float>>();
vector<vector<float>> propNormals = vector<vector<float>>();
vector<vector<int>> propPolygons = vector<vector<int>>();
vector<int> propColorControl = vector<int>();


// reshape constants
const int originalWidth = 1500;
const int originalHeight = 1000;

// Scene control variables.
bool wireFrame = true;
bool fullScreen = false;

// Camera position vertex.
float cameraPosition[3] = {
	0, 100, -200
};

// Camera lookat vertex.
float lookAtPosition[3] = {
	0, 0, 0
};

/* 
	[0] X/Y angle.
	[1] Y/Z angle.
*/
float cameraAngles[2] = {
	0, 0
};

float planeSpeed = 3;

float propRotationAngle = 0;
float propRotationInc = 20;

// Increment that the XY lookat will increase by in degrees.
float degreeIncrementXY = 0;

// For drawing spheres.
GLUquadric *quad = gluNewQuadric();

// grid size constant. Number of quads.
const int GRID_SIZE = 100;


// directional light position array.
float lightPosition[] = { -1000, 250, 1000, 1 };
float lightDirection[] = { 1, -.25, -1 };

float zeroMaterial[] = { 0, 0, 0, 1 };
float whiteMaterial[] = { 1, 1, 1, 1 };
float redMaterial[] = { 1, 0, 0, 1 };
float greenMaterial[] = { 0, 1, 0, 1 };
float blueMaterial[] = { 0, 0, 1, 1 };
float yellowMaterial[] = { 1, 1, 0, 1 };
float purpleMaterial[] = { .86, .61, .86, 1 };
float greyMaterial[] = { .2, .2, .2, 1 };

/*
function: parseString

Reads in a string of four tokens delimited by spaces and
returns a pointer to a generated array.

*/
vector<string>* parseString(string line) {
	int lineIndex = 2;
	string substring;
	vector<string>* stringVec = new vector<string>;
	(*stringVec).push_back(string(1, line[0]));
	while(lineIndex > 0){
		substring = line.substr(lineIndex, line.find_first_of(" ", lineIndex) - lineIndex);
		(*stringVec).push_back(substring);
		lineIndex = line.find_first_of(" ", lineIndex) + 1;
	}
	return stringVec;
}

void initializePropellers() {
	vector<string>* splitLine = nullptr;
	string line;
	int numOfVertices = 0;
	int numOfPolygons = 0;
	int numOfNormals = 0;
	float largestX = 0;
	float smallestX = 0;
	float largestY = 0;
	float smallestY = 0;
	ifstream infile;
	infile.open("propeller.txt");

	if (!infile) {
		std::cout << "cant find it" << endl;
	}

	vector<float> tempVecFloat = vector<float>();
	vector<int> tempVecInt = vector<int>();

	// Fill vectors with vertices/normals/topology info.
	int colorCount = 0;
	while (!infile.eof()) {
		getline(infile, line);
		if (line.length() > 0) {
			splitLine = parseString(line);
			if ((*splitLine)[0] == "v") {
				for (int i = 1; i < (*splitLine).size(); i++) {
					tempVecFloat.push_back(stof((*splitLine)[i]) * 3);
				}
				propVertices.push_back(tempVecFloat);
				tempVecFloat.clear();
				numOfVertices++;
			}
			// propeller.txt file includes double spaces for polygon topology, need to remove them.
			else if ((*splitLine)[0] == "f") {
				for (int i = 1; i < (*splitLine).size(); i++) {
					if ((*splitLine)[i] != "") {
						tempVecInt.push_back(stoi((*splitLine)[i]) - 1);
					}
				}
				propPolygons.push_back(tempVecInt);
				tempVecInt.clear();
				numOfPolygons++;
				colorCount++;
			}
			else if ((*splitLine)[0] == "n") {
				for (int i = 1; i < (*splitLine).size(); i++) {
					tempVecFloat.push_back(stof((*splitLine)[i]));
				}
				propNormals.push_back(tempVecFloat);
				tempVecFloat.clear();
				numOfNormals++;
			}
			else if ((*splitLine)[0] == "g") {
				if (colorCount != 0) {
					propColorControl.push_back(colorCount);
					colorCount = 0;
				}
			}
			delete splitLine;
		}
	}
	propColorControl.push_back(colorCount);
	infile.close();

}

/*
function initializePlane

Reads in from the plane file and fills a vector with information about
vertex location and mesh topology.
*/
void initializePlane() {
	vector<string>* splitLine = nullptr;
	string line;
	int numOfVertices = 0;
	int numOfPolygons = 0;
	int numOfNormals = 0;
	ifstream infile;
	infile.open("cessna.txt");

	if (!infile) {
		std::cout << "cant find it" << endl;
	}

	vector<float> tempVecFloat = vector<float>();
	vector<int> tempVecInt = vector<int>();

	// Fill vectors with vertices/normals/topology info.
	int colorCount = 0;
	while (!infile.eof()) {
		getline(infile, line);
		if (line.length() > 0) {
			splitLine = parseString(line);
			if ((*splitLine)[0] == "v") {
				for (int i = 1; i < (*splitLine).size(); i++) {
					tempVecFloat.push_back(stof((*splitLine)[i]) * 3);
				}
				planeVertices.push_back(tempVecFloat);
				tempVecFloat.clear();
				numOfVertices++;
			}
			// Cessna.txt file includes double spaces for polygon topology, need to remove them.
			else if ((*splitLine)[0] == "f") {
				for (int i = 1; i < (*splitLine).size(); i++) {
					if ((*splitLine)[i] != "") {
						tempVecInt.push_back(stoi((*splitLine)[i])-1);
					}
				}
				planePolygons.push_back(tempVecInt);
				tempVecInt.clear();
				numOfPolygons++;
				colorCount++;
			}
			else if ((*splitLine)[0] == "n") {
				for (int i = 1; i < (*splitLine).size(); i++) {
					tempVecFloat.push_back(stof((*splitLine)[i]));
				}
				planeNormals.push_back(tempVecFloat);
				tempVecFloat.clear();
				numOfNormals++;
			}
			else if ((*splitLine)[0] == "g") {
				if (colorCount != 0) {
					planeColorControl.push_back(colorCount);
					colorCount = 0;
				}
			}
			delete splitLine;
		}
	}
	planeColorControl.push_back(colorCount);
	infile.close();


}


/*
function: changeCameraPos

linearly modifies the camera and lookat positions.
*/
void changeCameraPos(float posX, float posY, float posZ, float lookX, float lookY, float lookZ) {
	cameraPosition[0] += posX;
	cameraPosition[1] += posY;
	cameraPosition[2] += posZ;

	lookAtPosition[0] += lookX;
	lookAtPosition[1] += lookY;
	lookAtPosition[2] += lookZ;
}


void drawPlane() {
	int polygonCount = 0;
	int polygonSubsetIndex = 0;
	int colorIndex = 0;
	for (int i = 0; i < planePolygons.size(); i++) {
		if (polygonCount == planeColorControl[polygonSubsetIndex]) {
			polygonSubsetIndex++;
			polygonCount = 0;
		}
			if (polygonSubsetIndex < 3) {
				glMaterialfv(GL_FRONT, GL_AMBIENT, yellowMaterial);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, yellowMaterial);
				glMaterialfv(GL_FRONT, GL_SPECULAR, yellowMaterial);
			}
			else if (polygonSubsetIndex > 3 && polygonSubsetIndex < 6) {
				glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, zeroMaterial);
				glMaterialfv(GL_FRONT, GL_SPECULAR, zeroMaterial);
			}
			else if (polygonSubsetIndex == 6) {
				glMaterialfv(GL_FRONT, GL_AMBIENT, purpleMaterial);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, purpleMaterial);
				glMaterialfv(GL_FRONT, GL_SPECULAR, zeroMaterial);
			}
			else if (polygonSubsetIndex == 7) {
				glMaterialfv(GL_FRONT, GL_AMBIENT, blueMaterial);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, blueMaterial);
				glMaterialfv(GL_FRONT, GL_SPECULAR, blueMaterial);
			}
			else if (polygonSubsetIndex > 7 && polygonSubsetIndex < 14) {
				glMaterialfv(GL_FRONT, GL_AMBIENT, yellowMaterial);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, yellowMaterial);
				glMaterialfv(GL_FRONT, GL_SPECULAR, yellowMaterial);
			}
			else if (polygonSubsetIndex > 13 && polygonSubsetIndex < 26) {
				glMaterialfv(GL_FRONT, GL_AMBIENT, blueMaterial);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, blueMaterial);
				glMaterialfv(GL_FRONT, GL_SPECULAR, blueMaterial);
			}
			else if (polygonSubsetIndex > 25) {
				glMaterialfv(GL_FRONT, GL_AMBIENT, yellowMaterial);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, yellowMaterial);
				glMaterialfv(GL_FRONT, GL_SPECULAR, yellowMaterial);
			}
		
		//glColor3fv(planeColors[colorIndex]);
		glBegin(GL_POLYGON);
		for (int j = 0; j < planePolygons[i].size(); j++) {
			glNormal3f(-planeNormals[planePolygons[i][j]][2],
				planeNormals[planePolygons[i][j]][1],
				-planeNormals[planePolygons[i][j]][0]);
			
			glVertex3f(-planeVertices[planePolygons[i][j]][2] + cameraPosition[0],
				planeVertices[planePolygons[i][j]][1] + cameraPosition[1] - 2,
				-planeVertices[planePolygons[i][j]][0] + cameraPosition[2] + 4);

		}
		glEnd();
		polygonCount++;
	}
}

void drawPropeller1() {
	int polygonCount = 0;
	int polygonSubsetIndex = 0;
	int colorIndex = 0;
	for (int i = 0; i < propPolygons.size(); i++) {
		if (polygonCount == propColorControl[polygonSubsetIndex]) {
			colorIndex++;
		}
		if (colorIndex == 0) {
			glMaterialfv(GL_FRONT, GL_AMBIENT, whiteMaterial);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, whiteMaterial);
			glMaterialfv(GL_FRONT, GL_SPECULAR, whiteMaterial);
		}
		else {
			glMaterialfv(GL_FRONT, GL_AMBIENT, redMaterial);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, redMaterial);
			glMaterialfv(GL_FRONT, GL_SPECULAR, redMaterial);
		}
		glPushMatrix();
		glTranslatef(cameraPosition[0] - 1.05, cameraPosition[1] - 2.45, cameraPosition[2] + 4);
		glRotatef(propRotationAngle, 0, 0, 1);
		glTranslatef(-cameraPosition[0] + 1.05, -cameraPosition[1] + 2.45, -cameraPosition[2] - 4);
		glBegin(GL_POLYGON);
		for (int j = 0; j < propPolygons[i].size(); j++) {
			glNormal3f(-propNormals[propPolygons[i][j]][2],
				propNormals[propPolygons[i][j]][1],
				-propNormals[propPolygons[i][j]][0]);
			glVertex3f(-propVertices[propPolygons[i][j]][2] + cameraPosition[0],
				propVertices[propPolygons[i][j]][1] + cameraPosition[1] - 2,
				-propVertices[propPolygons[i][j]][0] + cameraPosition[2] + 4);
		}
		glEnd();
		glPopMatrix();
		polygonCount++;
	}
}

void drawPropeller2() {
	int polygonCount = 0;
	int polygonSubsetIndex = 0;
	int colorIndex = 0;
	for (int i = 0; i < propPolygons.size(); i++) {
		if (polygonCount == propColorControl[polygonSubsetIndex]) {
			colorIndex++;
		}
		if (colorIndex == 0) {
			glMaterialfv(GL_FRONT, GL_AMBIENT, whiteMaterial);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, whiteMaterial);
			glMaterialfv(GL_FRONT, GL_SPECULAR, whiteMaterial);
		}
		else {
			glMaterialfv(GL_FRONT, GL_AMBIENT, redMaterial);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, redMaterial);
			glMaterialfv(GL_FRONT, GL_SPECULAR, redMaterial);
		}
		glPushMatrix();
		glTranslatef(cameraPosition[0] + 1.05, cameraPosition[1] - 2.45, cameraPosition[2] + 4);
		glRotatef(propRotationAngle, 0, 0, 1);
		glTranslatef(-cameraPosition[0] - 1.05, -cameraPosition[1] + 2.45, -cameraPosition[2] - 4);
		glBegin(GL_POLYGON);
		for (int j = 0; j < propPolygons[i].size(); j++) {
			glNormal3f(-propNormals[propPolygons[i][j]][2],
				propNormals[propPolygons[i][j]][1],
				-propNormals[propPolygons[i][j]][0]);
			glVertex3f(propVertices[propPolygons[i][j]][2] + cameraPosition[0],
				propVertices[propPolygons[i][j]][1] + cameraPosition[1] - 2,
				-propVertices[propPolygons[i][j]][0] + cameraPosition[2] + 4);
		}
		glEnd();
		glPopMatrix();
		polygonCount++;
	}
}



void drawQuadGrid() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, greyMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, greyMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, greyMaterial);

	glNormal3d(0, 1, 0);
	for (int j = -(GRID_SIZE/2); j < (GRID_SIZE / 2); j++) {
		glBegin(GL_QUAD_STRIP);
		for (int i = -(GRID_SIZE / 2); i < (GRID_SIZE / 2); i++) {
			glVertex3f(i * 20, 0, 20*j);
			glVertex3f(i * 20, 0, 20*j -20);
		}
		glEnd();
	}
}


void drawAxis() {
	glLineWidth(3);
	glMaterialfv(GL_FRONT, GL_AMBIENT, whiteMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, whiteMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, whiteMaterial);
	gluSphere(quad, 4, 50, 50);
	glBegin(GL_LINES);
	glMaterialfv(GL_FRONT, GL_AMBIENT, redMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, redMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, redMaterial);
	glVertex3f(0, 0, 0);
	glVertex3f(100, 0, 0);
	glMaterialfv(GL_FRONT, GL_AMBIENT, greenMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, greenMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, greenMaterial);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 100, 0);
	glMaterialfv(GL_FRONT, GL_AMBIENT, blueMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blueMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blueMaterial);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 100);
	glEnd();
	glLineWidth(1);
}


void myDisplay(void)
{

	// clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// load the identity matrix into the model view matrix
	glLoadIdentity();

	// set the camera position
	gluLookAt(
		cameraPosition[0], cameraPosition[1], cameraPosition[2],
		lookAtPosition[0], lookAtPosition[1], lookAtPosition[2],
		0, 1, 0);

	glPushMatrix();
	glTranslatef(cameraPosition[0], cameraPosition[1] - 2.5, cameraPosition[2]);
	glRotatef(-degreeIncrementXY * 30, 0, 0, 1);
	glTranslatef(-cameraPosition[0], -cameraPosition[1] + 2.5, -cameraPosition[2]);
	drawPlane();
	drawPropeller1();
	drawPropeller2();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(cameraPosition[0], cameraPosition[1], cameraPosition[2]);
	glRotatef(-cameraAngles[0], 0, 1, 0);
	glTranslatef(-cameraPosition[0], -cameraPosition[1], -cameraPosition[2]);
	if (wireFrame) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	drawQuadGrid();
	glTranslatef(0, 5, 0);
	glPushMatrix();
	drawAxis();
	glPopMatrix();
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180);
	glPopMatrix();


	// swap the drawing buffers
	glutSwapBuffers();
}

/*
function: mouseMove

passive mouse callback function.
Used to look around the scene with the mouse.
*/
void mouseMove(int x, int y) {
	float screenWidth = glutGet(GLUT_WINDOW_WIDTH);
	float centerScreen = screenWidth / 2;

	if (x > centerScreen) {
		degreeIncrementXY = -(((float)x - centerScreen) / 500);
	}
	else if (x < centerScreen) {
		degreeIncrementXY = ((centerScreen - (float)x) / 500);
	}
	glutPostRedisplay();
}

/*
function: specialInput

callback function for special key presses.
Used for camera control from the keyboard.
*/
void specialInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_PAGE_UP:
		planeSpeed += 0.1;
		break;
	case GLUT_KEY_PAGE_DOWN:
		planeSpeed -= 0.1;
		break;
	case GLUT_KEY_UP:
		changeCameraPos(0, 5, 0, 0, 5, 0);
		break;
	case GLUT_KEY_DOWN:
		changeCameraPos(0, -5, 0, 0, -5, 0);
		break;
	}

	glutPostRedisplay();
}

/*
function: keyboard

callback function for key presses.
Used for scene control from the keyboard.
*/
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'W':
	case 'w':
		wireFrame = !wireFrame;
		break;
	case 'F':
	case 'f':
		if (fullScreen) {
			glViewport(0, 0, originalWidth, originalHeight);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(90, (float)originalWidth / (float)originalHeight, 0.01, 5000);
			glMatrixMode(GL_MODELVIEW);
			fullScreen = !fullScreen;
			glutLeaveFullScreen();
		}
		else {
			glutFullScreen();
			fullScreen = !fullScreen;
		}
		break;
	case 'Q':
	case 'q':
		exit(0);
	}
	glutPostRedisplay();
}

void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, (float)width / (float)height, 0.01, 5000);
	glMatrixMode(GL_MODELVIEW);
}

void idle() {
	cameraAngles[0] += degreeIncrementXY;
	if (cameraAngles[0] > 360) {
		cameraAngles[0] = fmod(cameraAngles[0], 360);
	}
	else if (cameraAngles[0] < 0) {
		cameraAngles[0] = 360 + cameraAngles[0];
	}
	cameraPosition[0] += planeSpeed*sin(DEG_TO_RAD*cameraAngles[0]);
	lookAtPosition[0] += planeSpeed*sin(DEG_TO_RAD*cameraAngles[0]);
	cameraPosition[2] += planeSpeed*cos(DEG_TO_RAD*cameraAngles[0]);
	lookAtPosition[2] += planeSpeed*cos(DEG_TO_RAD*cameraAngles[0]);

	propRotationInc = 20 + planeSpeed * 2;
	if (propRotationInc > 178) {
		propRotationInc = 178;
	}
	propRotationAngle += propRotationInc;
	if (propRotationAngle >= 360) {
		propRotationAngle = 0;
	}

	glutPostRedisplay();
}


/************************************************************************

Function:		initializeGL

Description:	Initializes the OpenGL rendering context for display.

*************************************************************************/
void initializeGL(void)
{

	// define the light color and intensity
	GLfloat ambientLight[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat diffuseLight[] = { 1, 1, 1, 1 };
	GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 };

	//  the global ambient light level
	GLfloat globalAmbientLight[] = { 0.4, 0.4, 0.4, 1.0 };

	// set the global ambient light level
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLight);

	// define the color and intensity for light 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, diffuseLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, specularLight);

	// enable lighting 
	glEnable(GL_LIGHTING);
	// enable light 0
	glEnable(GL_LIGHT0);

	// enable smooth shading.
	glShadeModel(GL_SMOOTH);

	// make sure the normals are unit vectors
	glEnable(GL_NORMALIZE);

	// Initialize plane vectors.
	initializePlane();

	// Initialize propeller.
	initializePropellers();

	// enable depth testing
	glEnable(GL_DEPTH_TEST);

	// set background color to be black
	glClearColor(0, 0, 0, 1.0);

	// change into projection mode so that we can change the camera properties
	glMatrixMode(GL_PROJECTION);

	// load the identity matrix into the projection matrix
	glLoadIdentity();

	// set window mode to 2D orthographic 
	gluPerspective(90, originalWidth/originalHeight, 0.01, 5000);

	// change into model-view mode so that we can change the object positions
	glMatrixMode(GL_MODELVIEW);
}

/************************************************************************

Function:		main

Description:	Sets up the openGL rendering context and the windowing
system, then begins the display loop.

*************************************************************************/
void main(int argc, char** argv)
{
	// initialize the toolkit
	glutInit(&argc, argv);
	// set display mode
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	// set window size
	glutInitWindowSize(originalWidth, originalHeight);
	// set window position on screen
	glutInitWindowPosition(100, 0);
	// open the screen window
	glutCreateWindow("Flight Simulator - A00362836");
	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// register redraw function
	glutDisplayFunc(myDisplay);
	// register special function.
	glutSpecialFunc(specialInput);
	// register passive motion function.
	glutPassiveMotionFunc(mouseMove);
	// register Idle function.
	glutIdleFunc(idle);
	// register keyboard function.
	glutKeyboardFunc(keyboard);
	// register reshape function.
	glutReshapeFunc(reshape);
	// initialize the rendering context
	initializeGL();
	// seed random
	srand(time(NULL));

	// go into a perpetual loop
	glutMainLoop();
}

