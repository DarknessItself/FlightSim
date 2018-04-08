/**
 * @author	: Matthew Thomas, mthomas@cs.dal.ca, B00558461
 * CSCI 3161 Project: Flight Sim
 * Creates an area to fly around with in a propeller based plane.
 * Generates 3 randomly generated islands.
 * Islands used midpoints and some "jitter" to make the peaks not always straight up
 * Generates normals for the mountains per vertex.
 * w - toggles between frame and solid rendering
 * f - toggles between fullscreen and windowed mode
 * b - toggle fog on or off
 * s - toggle between grid and sea+sky
 * m - toggles mountains on and off
 * t - toggles textures for mountains on and off
 * q - quit
 *
 * Standard controls:
 * 	Page Up - Increase Speed
 * 	Page Down - Decrease Speed
 * 	Up - Raise elevation of plane
 * 	Down - Lower elevation of plane
 * 	mouse on left side of screen - Turn plane to the left
 * 	mouse on right side of screen - Turn plane to the right
 *
 * Bonuses:
 * 	f1 - toggle between standard and alternate controls
 * 	f2 - toggle between standard fog and cloud-like fog
 * 	z - shoot bullets
 * 	Spedometer (Shows speed roughly, green bar bottom left)
 * 	Altometer (Shows altitude roughly, blue bar bottom right)
 * 	Crash into the sea and there is an explosion + plane dies
 * 	r - respawn and generate new random world
 *	crosshair
 *
 * Alternate controls:
 *  mouse on top of screen - Tilt plane and camera up
 *  mouse on bottom of screen - Title plane and camera down
 *  scroll wheel - increase or decrease speed
 */

#include "OGLFlightSim.h"
#include "Common.h"
/**
 * main
 * The main method initially run. Creates the context of the application and begins the display loop.
 */
int main(int argc, char** argv) {
	init(argc, argv);
	printf(" * w - toggles between frame and solid rendering\n");
	printf(" * f - toggles between fullscreen and windowed mode\n");
	printf(" * b - toggle fog on or off\n");
	printf(" * s - toggle between grid and sea+sky\n");
	printf(" * m - toggles mountains on and off\n");
	printf(" * t - toggles textures for mountains on and off\n");
	printf(" * q - quit\n");
	printf(" * Standard controls:\n");
	printf(" *  Page Up - Increase Speed\n");
	printf(" *  Page Down - Decrease Speed\n");
	printf(" *  Up - Raise elevation of plane\n");
	printf(" *  Down - Lower elevation of plane\n");
	printf(" *  mouse on left side of screen - Turn plane to the left\n");
	printf(" *  mouse on right side of screen - Turn plane to the right\n");
	printf(" * Bonuses:\n");
	printf(" *  f1 - toggle between standard and alternate controls\n");
	printf(" *  f2 - toggle between standard fog and cloud-like fog\n");
	printf(" *  z - shoot bullets\n");
	printf(" *  Spedometer (Shows speed roughly, green bar bottom left)\n");
	printf(" *  Altometer (Shows altitude roughly, blue bar bottom right)\n");
	printf(" *  Crash into the sea and there is an explosion + plane dies\n");
	printf(" *  r - respawn and generate new random world\n");
	printf(" *  crosshair\n");
	printf(" * Alternate controls:\n");
	printf(" *  mouse on top of screen - Tilt plane and camera up\n");
	printf(" *  mouse on bottom of screen - Title plane and camera down\n");
	printf(" *  scroll wheel - increase or decrease speed\n");
	glutMainLoop();
	return 0;
}
/**
 * init
 * Initializes all of the components for the application.
 * Displays the controls in standard out.
 */
void init(int argc, char** argv) {
	float seed = time(NULL);
	//printf("Seed: %f\n", seed);

	srand(seed);
	initGLUT(argc, argv);
	initNew();
	initGL();
}
/**
 * initNew
 * Initializes a new world
 */
void initNew() {
	numBullets = 0;
	maxNumBullets = 100;
	currentBullet = 0;
	targetScale = 1;
	firstBullet = NULL;
	currBullet = NULL;
	initTextures();
	initSea();
	initSky();
	initTarget();
	initGrid();
	initAxis();
	initMountains();
	planeId = loadModel(planeId, 6764, "./resources/cessna.txt", 1);
	propId = loadModel(propId, 6764, "./resources/propeller.txt", 2);
	initLight();

}
/**
 * initLight
 * Initializes the world's light.
 */
void initLight() {

	// Set lighting values, self explanatory
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);

	glShadeModel(GL_SMOOTH);
	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat diffuseMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat ambientMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat specularMaterial[4] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat emissiveMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mShininess[] = { 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
}

/**
 * colorMountainByHeight
 * Colors a mountain vertex based on its height.
 */
void colorMountainByHeight(float x, float y, float z,
		float mountainDetailAccuracy) {

	GLfloat diffuseMaterial[] = { 0.0, 0.0, 0.0, 1.0 };
	if (y < 7) {
		diffuseMaterial[0] = darkgreen[0];
		diffuseMaterial[1] = darkgreen[1];
		diffuseMaterial[2] = darkgreen[2];
		diffuseMaterial[3] = darkgreen[3];
	} else if (y < 15) {
		diffuseMaterial[0] = lightgreen[0];
		diffuseMaterial[1] = lightgreen[1];
		diffuseMaterial[2] = lightgreen[2];
		diffuseMaterial[3] = lightgreen[3];
	} else {
		diffuseMaterial[0] = white[0];
		diffuseMaterial[1] = white[1];
		diffuseMaterial[2] = white[2];
		diffuseMaterial[3] = white[3];
	}
	glColor4f(diffuseMaterial[0], diffuseMaterial[1], diffuseMaterial[2],
			diffuseMaterial[3]);
}
/**
 * raiseMountain
 * Recurssive function to randomize the heights of a heightmap using midpoints.
 *
 */
void raiseMountain(float map[mountainDetailAccuracy][mountainDetailAccuracy],
		int left, int right, int top, int bottom, int iteration) {
	if (iteration >= 8) {
		return;
	}
	int width = right - left;
	int height = bottom - top;
	map[left + (width / 2)][top + (height / 2)] += randBetween(0, 4)
			/ (2.0f * iteration);
	map[left][top + (height / 2)] += randBetween(0, 4) / (2.0f * iteration);
	map[left + (width / 2)][top] += randBetween(0, 4) / (2.0f * iteration);
	map[left + (width - 1)][top + (height / 2)] += randBetween(0, 4)
			/ (2.0f * iteration);
	map[left + (width / 2)][top + (height - 1)] += randBetween(0, 4)
			/ (2.0f * iteration);

	iteration++;

	raiseMountain(map, left, left + (width / 2), top, bottom - (height / 2),
			iteration);
	raiseMountain(map, left + (width / 2), right, top, bottom - (height / 2),
			iteration);
	raiseMountain(map, left, left + (width / 2), top + (height / 2), bottom,
			iteration);
	raiseMountain(map, left + (width / 2), right, top + (height / 2), bottom,
			iteration);

}
/**
 * calcNormal
 * Calculates the normal of a polygon using the cross product.
 */
Point calcNormal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) {
	float ax, ay, az;
	float bx, by, bz;
	float nx, ny, nz;
	ax = x2 - x1;
	ay = y2 - y1;
	az = z2 - z1;

	bx = x3 - x1;
	by = y3 - y1;
	bz = z3 - z1;

	nx = (ay * bz) - (az * by);
	ny = (az * bx) - (ax * bz);
	nz = (ax * by) - (ay * bx);

	float length = sqrt(pow(nx,2)+pow(ny,2)+pow(nz,2));
	nx /= length;
	ny /= length;
	nz /= length;
	Point normal = { x1, y1, z1, nx, ny, nz };
	return normal;
}
/**
 * calcVertexNormal
 * Calculates the normal for a vertex using the normals of polygons around it.
 */
void calcVertexNormal(float map[mountainDetailAccuracy][mountainDetailAccuracy], float jitterX[mountainDetailAccuracy][mountainDetailAccuracy], float jitterZ[mountainDetailAccuracy][mountainDetailAccuracy], int x, int z) {
	Point topleft = calcNormal(x+jitterX[x][z], map[x][z], z+jitterZ[x][z], //
			x+jitterX[x][z-1], map[x][z - 1], z - 1 +jitterZ[x][z-1], //
			x - 1+jitterX[x-1][z], map[x - 1][z], z+jitterZ[x-1][z]);
	Point topright = calcNormal(x+jitterX[x][z], map[x][z], z+jitterZ[x][z], //
			x + 1+jitterX[x+1][z], map[x + 1][z], z+jitterZ[x+1][z], //
			x+jitterX[x][z-1], map[x][z - 1], z - 1+jitterZ[x][z-1]);
	Point bottomleft = calcNormal(x+jitterX[x][z], map[x][z], z+jitterZ[x][z], //
			x+jitterX[x][z+1], map[x][z + 1], z + 1+jitterZ[x][z+1], //
			x - 1+jitterX[x-1][z], map[x - 1][z], z+jitterZ[x-1][z]);
	Point bottomright = calcNormal(x+jitterX[x][z], map[x][z], z+jitterZ[x][z], //
			x+jitterX[x][z+1], map[x][z + 1], z + 1+jitterZ[x][z+1], //
			x + 1+jitterX[x+1][z+1], map[x + 1][z], z+jitterZ[x+1][z]);
	float nx = (topleft.nx + topright.nx + bottomleft.nx + bottomright.nx)
			/ 4.0f;
	float ny = (topleft.ny + topright.ny + bottomleft.ny + bottomright.ny)
			/ 4.0f;
	float nz = (topleft.nz + topright.nz + bottomleft.nz + bottomright.nz)
			/ 4.0f;
	glNormal3f(nx, ny, nz);
}
/**
 * drawMountain
 * Draws a randomly generated mountain
 */
void drawMountain(float mountainDetailAccuracy, int maxIterations) {
	int x, z;
	int mapSize = (int) mountainDetailAccuracy;
	float map[mapSize][mapSize];
	float jitterX[mapSize][mapSize];
	float jitterZ[mapSize][mapSize];
	//initialize the heights to be in a cone-like shape based on distance from center
	for (x = 0; x < mapSize; x++) {
		for (z = 0; z < mapSize; z++) {
			float distance =
					sqrt((pow((mapSize/2)-x,2))+(pow((mapSize/2)-z,2)))
							* 0.9f;
			map[x][z] = ((mapSize / 2) - distance) / 2.0f;
			//generate some random jitter to be applied to x and z values when drawing (so peaks aren't always straight up)
			jitterX[x][z] = (randBetween(0, 200) - 100) / 200.0f;
			jitterZ[x][z] = (randBetween(0, 200) - 100) / 200.0f;
			if (map[x][z] < 0) {
				map[x][z] = 0;
			}
		}
	}
	//recussively raise the mountain to give it peaks and valleys, ignore the edges
	raiseMountain(map, 1, mapSize - 1, 1, mapSize - 1, 1);

	//change the outeredge to always be flat on the ground (so their normals will be 0, 1, 0
	int i;
	for (i = 0; i < mapSize; i++) {
		map[i][0] = 0.0;
		map[i][mapSize - 1] = 0.0;
		map[0][i] = 0.0;
		map[mapSize - 1][i] = 0.0;
	}

	//draw the mountain
	for (x = 0; x < mountainDetailAccuracy - 1; x++) {
		for (z = 0; z < mountainDetailAccuracy - 1; z++) {
			glBegin(GL_POLYGON);

			//be default the normal is straight up (edges)
			glNormal3f(0, 1, 0);

			//if it's not an edge then calculate the normal for the point
			if (x != 0 && z != 0 && x != mountainDetailAccuracy - 1
					&& z != mountainDetailAccuracy - 1) {
				calcVertexNormal(map, jitterX, jitterZ, x, z + 1);
			}
			//color the mountain for when not using textures
			colorMountainByHeight(x, map[x][z + 1], z + 1,
					mountainDetailAccuracy);
			//map coords of texture
			glTexCoord2f((x / mountainDetailAccuracy),
					((z + 1) / mountainDetailAccuracy));
			//draw vertex
			glVertex3f(x + jitterX[x][z + 1], map[x][z + 1],
					z + 1 + jitterZ[x][z + 1]);

			//repeat for other 3 points of tile/quad/square

			if (x != 0 && z != 0 && x != mountainDetailAccuracy - 1
					&& z != mountainDetailAccuracy - 1) {
				calcVertexNormal(map, jitterX, jitterZ, x + 1, z + 1);
			}
			colorMountainByHeight(x + 1, map[x + 1][z + 1], z + 1,
					mountainDetailAccuracy);

			glTexCoord2f(((x + 1) / mountainDetailAccuracy),
					((z + 1) / mountainDetailAccuracy));
			glVertex3f(x + 1 + jitterX[x + 1][z + 1], map[x + 1][z + 1],
					z + 1 + jitterZ[x + 1][z + 1]);

			if (x != 0 && z != 0 && x != mountainDetailAccuracy - 1
					&& z != mountainDetailAccuracy - 1) {
				calcVertexNormal(map, jitterX, jitterZ, x + 1, z);
			}
			colorMountainByHeight(x + 1, map[x + 1][z], z,
					mountainDetailAccuracy);

			glTexCoord2f(((x + 1) / mountainDetailAccuracy),
					(z / mountainDetailAccuracy));
			glVertex3f(x + 1 + jitterX[x + 1][z], map[x + 1][z],
					z + jitterZ[x + 1][z]);

			if (x != 0 && z != 0 && x != mountainDetailAccuracy - 1
					&& z != mountainDetailAccuracy - 1) {
				calcVertexNormal(map, jitterX, jitterZ, x, z);
			}
			glTexCoord2f((x / mountainDetailAccuracy),
					(z / mountainDetailAccuracy));
			glVertex3f(x + jitterX[x][z], map[x][z], z + jitterZ[x][z]);
			glEnd();
		}
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

/**
 * initMountains
 * Initializes the mountains including position and scale
 */
void initMountains() {
	mountainId = glGenLists(1);
	glNewList(mountainId, GL_COMPILE);

	glBindTexture(GL_TEXTURE_2D, mountainTexture);

	glColor4f(1, 1, 1, 1);
	int i;
	int numMountains = 3;
	//generate a certain number of mountains
	for (i = 0; i < numMountains; i++) {
		glPushMatrix();
		float angle = (90 * i);
		float distance = 80;

		//move it down so edges are below sea
		glTranslatef(0, -2.5f, 0);
		glScalef(0.5f, 0.5f, 0.5f);
		glScalef(randBetween(20, 150) / 100.0f, randBetween(20, 150) / 100.0f,
				randBetween(20, 150) / 100.0f);

		glTranslatef(sin(angle) * distance, 0, cos(angle) * distance);
		glTranslatef(mountainDetailAccuracy / -2.0f, 0,
				mountainDetailAccuracy / -2.0f);
		int accuracy = mountainDetailAccuracy;
		drawMountain(accuracy, 4);

		glPopMatrix();
	}
	glEndList();
}

/**
 * drawMountains
 * Draws all of the mountains with or without textures based on texture.
 */
void drawMountains() {
	glPushMatrix();
	glDisable( GL_TEXTURE_2D);
	if (toggleMountainTextures == 1) {
		glEnable( GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mountainTexture);
	}
	glColor4f(1, 1, 1, 1);
	glDisable(GL_CULL_FACE);
	glCallList(mountainId);
	glEnable(GL_CULL_FACE);
	glPopMatrix();
}
/**
 * initAxis
 * Initializes the axis marker showing x,y,z axis
 */
void initAxis() {
	float lineLength = 7.0f;
	axisId = glGenLists(1);
	glNewList(axisId, GL_COMPILE);
	glBegin(GL_LINES);
	//x red
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(lineLength, 0, 0);
	//y green
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, lineLength, 0);
	//z blue
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, lineLength);
	glEnd();
	//center sphere
	glColor3f(1, 1, 1);
	glRotatef(90, 1, 0, 0);
	glutSolidSphere(0.6, 16, 16);
	glEndList();
}
/**
 * drawCircle
 * Draws a circle.
 */
void drawCircle() {
	float numPoints = 120;
	double angleStep = (2 * 3.14) / numPoints;
	glBegin(GL_LINE_STRIP);
	double angle = 0.0;
	int i;
	for (i = 0; i <= numPoints; i++) {
		glVertex2f(sin(angle), cos(angle));
		angle += angleStep;
	}
	glEnd();
}
/**
 * initTarget
 * Initializes the target/crosshair
 */
void initTarget() {
	float lineLength = 1.0f;
	targetId = glGenLists(1);
	glNewList(targetId, GL_COMPILE);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	//y
	glVertex3f(0, -lineLength / 2.0f, 0);
	glVertex3f(0, lineLength / 2.0f, 0);
	//z
	glVertex3f(0, 0, -lineLength / 2.0f);
	glVertex3f(0, 0, lineLength / 2.0f);
	glEnd();
	glRotatef(90, 0, 1, 0);
	glScalef(0.3, 0.3, 1);
	drawCircle();
	glEndList();
}
/*
 * drawTarget
 * Draws the target/crosshair.
 */
void drawTarget() {
	glPushMatrix();
	glTranslatef(-10, 0, 0);

	if (boolShoot == 1) {
		targetScale *= 1.3f;
		if (targetScale > 1.5) {
			targetScale = 1.5;
		}
	} else if (targetScale > 1.0f) {
		targetScale *= 0.5f;
	}

	glScalef(targetScale, targetScale, targetScale);
	glCallList(targetId);
	glPopMatrix();
}
/**
 * randBetween
 * Returns a random number between the given min and max.
 */
float randBetween(int min, int max) {
	if (max <= min) {
		return min;
	}
	return ((rand() % (max - min)) + min);
}
/**
 * initSky
 * Initializes the sky.
 */
void initSky() {
	skyObj = gluNewQuadric();
	skyId = glGenLists(4);
	gluQuadricNormals(skyObj, GLU_SMOOTH);
	gluQuadricTexture(skyObj, GL_TRUE);
	//texture the sky
	glBindTexture(GL_TEXTURE_2D, skyTexture);
	gluQuadricTexture(skyObj, skyTexture);

	glNewList(skyId, GL_COMPILE);
	glTranslatef(0, (seaDetailAccuracy * 2) - 2.0f, 0);
	glRotatef(90, 1, 0, 0);

	GLfloat diffuseMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat ambientMaterial[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mShininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
	gluCylinder(skyObj, seaDetailAccuracy, seaDetailAccuracy,
			seaDetailAccuracy * 2, 100, 100);

	glRotatef(-180, 0, 1, 0);

	glScalef(1.01, 1, 1.01);
	gluCylinder(skyObj, seaDetailAccuracy, seaDetailAccuracy,
			(seaDetailAccuracy * 1.9f), 100, 100);
	glEndList();
}
/**
 * initSea
 * Initializes the sea
 */
void initSea() {
	seaObj = gluNewQuadric();
	seaId = glGenLists(1);
	gluQuadricNormals(seaObj, GLU_SMOOTH);
	gluQuadricTexture(seaObj, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, seaTexture);
	gluQuadricTexture(seaObj, seaTexture);

	glNewList(seaId, GL_COMPILE);
	glRotatef(270, 1, 0, 0);
	glColor4f(0.0, 0.0, 0.6, 1.0);
	GLfloat ambientMaterial[4] = { 0.2, 0.2, 0.4, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
	gluDisk(seaObj, 0, seaDetailAccuracy + 2, 64, 64);
	glEndList();
}
/**
 * initGrid
 * Initializes the grid.
 */
void initGrid() {
	gridId = glGenLists(1);
	glNewList(gridId, GL_COMPILE);
	float x, y, z;
	y = -0.5f;
	GLfloat diffuseMaterial[4] = { 0.3, 0.3, 0.3, 1.0 };
	glBegin(GL_QUADS);
	for (x = 0; x < gridSize; x++) {
		for (z = 0; z < gridSize; z++) {
			glColor4f(diffuseMaterial[0], diffuseMaterial[1],
					diffuseMaterial[2], diffuseMaterial[3]);
			glNormal3f(0, 1, 0);

			glTexCoord2f((x / gridSize), ((z + 1) / gridSize));
			glVertex3f(x, y, z + 1);

			glTexCoord2f(((x + 1) / gridSize), ((z + 1) / gridSize));
			glVertex3f(x + 1, y, z + 1);

			glTexCoord2f(((x + 1) / gridSize), (z / gridSize));
			glVertex3f(x + 1, y, z);

			glTexCoord2f((x / gridSize), (z / gridSize));
			glVertex3f(x, y, z);

		}
	}
	glEnd();
	glEndList();

}
/**
 * drawGrid
 * Draws the grid.
 */
void drawGrid() {
	glTranslatef(0, -0.5f, 0);
	glEnable(GL_COLOR_MATERIAL);
	// set material properties which will be assigned by glColor
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glCallList(gridId);
	glDisable(GL_COLOR_MATERIAL);
}
/**
 * drawSea
 */
void drawSea() {
	glCallList(seaId);
}
/**
 * drawSky
 */
void drawSky() {
	glCallList(skyId);
}
/**
 * drawPlane
 */
void drawPlane() {
	glEnable(GL_COLOR_MATERIAL);
	// set material properties which will be assigned by glColor
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glCallList(planeId);
	glDisable(GL_COLOR_MATERIAL);
}
/**
 * drawProps
 * Draws the propellers for the plane including positioning and rotating
 */
void drawProps() {
	glEnable(GL_COLOR_MATERIAL);
	// set material properties which will be assigned by glColor
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	propRotation += ((propellerSpeed - planeSpeed) * 2);

	glPushMatrix();
	glTranslatef(-0.01f, -0.14f, 0.35f);
	if (alive == 1) {
		glRotatef(propRotation, 1, 0, 0);
	}
	glTranslatef(0, 0.15f, -0.35f);
	glCallList(propId);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-0.01f, -0.14f, -0.35f);
	if (alive == 1) {
		glRotatef(propRotation, 1, 0, 0);
	}
	glTranslatef(0, 0.15f, -0.35f);
	glCallList(propId);
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);
}
/**
 * loadModel
 * Loads a model from file using a specific format.
 */
GLuint loadModel(GLuint id, GLuint numPoints, char* fileName, int colorScheme) {
	//store the points in an array
	Point* points = malloc(sizeof(Point) * numPoints);
	//count the number of points and faces
	int pointCount = 0;
	int normalCount = 0;
	int faceCount = 0;
	int objectCount = 0;
	//store the display list id for calling later.
	id = glGenLists(1);
	glNewList(id, GL_COMPILE);

	GLfloat diffuseMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat ambientMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat specularMaterial[4] = { 1.0, 1.0, 1.0, 1.0 };

	FILE * file;
	char objectName[256];
	char line[256];
	// open enterprise point/face data file
	file = fopen(fileName, "rt");
	if (file == NULL) {
		printf("Could not load resource.\n");
		return id;
	}

	//begin drawing triangles (stored in display list not actually drawn).

	//read line by line, assuming less than 256 characters
	while (fgets(line, 256, file) != NULL) {
		float x;
		float y;
		float z;
		float nx;
		float ny;
		float nz;
		char ch;
		//look for vertex information
		int found = sscanf(line, "v %f %f %f", &x, &y, &z);
		//if found vertex info
		if (found != 0) {
			//create a point and store in array
			Point point = { x, y, z };
			points[pointCount] = point;
			pointCount++;
		} else if ((found = sscanf(line, "n %f %f %f", &nx, &ny, &nz)) != 0) {
			//add the normal data to the right point
			points[normalCount].nx = nx;
			points[normalCount].ny = ny;
			points[normalCount].nz = nz;
			normalCount++;
		} else if ((found = sscanf(line, "g %s", objectName)) != 0) {
			//printf("%d %s\n", found, objectName);
			if (colorScheme == 1) {
				if (objectCount <= 3) {
					diffuseMaterial[0] = yellow[0];
					diffuseMaterial[1] = yellow[1];
					diffuseMaterial[2] = yellow[2];
					diffuseMaterial[3] = yellow[3];
				} else if (objectCount >= 4 && objectCount <= 5) {
					diffuseMaterial[0] = black[0];
					diffuseMaterial[1] = black[1];
					diffuseMaterial[2] = black[2];
					diffuseMaterial[3] = black[3];
				} else if (objectCount == 6) {
					diffuseMaterial[0] = purple[0];
					diffuseMaterial[1] = purple[1];
					diffuseMaterial[2] = purple[2];
					diffuseMaterial[3] = purple[3];
				} else if (objectCount == 7) {
					diffuseMaterial[0] = blue[0];
					diffuseMaterial[1] = blue[1];
					diffuseMaterial[2] = blue[2];
					diffuseMaterial[3] = blue[3];
				} else if (objectCount >= 8 && objectCount <= 10) {
					diffuseMaterial[0] = yellow[0];
					diffuseMaterial[1] = yellow[1];
					diffuseMaterial[2] = yellow[2];
					diffuseMaterial[3] = yellow[3];
				} else if (objectCount == 11) {
					diffuseMaterial[0] = black[0];
					diffuseMaterial[1] = black[1];
					diffuseMaterial[2] = black[2];
					diffuseMaterial[3] = black[3];
				} else if (objectCount >= 12 && objectCount <= 13) {
					diffuseMaterial[0] = yellow[0];
					diffuseMaterial[1] = yellow[1];
					diffuseMaterial[2] = yellow[2];
					diffuseMaterial[3] = yellow[3];
				} else if (objectCount >= 14 && objectCount <= 25) {
					diffuseMaterial[0] = blue[0];
					diffuseMaterial[1] = blue[1];
					diffuseMaterial[2] = blue[2];
					diffuseMaterial[3] = blue[3];
				} else if (objectCount >= 26 && objectCount <= 31) {
					diffuseMaterial[0] = yellow[0];
					diffuseMaterial[1] = yellow[1];
					diffuseMaterial[2] = yellow[2];
					diffuseMaterial[3] = yellow[3];
				}
			} else {
				if (objectCount == 0) {
					diffuseMaterial[0] = yellow[0];
					diffuseMaterial[1] = yellow[1];
					diffuseMaterial[2] = yellow[2];
					diffuseMaterial[3] = yellow[3];
				} else {
					diffuseMaterial[0] = red[0];
					diffuseMaterial[1] = red[1];
					diffuseMaterial[2] = red[2];
					diffuseMaterial[3] = red[3];
				}
			}
			objectCount++;
		} else if ((found = sscanf(line, "%c ", &ch)) != 0 && ch == 'f') {
			int f;
			//printf("char %c\n", ch);
			char *token;
			token = strtok(line, " ");
			ambientMaterial[0]=diffuseMaterial[0]*0.2f;
			ambientMaterial[1]=diffuseMaterial[1]*0.2f;
			ambientMaterial[2]=diffuseMaterial[2]*0.2f;
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularMaterial);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMaterial);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, none);
			glColor4f(diffuseMaterial[0], diffuseMaterial[1],
					diffuseMaterial[2], diffuseMaterial[3]);
			glBegin(GL_POLYGON);

			while (token != NULL) {
				f = atoi(token);
				if (f != 0) {
					//printf(" %d %f %f %f\n", f,points[f - 1].x, points[f - 1].y,
					//		points[f - 1].z);
					glNormal3f(points[f - 1].nx, points[f - 1].ny,
							points[f - 1].nz);
					glVertex3f(points[f - 1].x, points[f - 1].y,
							points[f - 1].z);
				}
				token = strtok( NULL, " ");
			}
			faceCount++;
			glEnd();
		}
	}

	glEndList();
	//close file
	fclose(file);
	//free list of points, not needed anymore (cached in display list)
	free(points);
	return id;
}
/**
 * loadGLTexture
 * Loads a texture and returns the textureid
 */
GLuint loadGLTexture(const char * filename, int wrap, int width, int height) {
	GLuint texture;
	BYTE * data;
	FILE * file;

	// open .raw texture file, reading bytes
	// .raw has no header
	file = fopen(filename, "rb");
	if (file == NULL) {
		printf("Could not load texture.\n");
		return 0;
	}
	// create data buffer
	data = malloc(width * height * 3);

	// read texture data
	fread(data, width * height * 3, 1, file);
	fclose(file);

	// generate texture id
	glGenTextures(1, &texture);
	// bind the texture
	glBindTexture( GL_TEXTURE_2D, texture);
	//configuration
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//wrap or cut off based on param
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

	// build 2d mipmaps
	gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,
	GL_RGB, GL_UNSIGNED_BYTE, data);

	// free data buffer, no longer needed
	free(data);

	//return the texture
	return texture;
}
/**
 * initTextures
 * Initializes the textures used.
 * Mountain, sky, sea
 */
void initTextures() {
	mountainTexture = loadGLTexture("./textures/mountain.raw", 1, 1280, 1104);
	seaTexture = loadGLTexture("./textures/sea.raw", 1, 1600, 1200);
	skyTexture = loadGLTexture("./textures/sky.raw", 1, 896, 385);
}

/**
 * display
 * Displays the scene, called by opengl every so many milliseconds.
 */
void display(int ms) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	update();
	draw();
	glutSwapBuffers();
	glutTimerFunc(ms, display, ms);
}
/**
 * LITERALLY DOES NOTHING OTHER THAN CALL THE RESHAPE FUNCTION "handleResize" WHEN NEEDED.
 * TIMERFUNCTION "display" IS USED FOR EVERYTHING ELSE.
 * **** OPENGL SOMETIMES...
 */
void myDisplay(void) {

}
/**
 * handleResize
 * Handles the resizing of the window at a very basic level.
 */
void handleResize(int w, int h) {
	appWidth = w;
	appHeight = h;

	// Set the viewport to be the entire window
	glViewport(0, 0, appWidth, appHeight);

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the correct perspective.
	gluPerspective(fov, (float) appWidth / (float) appHeight, nearValue,
			farValue);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);

}
/**
 * mouseWheel
 * Event occurs when scroll wheel is spun.
 */
void mouseWheel(int button, int state, int x, int y) {
	if (toggleAltControls == 1) {
		boolAccelerate = 0;
		boolDeaccelerate = 0;
		if (state == GLUT_DOWN) {
			return;
		}
		if (button == 3) {
			if (planeSpeed <= planeMaxSpeed - planeAcc) {
				planeSpeed += planeAcc;
			}
		} else if (button == 4) {
			if (planeSpeed >= planeMinSpeed + planeDeacc) {
				planeSpeed -= planeDeacc;
			}
		}
	}
}
/**
 * initGLUT
 * Initializes some GLUT values for the Application Window.
 * Configures the timer function for the display, and mouse function for GUI.
 */
void initGLUT(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(appWidth, appHeight);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(appX, appY);
	glutCreateWindow("CSCI 3161 - Project - Flight Simulator - Matthew Thomas");

	glutTimerFunc(15, display, 15);
	glutDisplayFunc(myDisplay);
	glutPassiveMotionFunc(mouseEvent);

	glutSpecialUpFunc(keySpecialUp);
	glutSpecialFunc(keySpecialDown);

	glutMouseFunc(mouseWheel);

	glutKeyboardUpFunc(keyUp);
	glutKeyboardFunc(keyDown);
	glutReshapeFunc(handleResize);
}
/**
 * initializeGL
 * Initializes GL settings.
 */
void initGL(void) {

	glClearColor(0.0f, 0.0f, 0.2f, 1.0);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glMatrixMode(GL_PROJECTION);
	gluPerspective(fov, /* field of view in degree */
	(float) appWidth / (float) appHeight,/* aspect ratio */
	nearValue, farValue);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, /* eye */
	atX, atY, atZ, /* looking at*/
	0.0, 1.0, 0.0); /* up is in positive Y direction */

	glEnable(GL_BLEND);
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode( GL_BACK, renderingOptions[toggleWireframe]);
	glPolygonMode( GL_FRONT, renderingOptions[toggleWireframe]);

	// define the color of the fog, light pink because assignment screenshots showed it
	GLfloat fogColor[4] = { 1, 0.7, 0.8, 0.3 };
	// set the color of the fog
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_MODE, GL_EXP);
	glHint(GL_FOG_HINT, GL_NICEST);
	glFogf(GL_FOG_DENSITY, originalFogDensity);
	glDisable(GL_FOG);

}
/**
 * drawAxis
 * Draws the indicator showing x,y,z axis
 */
void drawAxis() {
	glLineWidth(5.0f);
	glScalef(0.5f, 0.5f, 0.5f);
	glCallList(axisId);

}
/**
 * drawAltometer
 * Shows the altitude (y-coord) of camera
 */
void drawAltometer() {
	float scaleY = (eyeY / 20.0f);
	glDisable(GL_LIGHTING);

	glPushMatrix();
	glColor3f(0, 0, 1);	//blue
	glTranslatef(4.8f, -2.8f, -5.0f);
	glScalef(1, scaleY, 0);
	//glRotatef(-90,0,1,0);

	glBegin(GL_POLYGON);
	float sizeX = 0.2f;
	float sizeY = 1.0f;
	glVertex3f(0, 0, 0);
	glVertex3f(sizeX, 0, 0);
	glVertex3f(sizeX, sizeY, 0);
	glVertex3f(0, sizeY, 0);

	glEnd();
	glPopMatrix();
	glEnable(GL_LIGHTING);
}
/**
 * drawSpedometer
 * Shows the speed of the plane
 */
void drawSpedometer() {
	float scaleX = (2 * planeSpeed / planeMaxSpeed);
	glDisable(GL_LIGHTING);

	glPushMatrix();
	glColor3f(0, 1, 0);	//green
	glTranslatef(-5.05f, -2.8f, -5.0f);
	glScalef(scaleX, 1, 1);
	//glRotatef(-90,0,1,0);

	glBegin(GL_POLYGON);
	float sizeX = 1.0f;
	float sizeY = 0.2f;
	glVertex3f(0, 0, 0);
	glVertex3f(sizeX, 0, 0);
	glVertex3f(sizeX, sizeY, 0);
	glVertex3f(0, sizeY, 0);

	glEnd();
	glPopMatrix();
	glEnable(GL_LIGHTING);

}
/**
 * drawExplosion
 * Draws the explosion when you crash into the sea
 */
void drawExplosion(int slices, int stacks) {
	int i, j;
	for (i = 0; i <= slices; i++) {
		float lat0 = M_PI * (-0.5 + (float) (i - 1) / slices);
		float z0 = sin(lat0);
		float zr0 = cos(lat0);

		float lat1 = M_PI * (-0.5 + (float) i / slices);
		float z1 = sin(lat1);
		float zr1 = cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= stacks; j++) {
			float lng = 2 * M_PI * (float) (j - 1) / stacks;
			//randomize the position to create somewhat of a jagged shape .
			float x = cos(lng)
					+ (((randBetween(0, 1000) / 1000.0f) - 0.5f) / 10.0f);
			float y = sin(lng)
					+ (((randBetween(0, 1000) / 1000.0f) - 0.5f) / 10.0f);
			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0, y * zr0, z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1, y * zr1, z1);
		}
		glEnd();
	}
}
/**
 * explode
 * Causes the plane to explode when it hits the sea creating an explosion
 */
void explode() {
	glEnable(GL_COLOR_MATERIAL);
	// set material properties which will be assigned by glColor
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	int i;
	for (i = 0; i < 10; i++) {
		explosionScale += delta / 50.0f;
		glScalef(explosionScale, explosionScale, explosionScale);
		glColor4f(1, 0, 0, 0.2f);
		glRotatef(randBetween(0, 360), 1, 0, 0);

		glDisable(GL_CULL_FACE);
		drawExplosion(32, 32);
		glEnable(GL_CULL_FACE);
	}
	if (explosionScale > 1) {
		exploding = 0;
		explosionScale = 1.0;
	}
	glDisable(GL_COLOR_MATERIAL);
}
/**
 * update
 * Updates the appplication's logic.
 * Controls actual movement of camera and bullets
 */
void update() {
	universeTime += delta;
	appX = glutGet((GLenum) GLUT_WINDOW_X);
	appY = glutGet((GLenum) GLUT_WINDOW_Y);

	//if shooting
	if (boolShoot == 1) {
		//create a new bullet
		Bullet* bullet = malloc(sizeof(Bullet));
		bullet->x = eyeX + (sin(planeRotation));
		bullet->y = eyeY + (sin(planeYawRotation)) - 1;
		bullet->z = eyeZ + (cos(planeRotation));
		bullet->rotation = planeRotation
				+ ((randBetween(0, targetScale * 2) - (targetScale)) / 80.0f);
		bullet->yaw = planeYawRotation
				+ ((randBetween(0, targetScale * 2) - (targetScale)) / 80.0f);
		bullet->nextBullet = NULL;
		//if its the first bullet
		if (numBullets == 0) {
			firstBullet = bullet;
			currBullet = bullet;
		} else {
			//otherwise add to end of linked list
			currBullet->nextBullet = bullet;
			currBullet = bullet;
		}
		//count number of bullets
		numBullets++;
		//if more than allowed number of bullets
		if (numBullets > maxNumBullets) {
			//remove the first bullet and set the next in line as the new first
			Bullet* tempBullet = firstBullet;
			free(firstBullet);
			firstBullet = tempBullet->nextBullet;
		}
	}
	//if there are bullets
	if (numBullets > 0) {
		//move them a bit based on bullet direction and speed
		Bullet* bullet = firstBullet;
		while (bullet != NULL) {
			bullet->x += (sin(bullet->rotation) * (bulletSpeed));
			bullet->z += (cos(bullet->rotation) * (bulletSpeed));
			bullet->y += (sin(bullet->yaw) * (bulletSpeed));
			bullet = bullet->nextBullet;
		}
	}
	//if alive update the state of camera/plane
	if (alive == 1) {

		float centerX = appWidth / 2.0f;
		float distanceX = (centerX - lastMouseX) / appWidth;
		planeRotation += distanceX / (10.5 * (planeSpeed + 1));

		planeTilt = distanceX * 40.0f;

		if (boolAccelerate == 1 && planeSpeed <= planeMaxSpeed - planeAcc) {
			planeSpeed += planeAcc;
		} else if (boolDeaccelerate == 1
				&& planeSpeed >= planeMinSpeed + planeDeacc) {
			planeSpeed -= planeDeacc;
		}
		//if alternate controls are enabled
		if (toggleAltControls == 0) {
			if (boolMoveUp == 1) {
				eyeY += planeRiseSpeed;
				atY += planeRiseSpeed;
			} else if (boolMoveDown == 1) {
				eyeY -= planeFallSpeed;
				atY -= planeFallSpeed;
			}
		} else {
			float centerY = appHeight / 2.0f;
			float distanceY = (centerY - lastMouseY) / appHeight;
			planeYawRotation += distanceY / (20 * (planeSpeed + 1));
			if (planeYawRotation > 1) {
				planeYawRotation = 1;
			} else if (planeYawRotation < -1) {
				planeYawRotation = -1;
			}

			eyeY += (sin(planeYawRotation) * (planeSpeed + 1)) / 10.0f;
			atY = eyeY + (sin(planeYawRotation) * 10.0f);

		}
		//move the camera based on circles using plane speed as the radius
		eyeX += (sin(planeRotation) * (planeSpeed + 1)) / 10.0f;
		eyeZ += (cos(planeRotation) * (planeSpeed + 1)) / 10.0f;
		//always look 10 units ahead
		atX = eyeX + (sin(planeRotation) * 10.0f);
		atZ = eyeZ + (cos(planeRotation) * 10.0f);

		if (eyeY < 2) {
			alive = 0;
			explosionScale = 0.0f;
			exploding = 1;
			eyeY = 2;
		}
	}

}
/**
 * drawBullet
 * Draws a bullet
 */
void drawBullet(float x, float y, float z) {
	glColor3f(0.7f, 0.7f, 0.7f);
	glTranslatef(x, y, z);
	glutSolidSphere(0.03, 16, 16);
}
/**
 * draw
 *
 * Draws the entire scene based on toggles and current state
 */
void draw() {
	glLoadIdentity();

	glPushMatrix();
	glColor4f(1.0, 1.0, 1.0, 1.0f);
	//set up camera
	gluLookAt(eyeX, eyeY, eyeZ, /* eye */
	atX, atY, atZ, /* looking at*/
	0.0, 1.0, 0.0); /* up is in positive Y direction */

	//reset light including position
	initLight();

	//if alternate weather is enabled change fog to be based on camera y-coord
	if (toggleAltWeather == 1) {
		float heightFactor = eyeY - 3.0f;
		float density = 0.0f;
		if (heightFactor < 0) {
			heightFactor = 0;
		} else {
			density = heightFactor / 100.0f;
			if (density > originalFogDensity * 2) {
				density = originalFogDensity * 2;
			}
		}
		glFogf(GL_FOG_DENSITY, density);
	} else {
		//otherwise use the normal fog
		glFogf(GL_FOG_DENSITY, originalFogDensity);
	}
	if (toggleGrid == 1) {
		glPushMatrix();
		glDisable(GL_LIGHTING);
		drawAxis();
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}
	glColor4f(1, 1, 1, 1);
	glPushMatrix();
	if (toggleFog == 1) {
		glEnable(GL_FOG);
	}
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, dull);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	if (toggleMountains == 1) {
		glPushMatrix();
		glDisable(GL_COLOR_MATERIAL);

		if (toggleMountainTextures == 0) {
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, none);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, none);

			glEnable(GL_COLOR_MATERIAL);
			// set material properties which will be assigned by glColor
			glColorMaterial(GL_FRONT, GL_DIFFUSE);
		}

		drawMountains();

		glDisable(GL_COLOR_MATERIAL);
		glPopMatrix();
	}

	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, dull);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, none);
	glEnable(GL_CULL_FACE);
	glDisable( GL_TEXTURE_2D);
	if (toggleGrid == 1) {
		glTranslatef(-100, 0, -100);

		drawGrid();

		glDisable(GL_FOG);

	} else {
		glEnable( GL_TEXTURE_2D);
		glPushMatrix();
		//glDisable(GL_CULL_FACE);
		glTranslatef(0, -1, 0);
		gluQuadricNormals(seaObj, GLU_SMOOTH);
		gluQuadricTexture(seaObj, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D, seaTexture);
		gluQuadricTexture(seaObj, seaTexture);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, none);

		drawSea();

		glPopMatrix();

		if (toggleAltWeather == 0) {
			glDisable(GL_FOG);
		}

		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, dull);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
		glPushMatrix();
		gluQuadricNormals(skyObj, GLU_SMOOTH);
		gluQuadricTexture(skyObj, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D, skyTexture);
		gluQuadricTexture(skyObj, skyTexture);
		glDisable(GL_CULL_FACE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, yellow);
		drawSky();
		glEnable(GL_CULL_FACE);
		glPopMatrix();
		glDisable( GL_TEXTURE_2D);
	}

	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, dull);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);

	glDisable(GL_FOG);
	glPopMatrix();

	initLight();
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, littleshiny);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, littlespecular);

	if(alive==1){
		glPushMatrix();
		float rotationDeg=(planeRotation * 180.0f /M_PI);
		glTranslatef(eyeX+((atX-eyeX)/2.0f), eyeY+((atY-eyeY)/2.0f)-2,eyeZ+((atZ-eyeZ)/2.0f));
		glScalef(0.8f, 0.8f, 0.8f);
		glRotatef(rotationDeg, 0, 1, 0);
		glRotatef(-planeTilt, 0, 0, 1);
		if (toggleAltControls == 1) {
			glRotatef(-planeYawRotation * 30.0f, 1, 0, 0);
		}

		glRotatef(90, 0, 1, 0);

		drawPlane();
		drawProps();
		glDisable(GL_LIGHTING);
		drawTarget();


		glPopMatrix();
	}
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, dull);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, white);

	//draw bullets if there are any
	if (numBullets > 0) {
		Bullet* bullet = firstBullet;
		while (bullet != NULL) {
			glPushMatrix();
			drawBullet(bullet->x+((atX-bullet->x)/2.0f), bullet->y+((atY-bullet->y)/2.0f)-2, bullet->z+((atZ-bullet->z)/2.0f));
			glPopMatrix();
			bullet = bullet->nextBullet;
		}
	}
	glPopMatrix();

	//everything after is no longer affected by camera
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	glPushMatrix();
	drawSpedometer();
	glPopMatrix();
	glPushMatrix();
	drawAltometer();
	glPopMatrix();

	if (alive == 0 && exploding == 1) {
		explode();
	}
	glEnable(GL_LIGHTING);

}
/**
 * mouseEvent
 * Gets the mouse position
 */
void mouseEvent(int x, int y) {
	lastMouseX = x;
	lastMouseY = y;
}
/**
 * keyDown
 * Is called when a normal key is pressed down.
 */
void keyDown(unsigned char key, int mouseX, int mouseY) {
	//z for shooting
	if (key == 'z') {
		boolShoot = 1;
	}
}
/**
 * keyUp
 * Is called when a normal key is released.
 * Used to toggle the various features on or off.
 */
void keyUp(unsigned char key, int mouseX, int mouseY) {
	if (key == 'w') {
		toggleWireframe = 1 - toggleWireframe;
		glPolygonMode( GL_BACK, renderingOptions[toggleWireframe]);
		glPolygonMode( GL_FRONT, renderingOptions[toggleWireframe]);
	}
	if (key == 'b') {
		toggleFog = 1 - toggleFog;
	}
	if (key == 'f') {
		toggleFullscreen = 1 - toggleFullscreen;
		//glutFullScreenToggle();
		//hackery because the windows freeglut doesn't have glutFullScreenToggle even though it's supposed to... ****
		if (toggleFullscreen == 1) {
			storedWidth = appWidth;
			storedHeight = appHeight;
			storedX = appX;
			storedY = appY;
			glutFullScreen();
		} else {
			glutReshapeWindow(storedWidth, storedHeight);
			glutPositionWindow(storedX, storedY);
			//have to do it twice because... ****ing freeglut/opengl
			glutReshapeWindow(storedWidth, storedHeight);
			glutPositionWindow(storedX, storedY);
		}
	}
	if (key == 's') {
		toggleGrid = 1 - toggleGrid;
	}
	if (key == 't') {
		toggleMountainTextures = 1 - toggleMountainTextures;
	}
	if (key == 'm') {
		toggleMountains = 1 - toggleMountains;
	}
	if (key == 'r') {
		srand(time(NULL));

		initNew();
		eyeX = 0;
		eyeY = 5;
		eyeZ = -15;
		atX = 0;
		atY = 5;
		atZ = -5;
		boolAccelerate = 0;
		boolDeaccelerate = 0;
		boolMoveUp = 0;
		boolMoveDown = 0;
		boolShoot = 0;
		planeSpeed = 1.0f;
		planeRotation = 0.0f;
		planeTilt = 0.0f;
		planeYawRotation = 0.0f;
		alive = 1;
	}
	//z for shooting
	if (key == 'z') {
		boolShoot = 0;
	}
	if (key == 'q') {
		exit(0);
	}
}
/**
 * keySpecialDown
 * Is called when a special key is pressed.
 * Used to move the enterprise around (while not warping).
 */
void keySpecialDown(int key, int mouseX, int mouseY) {
	if (!toggleAltControls == 1) {
		if (key == GLUT_KEY_PAGE_UP) {
			boolAccelerate = 1;
		}
		if (key == GLUT_KEY_PAGE_DOWN) {
			boolDeaccelerate = 1;
		}
		if (key == GLUT_KEY_UP) {
			boolMoveUp = 1;
		}
		if (key == GLUT_KEY_DOWN) {
			boolMoveDown = 1;
		}
	}
	if (key == GLUT_KEY_F1) {
		toggleAltControls = 1 - toggleAltControls;
	}
	if (key == GLUT_KEY_F2) {
		toggleAltWeather = 1 - toggleAltWeather;
	}
}
/**
 * keySpecialUp
 * Is called when a special key is released.
 * Used to move the enterprise around (while not warping).
 */
void keySpecialUp(int key, int mouseX, int mouseY) {
	if (!toggleAltControls == 1) {
		if (key == GLUT_KEY_PAGE_UP) {
			boolAccelerate = 0;
		}
		if (key == GLUT_KEY_PAGE_DOWN) {
			boolDeaccelerate = 0;
		}
		if (key == GLUT_KEY_UP) {
			boolMoveUp = 0;
		}
		if (key == GLUT_KEY_DOWN) {
			boolMoveDown = 0;
		}

	}
}
