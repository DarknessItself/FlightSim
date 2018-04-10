//main.cpp

#pragma warning(disable:4996) // allow use of unsafe methods fscanf and sscanf

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <glut.h>
#include <math.h>
#include <time.h>


// simplify for loops
#define forEach(i, n) for(int i = 0; i < n; i++)

const GLfloat PI = 3.14159;

int windowWidth = 640, windowHeight = 480;

int mx, my;

GLuint seaTexture, landTexture, skyTexture;

// Where the camera is, where it's looking, how high in Y it is, and what direction it's pointing in
GLfloat camPos[] = { 10.0,10.0,20.0 };
GLfloat camAt[] = { 0.0,0.0,0.0 };
GLfloat camHeight = 10.0;
GLfloat camAngle = -PI / 4;

// Camera's velocity in XZ (trans.), Y, and XY (rot.)
GLfloat camHVel = 0.003;
GLfloat camVVel = 0.0;
GLfloat camRotVel = 0.0;

const GLfloat gridSize = 100.0f;

bool // flags
    wireframe     = false,
    vMouseControl = true,
	gridMode      = false,
	fog           = true,
	fullscreen    = false,
	islandTex     = true,
	drawIslands   = true;

GLfloat propTheta = 0;

int planeId, propId, landId;

GLUquadricObj *skyObj = gluNewQuadric();
GLUquadricObj *seaObj = gluNewQuadric();

GLfloat sunAmbient[]  = { 0.75, 0.75, 0.75 };
GLfloat sunDiffuse[]  = { 1, 1, 1 };
GLfloat sunSpecular[] = { 1, 1, 1 };
GLfloat sunPos[]      = { 300, 1500, 0, 1.0 };

// zero-vector used when we want to turn something off
GLfloat none[] = { 0,0,0,0 };

GLfloat fogColor[4] = { 0.8, 0.6, 0.7, 0.25 };


typedef GLfloat colour[4];

colour littlespecular = { 0.5, 0.5, 0.5, 1.0 };
colour darkgreen	  = { 0.0, 1.0, 0.0, 1.0 };
colour lightgreen	  = { 0.6, 0.8, 0.5, 1.0 };
colour white		  = { 1.0, 1.0, 1.0, 1.0 };
colour grey			  = { 0.3, 0.3, 0.3, 1.0 };
colour yellow		  = { 0.8, 0.8, 0.0, 1.0 };
colour red			  = { 1.0, 0.0, 0.0, 1.0 };
colour black		  = { 0.0, 0.0, 0.0, 1.0 };
colour blue			  = { 0.0, 0.0, 0.8, 1.0 };
colour purple		  = { 0.7, 0.5, 0.9, 1.0 };


typedef struct Point
{
	float x;
	float y;
	float z;
	float nx;
	float ny;
	float nz;
} Point;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLint loadCessna() {
	//store the points in an array
	Point points[6764];
	//Point* points = malloc(sizeof(Point) * 6764);
	//count the number of points and faces
	int pointCount = 0;
	int normalCount = 0;
	int faceCount = 0;
	int objectCount = 0;
	//store the display list id for calling later.
	planeId = glGenLists(1);
	glNewList(planeId, GL_COMPILE);
	
	GLfloat diffuseMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat ambientMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat specularMaterial[4] = { 1.0, 1.0, 1.0, 1.0 };

	FILE * file;
	char objectName[256];
	char line[256];
	// open cessna txt file
	file = fopen("cessna.txt", "rt");
	if (file == NULL) {
		printf("Could not load cessna.txt\n");
		return planeId;
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
		// start by looking for info about vertices
		int info = sscanf(line, "v %f %f %f", &x, &y, &z);
		// if vertex info found
		if (info != 0) {
			// create and store point
			Point point = { x, y, z };
			points[pointCount] = point;
			pointCount++;
		}
		else if ((info = sscanf(line, "n %f %f %f", &nx, &ny, &nz)) != 0) {
			// add the normal data to the right point
			points[normalCount].nx = nx;
			points[normalCount].ny = ny;
			points[normalCount].nz = nz;
			normalCount++;
		}
		else if ((info = sscanf(line, "g %s", objectName)) != 0)
		{
			//printf("%d %s\n", found, objectName);

			if (objectCount <= 3) {
				diffuseMaterial[0] = yellow[0];
				diffuseMaterial[1] = yellow[1];
				diffuseMaterial[2] = yellow[2];
				diffuseMaterial[3] = yellow[3];
			}
			else if (objectCount >= 4 && objectCount <= 5)
			{
				diffuseMaterial[0] = black[0];
				diffuseMaterial[1] = black[1];
				diffuseMaterial[2] = black[2];
				diffuseMaterial[3] = black[3];
			}
			else if (objectCount == 6)
			{
				diffuseMaterial[0] = purple[0];
				diffuseMaterial[1] = purple[1];
				diffuseMaterial[2] = purple[2];
				diffuseMaterial[3] = purple[3];
			}
			else if (objectCount == 7) {
				diffuseMaterial[0] = blue[0];
				diffuseMaterial[1] = blue[1];
				diffuseMaterial[2] = blue[2];
				diffuseMaterial[3] = blue[3];
			}
			else if (objectCount >= 8 && objectCount <= 10)
			{
				diffuseMaterial[0] = yellow[0];
				diffuseMaterial[1] = yellow[1];
				diffuseMaterial[2] = yellow[2];
				diffuseMaterial[3] = yellow[3];
			}
			else if (objectCount == 11) {
				diffuseMaterial[0] = black[0];
				diffuseMaterial[1] = black[1];
				diffuseMaterial[2] = black[2];
				diffuseMaterial[3] = black[3];
			}
			else if (objectCount >= 12 && objectCount <= 13)
			{
				diffuseMaterial[0] = yellow[0];
				diffuseMaterial[1] = yellow[1];
				diffuseMaterial[2] = yellow[2];
				diffuseMaterial[3] = yellow[3];
			}
			else if (objectCount >= 14 && objectCount <= 25)
			{
				diffuseMaterial[0] = blue[0];
				diffuseMaterial[1] = blue[1];
				diffuseMaterial[2] = blue[2];
				diffuseMaterial[3] = blue[3];
			}
			else if (objectCount >= 26 && objectCount <= 31)
			{
				diffuseMaterial[0] = yellow[0];
				diffuseMaterial[1] = yellow[1];
			    diffuseMaterial[2] = yellow[2];
			    diffuseMaterial[3] = yellow[3];
		    }
	
		objectCount++;
		}
		else if ((info = sscanf(line, "%c ", &ch)) != 0 && ch == 'f') {
			int f;
			//printf("char %c\n", ch);
			char *token;
			token = strtok(line, " ");
			ambientMaterial[0] = diffuseMaterial[0] * 0.2f;
			ambientMaterial[1] = diffuseMaterial[1] * 0.2f;
			ambientMaterial[2] = diffuseMaterial[2] * 0.2f;
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularMaterial);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMaterial);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, none);
			glColor4f(diffuseMaterial[0], diffuseMaterial[1], diffuseMaterial[2], diffuseMaterial[3]);
			glBegin(GL_POLYGON);

			while (token != NULL) {
				f = atoi(token);
				if (f != 0) {
					//printf(" %d %f %f %f\n", f,points[f - 1].x, points[f - 1].y,
					//		points[f - 1].z);
					glNormal3f(points[f - 1].nx, points[f - 1].ny, points[f - 1].nz);
					glVertex3f(points[f - 1].x, points[f - 1].y, points[f - 1].z);
				}
				token = strtok(NULL, " ");
			}
			faceCount++;
			glEnd();
		}
	}

	glEndList();

	//close file
	fclose(file);

	return planeId;
}

GLint loadProps()
{
	//store the points in an array
	Point points[6764];
	//Point* points = malloc(sizeof(Point) * 6764);
	//count the number of points and faces
	int pointCount = 0;
	int normalCount = 0;
	int faceCount = 0;
	int objectCount = 0;
	//store the display list id for calling later.
	propId = glGenLists(1);
	glNewList(propId, GL_COMPILE);

	GLfloat diffuseMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat ambientMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat specularMaterial[4] = { 1.0, 1.0, 1.0, 1.0 };

	FILE * file;
	char objectName[256];
	char line[256];
	// open cessna txt file
	file = fopen("propeller.txt", "rt");
	if (file == NULL)
	{
		printf("Could not load cessna.txt\n");
		return propId;
	}

	//begin drawing triangles (stored in display list not actually drawn).

	//read line by line, assuming less than 256 characters
	while (fgets(line, 256, file) != NULL)
	{
		float x;
		float y;
		float z;
		float nx;
		float ny;
		float nz;
		char ch;
		// start by looking for info about vertices
		int info = sscanf(line, "v %f %f %f", &x, &y, &z);
		// if vertex info found
		if (info != 0) {
			// create and store point
			Point point = { x, y, z };
			points[pointCount] = point;
			pointCount++;
		}
		else if ((info = sscanf(line, "n %f %f %f", &nx, &ny, &nz)) != 0) {
			// add the normal data to the right point
			points[normalCount].nx = nx;
			points[normalCount].ny = ny;
			points[normalCount].nz = nz;
			normalCount++;
		}
		else if ((info = sscanf(line, "g %s", objectName)) != 0)
		{
			//printf("%d %s\n", found, objectName);

			if (objectCount == 0) {
				diffuseMaterial[0] = yellow[0];
				diffuseMaterial[1] = yellow[1];
				diffuseMaterial[2] = yellow[2];
				diffuseMaterial[3] = yellow[3];
			}
			else {
				diffuseMaterial[0] = red[0];
				diffuseMaterial[1] = red[1];
				diffuseMaterial[2] = red[2];
				diffuseMaterial[3] = red[3];
			}

			objectCount++;
		}
		else if ((info = sscanf(line, "%c ", &ch)) != 0 && ch == 'f') {
			int f;
			//printf("char %c\n", ch);
			char *token;
			token = strtok(line, " ");
			ambientMaterial[0] = diffuseMaterial[0] * 0.2f;
			ambientMaterial[1] = diffuseMaterial[1] * 0.2f;
			ambientMaterial[2] = diffuseMaterial[2] * 0.2f;
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularMaterial);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMaterial);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, none);
			glColor4f(diffuseMaterial[0], diffuseMaterial[1], diffuseMaterial[2], diffuseMaterial[3]);
			glBegin(GL_POLYGON);

			while (token != NULL) {
				f = atoi(token);
				if (f != 0) {
					//printf(" %d %f %f %f\n", f,points[f - 1].x, points[f - 1].y,
					//		points[f - 1].z);
					glNormal3f(points[f - 1].nx, points[f - 1].ny, points[f - 1].nz);
					glVertex3f(points[f - 1].x, points[f - 1].y, points[f - 1].z);
				}
				token = strtok(NULL, " ");
			}
			faceCount++;
			glEnd();
		}
	}

	glEndList();

	//close file
	fclose(file);

	return propId;
}

void initSun()
{

	// Set lighting values, self explanatory
	glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sunSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, sunPos);

	glShadeModel(GL_SMOOTH);
	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	GLfloat diffuseMaterial[4] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat ambientMaterial[4] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat specularMaterial[4] = { 0.9, 0.9, 0.9, 1.0 };
	GLfloat emissiveMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, new GLfloat { 1 });
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
}

void initSky()
{
	gluQuadricNormals(skyObj, GLU_SMOOTH);
	gluQuadricTexture(skyObj, GL_TRUE);
}

void initSea()
{
	gluQuadricNormals(seaObj, GLU_SMOOTH);
	gluQuadricTexture(seaObj, GL_TRUE);
}

// Performs millions of years worth of geology in seconds
void geologize(float map[75][75], int left, int right, int top, int bottom, int iteration)
{
	if (iteration >= 8) {
		return;
	}
	int width = right - left;
	int height = bottom - top;
	map[left + (width / 2)][top + (height / 2)] += (2 * (GLfloat)rand() / RAND_MAX) / (iteration);
	map[left][top + (height / 2)]               += (2 * (GLfloat)rand() / RAND_MAX) / (iteration);
	map[left + (width / 2)][top]                += (2 * (GLfloat)rand() / RAND_MAX) / (iteration);
	map[left + (width - 1)][top + (height / 2)] += (2 * (GLfloat)rand() / RAND_MAX) / (iteration);
	map[left + (width / 2)][top + (height - 1)] += (2 * (GLfloat)rand() / RAND_MAX) / (iteration);

	iteration++;

	geologize(map, left, left + (width / 2), top, bottom - (height / 2), iteration);
	geologize(map, left + (width / 2), right, top, bottom - (height / 2), iteration);
	geologize(map, left, left + (width / 2), top + (height / 2), bottom, iteration);
	geologize(map, left + (width / 2), right, top + (height / 2), bottom, iteration);
}

void initIslands()
{
	landId = glGenLists(1);
	glNewList(landId, GL_COMPILE);

	//glBindTexture(GL_TEXTURE_2D, landTexture);

	int i;
	int numIslands = 5;

	// angle and distance from origin for each island
	float angle, distance;

	//generate a certain number of islands
	for (i = 0; i < numIslands; i++) {
		glPushMatrix();
		angle = ((360 / numIslands) * i);
		distance = 100 + 100 * (GLfloat) rand() / RAND_MAX;

		//move it down so edges are below sea
		glTranslatef(0, -3, 0);

		//glScalef(0.5f, 0.5f, 0.5f);
		glScalef((20 + 130 * (GLfloat) rand() / RAND_MAX) / 100.0f, (20 + 130 * (GLfloat) rand() / RAND_MAX) / 100.0f, (20 + 130 * (GLfloat) rand() / RAND_MAX) / 100.0f);

		glTranslatef(sin(angle) * distance, 0, cos(angle) * distance);
		glTranslatef(-32, 0, -32);
		
		float map[75][75];

		//initialize the heights to be in a cone-like shape based on distance from center
		forEach(x, 75)
		{
			forEach(z, 75)
			{
				float distance = sqrt((pow((75 / 2) - x, 2)) + (pow((75 / 2) - z, 2))) * 0.9f;
				map[x][z] = ((75 / 2) - distance) / 2.0f;
				if (map[x][z] < 0) map[x][z] = 0;
			}
		}

		//recursively raise the island to give it peaks and valleys, ignore the edges
		geologize(map, 1, 74, 1, 74, 1);

		// change the outer edge to always be flat on the ground
		forEach(i, 75)
		{
			map[i][0] = 0.0;
			map[i][74] = 0.0;
			map[0][i] = 0.0;
			map[74][i] = 0.0;
		}

		//glBindTexture(GL_TEXTURE_2D, landTexture);

		//draw the island
		forEach(x, 74)
		{
			forEach(z, 74)
			{
				glBegin(GL_POLYGON);

				//be default the normal is straight up (edges)
				glNormal3f(0, 1, 0);

				glColor3f(map[x][z+1], 1, map[x][z+1]);
				//map coords of texture
				glTexCoord2f((x / 75), ((z + 1) / 75));
				//draw vertex
				glVertex3f(x , map[x][z + 1], z + 1);

				//repeat for other 3 points of tile/quad/square

				glColor3f(map[x + 1][z + 1] / 150, 1, map[x + 1][z + 1] / 150);
				glTexCoord2f(((x + 1) / 75), ((z + 1) / 75));
				glVertex3f(x + 1, map[x + 1][z + 1], z + 1);

				glColor3f(map[x + 1][z] / 150, 1, map[x + 1][z] / 150);
				glTexCoord2f(((x + 1) / 75), (z / 75));
				glVertex3f(x + 1 , map[x + 1][z], z);
				
				glColor3f(map[x][z] / 150, 1, map[x][z] / 150);
				glTexCoord2f((x / 75), (z / 75));
				glVertex3f(x, map[x][z], z);
				glEnd();
			}
		}
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glPopMatrix();
	}
	glEndList();
}

// largely taken from example code from brightspace
void loadTexture(const char * filename, GLuint * ID)
{
	int imageWidth, imageHeight;

	// temporary character
	char tempChar;

	// array for reading in header information
	char headerLine[100];

	// open the image file for reading
	FILE *fileID = fopen(filename, "r");

	// read in the first header line
	fscanf(fileID, "%[^\n] ", headerLine);

	// make sure that the image begins with 'P3', which signifies a PPM file
	if ((headerLine[0] != 'P') || (headerLine[1] != '3'))
	{
		printf("This is not a PPM file!\n");
		exit(0);
	}

	// read in the first character of the next line
	fscanf(fileID, "%c", &tempChar);

	// while we still have comment lines (which begin with #)
	while (tempChar == '#')
	{
		// read in the comment
		fscanf(fileID, "%[^\n] ", headerLine);
		// read in the first character of the next line
		fscanf(fileID, "%c", &tempChar);
	}

	// the last one was not a comment character '#', so we need to put it back into the file stream (undo)
	ungetc(tempChar, fileID);

	// read in the image hieght, width and the maximum value
	int maxValue;
	fscanf(fileID, "%d %d %d", &imageWidth, &imageHeight, &maxValue);

	glEnable(GL_TEXTURE_2D);

	GLubyte *texture = (GLubyte *)malloc(3 * imageWidth * imageHeight * sizeof(GLubyte));

	// if the maxValue is 255 carry on, otherwise report that we only support max value of 255
	int c;
	if(maxValue == 255)
	{
		forEach(b, 3 * imageWidth * imageHeight)
		{
			fscanf(fileID, "%d", &c);
			texture[b] = (GLubyte) c;
		}
	}
	else printf("This program does not support PPM files with max values other than 255");

	// close the image file
	fclose(fileID);

	glGenTextures(1, ID);
	glBindTexture(GL_TEXTURE_2D, *ID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// build 2d mipmaps
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, texture);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void drawAxis()
{
	float lineLength = 7.0f;

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
	glutSolidSphere(0.3, 16, 16);
}

void drawGrid()
{
	float x, y, z;
	glTranslatef(0, -0.5f, 0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	y = -0.5f;
	glBegin(GL_QUADS);
	for(int x = -gridSize; x < gridSize; x++)
	{
		for (int z = -gridSize; z < gridSize; z++)
		{
			glColor3f(0.3, 0.3, 0.3);
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
	glDisable(GL_COLOR_MATERIAL);
}

void drawSky()
{
	glTranslatef(0, 990, 0);
	glRotatef(90, 1, 0, 0);

	GLfloat diffuseMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat ambientMaterial[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mShininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
	gluCylinder(skyObj, 500, 500, 500 * 2, 100, 100);

	glRotatef(-180, 0, 1, 0);

	glScalef(1.01, 1, 1.01);
	gluCylinder(skyObj, 500, 500, 500, 100, 100);
}

void drawSea()
{
	gluQuadricNormals(seaObj, GLU_SMOOTH);
	gluQuadricTexture(seaObj, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, seaTexture);
	gluQuadricTexture(seaObj, seaTexture);

	glRotatef(270, 1, 0, 0);
	glColor4f(0.0, 0.0, 0.6, 1.0);
	GLfloat ambientMaterial[4] = { 0.2, 0.2, 0.4, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, new GLfloat{ 50 });
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, lightgreen);
	gluDisk(seaObj, 0, 510, 64, 64);
}

void drawProps()
{
	glEnable(GL_COLOR_MATERIAL);
	// set material properties which will be assigned by glColor
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	
	glPushMatrix();
	glTranslatef(-0.01f, -0.14f, 0.35f);
	glRotatef(propTheta, 1, 0, 0);
	glTranslatef(0, 0.15f, -0.35f);
	glCallList(propId);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-0.01f, -0.14f, -0.35f);
    glRotatef(propTheta, 1, 0, 0);
	glTranslatef(0, 0.15f, -0.35f);
	glCallList(propId);
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);
}

void drawPlane()
{
	glPushMatrix();
	glTranslatef(camPos[0] + (3 * sin(camAngle)), camAt[1] - .75 + 5 * camVVel, camPos[2] - (3 * cos(camAngle)));
	glScalef(0.75f, 0.75f, 0.75f);
	glRotatef((180 / PI * -camAngle) - 90, 0, 1, 0); // orient the cessna to face away from the camera
	glTranslatef(-150 * camHVel, 0, -50 * camRotVel);
	glRotatef(-2000 * camRotVel, 0, 1, 0); // rotation
	glRotatef(-7500 * camRotVel, 1, 0, 0); // banking
	glRotatef(-850 * camVVel, 0, 0, 1); // up/ down
	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glCallList(planeId);
	glDisable(GL_COLOR_MATERIAL);
	drawProps();
	glPopMatrix();
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	// set the camera position
	gluLookAt(camPos[0], camPos[1], camPos[2],
			  camAt[0],  camAt[1],  camAt[2],
			  0,         1,         0);

	initSun();

	if (gridMode)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		drawAxis();
		drawGrid();
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
    }
	else
	{
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glTranslatef(0, -1, 0);

		if (fog) glEnable(GL_FOG);

		drawSea();

		glDisable(GL_FOG);

		glPopMatrix();

		if (drawIslands)
		{
			glDisable(GL_CULL_FACE);
			if (islandTex) glBindTexture(GL_TEXTURE_2D, landTexture);

			glCallList(landId);
			glEnable(GL_CULL_FACE);
		}
		
		glPushMatrix();

		gluQuadricNormals(skyObj, GLU_SMOOTH);
		gluQuadricTexture(skyObj, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D, skyTexture);
		gluQuadricTexture(skyObj, skyTexture);
		glDisable(GL_CULL_FACE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, new GLfloat {50});
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, yellow);

		drawSky();

		glEnable(GL_CULL_FACE);

		glPopMatrix();
	}

	//initSun();

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, new GLfloat {10});
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);

	drawPlane();

	glutSwapBuffers();
}

void moveCamH(GLfloat amount)
{
	camPos[0] -= amount * (camPos[0] - camAt[0]);
	camPos[2] -= amount * (camPos[2] - camAt[2]);
}

void key(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'w':
			wireframe = !wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
			break;
		case 'v':
			vMouseControl = !vMouseControl;
			break;
		case 's':
			gridMode = !gridMode;
			break;
		case 'b':
			fog = !fog;
			break;
		case 'i':
			initIslands();
			break;
		case 't':
			islandTex = !islandTex;
			break;
		case 'f':
			if (fullscreen) glutReshapeWindow(640, 480);
			else glutFullScreen();
			fullscreen = !fullscreen;
			break;
		case 'm':
			drawIslands = !drawIslands;
			break;
	}
}

void sKey(int key, int x, int y)
{
	// add to or subtract from velocities and ensure they don't exceed bounds
	//if (key == GLUT_KEY_LEFT) camRotVel -= .0001;
	//if (key == GLUT_KEY_RIGHT) camRotVel += .0001;
	if (key == GLUT_KEY_DOWN) camVVel -= .001;
	if (key == GLUT_KEY_UP) camVVel += .001;
	if (key == GLUT_KEY_PAGE_DOWN) camHVel -= 0.0002;
	if (key == GLUT_KEY_PAGE_UP) camHVel += 0.0002;

	if (camHVel > 0.01) camHVel = 0.01;
	else if (camHVel < 0.003) camHVel = 0.003;
}

void pMouse(int x, int y){ mx = x, my = y; }

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)w / (float)h, 1, 10000);
	glMatrixMode(GL_MODELVIEW);
	windowWidth = w, windowHeight = w;
}

void idle()
{
	// move the camera according to it's velocity, and decrease it's speed
	moveCamH(camHVel);

	camAngle += camRotVel;

	camHeight += camVVel;
	camPos[1] = camHeight;

	// update where the camera is looking
	camAt[0] = camPos[0] + (10 * sin(camAngle));
	camAt[1] = camHeight + 25 * camVVel;
	camAt[2] = camPos[2] - (10 * cos(camAngle));

	// mouse control
	camRotVel += -(GLfloat)(windowWidth / 2 - mx) / 10000000;
	if(vMouseControl) camVVel += ((GLfloat) windowHeight / 3 - my) / 3000000;

	// can bank harder when going slower
	if (abs(camRotVel) > 0.002 + 0.00001 / camHVel) camRotVel *= (1 - 200 * camHVel * abs(camRotVel));

	if (camVVel > 7.5 * camHVel) camVVel *= (1 - (GLfloat) camVVel / 25);
	else if (camVVel < -.1) camVVel = -.1;

	camVVel *= 0.998;
	camRotVel *= 0.995;

	propTheta += ((300 * camHVel) + (75 * (camVVel + 0.07)));

	if (propTheta >= 360) propTheta = 0;

	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	srand(time(NULL));

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("A00396139 A03");

	// set CBs
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(key);
	glutSpecialFunc(sKey);
	glutPassiveMotionFunc(pMouse);

	// init params
	glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glPolygonMode(GL_FRONT, GL_FILL);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 640.0 / 480.0, 1, 10000);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glMatrixMode(GL_MODELVIEW);

	loadTexture("sea02.ppm", &seaTexture);
	loadTexture("mount03.ppm", &landTexture);
	//loadTexture("sky08.ppm", &landTexture);
	loadTexture("sky08.ppm", &skyTexture);

	loadCessna();
	loadProps();

	initIslands();

	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_MODE, GL_EXP);
	glHint(GL_FOG_HINT, GL_NICEST);
	glFogf(GL_FOG_DENSITY, 0.004f);

	// print controls
	printf("\n\nScene controls\n----------------\n\n");
	printf("r:\trings\n");
	printf("s:\tstars\n");
	printf("c:\tcorona\n");
	printf("k:\tshield\n");
	printf("z:\tcruise control\n");
	printf("g:\tred giant\n");
	printf("\n\nCamera controls\n----------------\n\n");
	printf("UP:\tup\n");
	printf("DOWN:\tdown\n");
	printf("LEFT:\tleft\n");
	printf("RIGHT:\tright\n");
	printf("PG UP:\tforwards\n");
	printf("PG DN:\tbackwards\n");

	glutMainLoop();
}
