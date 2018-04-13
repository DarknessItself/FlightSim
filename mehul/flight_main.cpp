//Flight Sim
#pragma warning(disable:4996) // allow use of unsafe methods fscanf and sscanf

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <glut.h>
#include <math.h>

// simplify for loops
#define forEvery(i, n) for(int i = 0; i < n; i++)

const GLfloat PI = 3.14159265;
int w_Width = 640, w_Height = 480;


// to track down mouse
int mouseX, mouseY;

// texture integers:
GLuint skyTexture;
GLuint seaTexture;
GLuint mountainTexture;


// Where the camera is, where it's looking, how high in Y it is.
GLfloat cameraPosition[] = { 3.0,3.0,3.0};
GLfloat cameraLookAt[] = { 0.0,0.0,0.0 };
GLfloat cameraHeight = 5.0;
GLfloat cameraAngle = -PI / 4;

// Camera's velocity in XZ (trans.), Y, and XY (rot.)
GLfloat cameraHVelocity = 0.002;
GLfloat cameraVVelocity = 0.0;
GLfloat cameraRotVelocity = 0.0;

const GLfloat gridSize = 200.0f;

//vMouseCtrl toggles if you can control the verticle motion of the plane with mouse
bool wireframe = false;
bool gridView = false;
bool verticleMouseCtrl = true;
bool fogToggle = true;
bool drawMountain = true;
bool mountainTextureToggle = true;
bool fullscreenToggle = false;





// propeller rotation control
GLfloat propellerRotation = 0;


// GL list IDs to draw objects
int cessnaID, propellerID, mountainID;


// making objects for sky and sea
GLUquadric *sky = gluNewQuadric();
GLUquadric *sea = gluNewQuadric();

// Light properties for skyLight
GLfloat skyLightAmbient[]  = { 0.2, 0.2, 0.2 };
GLfloat skyLightDiffuse[]  = { 1, 1, 1 };
GLfloat skyLightSpecular[] = { 1, 1, 1 };
GLfloat skyLightPosition[] = { 500, 250,500, 1};


// none vector to reset
GLfloat none[]			 = { 0,0,0,0 };


//points for coordinates and normals
typedef struct Point
{
	float x;
	float y;
	float z;
	float nx;
	float ny;
	float nz;
} Point;


// color arrays for coloring objects in the scene

GLfloat white[4]			  = { 1.0, 1.0, 1.0, 1.0 };
GLfloat grey[4]				  = { 0.3, 0.3, 0.3, 1.0 };
GLfloat black[4]			  = { 0.0, 0.0, 0.0, 1.0 };
GLfloat red[4]				  = { 1.0, 0.0, 0.0, 1.0 };
GLfloat green[4]			  = { 0.0, 1.0, 0.0, 1.0 };
GLfloat lightgreen[4]		  = { 0.6, 0.8, 0.5, 1.0 };
GLfloat blue[4]				  = { 0.0, 0.0, 0.8, 1.0 };
GLfloat yellow[4]			  = { 0.8, 0.8, 0.0, 1.0 };
GLfloat purple[4]			  = { 0.7, 0.5, 0.9, 1.0 };

//Fog color
GLfloat fogColor[4] = { 1.0, 0.6196, 0.6862, 0.30 };



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///												Calculating Normals for points                                                     ///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Point normalCalculator(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
	float obj1X, obj1Y, obj1Z;
	float obj2X, obj2Y, obj2Z;
	float nx, ny, nz;

	//bw 2 and 1
	obj1X = x2 - x1;
	obj1Y = y2 - y1;
	obj1Z = z2 - z1;

	//bw 3 and 1
	obj2X = x3 - x1;
	obj2Y = y3 - y1;
	obj2Z = z3 - z1;

	
	nx = (obj1Y * obj2Z) - (obj1Z * obj2Y);
	ny = (obj1Z * obj2X) - (obj1X * obj2Z);
	nz = (obj1X * obj2Y) - (obj1Y * obj2X);

	float length = sqrt(pow(nx, 2) + pow(ny, 2) + pow(nz, 2));

	nx /= length;
	ny /= length;
	nz /= length;

	Point normal = { x1, y1, z1, nx, ny, nz };

	return normal;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///											Reading provided files									  ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// reads Cessna File, uses a glList 
GLint readCessna() {
	//store the points and normals in an array of type Point (Defined earlier)

	Point points[6764];
	
	//count the number of points, faces, normanls and objects
	int pointCount = 0;
	int normalCount = 0;
	int faceCount = 0;
	int objectCount = 0;
	
	//store the information in a glList
	cessnaID = glGenLists(1);
	glNewList(cessnaID, GL_COMPILE);
	
	//material properties for Cessna
	GLfloat diffuseMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat ambientMaterial[4] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat specularMaterial[4] = { 1.0, 1.0, 1.0, 1.0 };//shiny plane

	FILE * file;
	char objectName[256];
	char line[256];
	// open cessna txt file
	file = fopen("cessna.txt", "rt");
	if (file == NULL) {
		printf("Could not load cessna.txt\n");
		return cessnaID;
	}

	
	//read each line
	while (fgets(line, 256, file) != NULL) {
		float x;
		float y;
		float z;
		float nx;
		float ny;
		float nz;
		char ch;
		
		// look in file for vertices
		int info = sscanf(line, "v %f %f %f", &x, &y, &z);
		
		// store vertex info if found
		if (info != 0) {
			// create and store point
			Point point = { x, y, z };
			points[pointCount] = point;
			pointCount++;
		}
		else if ((info = sscanf(line, "n %f %f %f", &nx, &ny, &nz)) != 0) {
			// the normal data for the given point
			points[normalCount].nx = nx;
			points[normalCount].ny = ny;
			points[normalCount].nz = nz;
			normalCount++;
		}
		else if ((info = sscanf(line, "g %s", objectName)) != 0)
		{
			//if something else, set color properties based on object
			

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
		else if ((info = sscanf(line, "%c ", &ch)) != 0 && ch == 'f') { // detect a face
			int f;
			char *token;
			token = strtok(line, " ");

			// Material properties
			ambientMaterial[0] = diffuseMaterial[0] * 0.2f;
			ambientMaterial[1] = diffuseMaterial[1] * 0.2f;
			ambientMaterial[2] = diffuseMaterial[2] * 0.2f;

			//setting properties for the material
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularMaterial);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMaterial);
			//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, none);

			glColor4f(diffuseMaterial[0], diffuseMaterial[1], diffuseMaterial[2], diffuseMaterial[3]);
			
			// drawing vertices based on face
			glBegin(GL_POLYGON);

			while (token != NULL) {
				f = atoi(token);
				if (f != 0) {
					
					glNormal3f(points[f - 1].nx, points[f - 1].ny, points[f - 1].nz);
					glVertex3f(points[f - 1].x, points[f - 1].y, points[f - 1].z);
				}
				token = strtok(NULL, " ");
			}
			faceCount++;
			glEnd();// end drawing
		}
	}

	//end the list
	glEndList();

	//close file
	fclose(file);

	return cessnaID;
}


// reads the propeller file, uses a glList same as that of Cessna
GLint readPropeller()
{
	//store the points and normals in an array of type Point (Defined earlier)
	Point points[6764];
	
	//count the number of points, faces, normanls and objects
	int pointCount = 0;
	int normalCount = 0;
	int faceCount = 0;
	int objectCount = 0;


	//store the information in a glList
	propellerID = glGenLists(1);
	glNewList(propellerID, GL_COMPILE);

	//material properties for Cessna
	GLfloat diffuseMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat ambientMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat specularMaterial[4] = { 1.0, 1.0, 1.0, 1.0 }; // shiny Propeller, same as plane

	FILE * file;
	char objectName[256];
	char line[256];

	// open propeller txt file
	file = fopen("propeller.txt", "rt");
	if (file == NULL)
	{
		printf("Could not load propeller.txt \n");
		return propellerID;
	}

	
	//read each line
	while (fgets(line, 256, file) != NULL)
	{
		float x;
		float y;
		float z;
		float nx;
		float ny;
		float nz;
		char ch;


		// look in file for vertices
		int info = sscanf(line, "v %f %f %f", &x, &y, &z);
		// store vertex info if found
		if (info != 0) {
			// create and store point
			Point point = { x, y, z };
			points[pointCount] = point;
			pointCount++;
		}
		else if ((info = sscanf(line, "n %f %f %f", &nx, &ny, &nz)) != 0) {
			// the normal data for the given file
			points[normalCount].nx = nx;
			points[normalCount].ny = ny;
			points[normalCount].nz = nz;
			normalCount++;
		}
		else if ((info = sscanf(line, "g %s", objectName)) != 0)
		{
			//if something else, set color properties based on object

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
			
			char *token;
			token = strtok(line, " ");

			// Material properties
			ambientMaterial[0] = diffuseMaterial[0] * 0.4f;
			ambientMaterial[1] = diffuseMaterial[1] * 0.4;
			ambientMaterial[2] = diffuseMaterial[2] * 0.4f;

			//setting properties for the material
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularMaterial);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMaterial);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, none);

			glColor4f(diffuseMaterial[0], diffuseMaterial[1], diffuseMaterial[2], diffuseMaterial[3]);

			// drawing vertices based on face
			glBegin(GL_POLYGON);

			while (token != NULL) {
				f = atoi(token);
				if (f != 0) {
					
					glNormal3f(points[f - 1].nx, points[f - 1].ny, points[f - 1].nz);
					glVertex3f(points[f - 1].x, points[f - 1].y, points[f - 1].z);
				}
				token = strtok(NULL, " ");
			}
			faceCount++;
			glEnd(); // end drawing
		}
	}

	//end the list
	glEndList();

	//close file
	fclose(file);

	return propellerID;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///												Setting up lights                                                     ///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//initializing skyLight
void initializeskyLight() {

	//set up lighting 
	glLightfv(GL_LIGHT0, GL_AMBIENT, skyLightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, skyLightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, skyLightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, skyLightPosition);



	glShadeModel(GL_SMOOTH);

	GLfloat globalAmbientLight[] = { 0.4, 0.4, 0.4, 1.0 };
	GLfloat diffuseMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat ambientMaterial[4] = { 0.05, 0.05, 0.05, 1.0 };
	GLfloat specularMaterial[4] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat emissiveMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };

	//global
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLight);


	// putting attributes to materials
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, new GLfloat{1});


	//enable GL states
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);





}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///													Sky                                                               ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//initializing sky
void initializing_sky() {

	gluQuadricNormals(sky, GLU_SMOOTH);
	gluQuadricTexture(sky, GL_TRUE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///													Sea                                                               ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//initializing sea
void initializing_sea() {

	gluQuadricNormals(sea, GLU_SMOOTH);
	gluQuadricTexture(sea, GL_TRUE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///													Mountain                                                               ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// random hieghts for mountains:


void randomHeight(float mesh[90][90], int passes, int left, int right, int top, int bottom)
{
	if (++passes >= 6) return;

	int width = right - left;
	int height = bottom - top;

	mesh[left + (width / 2)][top + (height / 2)] += (2 * (GLfloat)rand() / RAND_MAX) / (passes);
	mesh[left][top + (height / 2)] += (2 * (GLfloat)rand() / RAND_MAX) / (passes);
	mesh[left + (width / 2)][top] += (2 * (GLfloat)rand() / RAND_MAX) / (passes);
	mesh[left + (width - 1)][top + (height / 2)] += (2 * (GLfloat)rand() / RAND_MAX) / (passes);
	mesh[left + (width / 2)][top + (height - 1)] += (2 * (GLfloat)rand() / RAND_MAX) / (passes);

	randomHeight(mesh, left, left + (width / 2), top, bottom - (height / 2), passes);
	randomHeight(mesh, left + (width / 2), right, top, bottom - (height / 2), passes);
	randomHeight(mesh, left, left + (width / 2), top + (height / 2), bottom, passes);
	randomHeight(mesh, left + (width / 2), right, top + (height / 2), bottom, passes);
}

// calculate normals for mountains:

void mountainNormalCalculator(float mesh[90][90], int x, int z) {

	Point upperLeft = normalCalculator(x, mesh[x][z], z, x, mesh[x][z - 1], z - 1, x - 1, mesh[x - 1][z], z);
	Point lowerLeft = normalCalculator(x, mesh[x][z], z, x, mesh[x][z + 1], z + 1, x - 1, mesh[x - 1][z], z);
	Point lowerRight = normalCalculator(x, mesh[x][z], z, x, mesh[x][z + 1], z + 1, x + 1, mesh[x + 1][z], z);
	Point upperRight = normalCalculator(x, mesh[x][z], z, x + 1, mesh[x + 1][z], z, x, mesh[x][z - 1], z - 1);

	float nx = (upperLeft.nx + lowerLeft.nx + lowerRight.nx + upperRight.nx) / 4.0f;
	float ny = (upperLeft.ny + lowerLeft.ny + lowerRight.ny + upperRight.ny) / 4.0f;
	float nz = (upperLeft.nz + lowerLeft.nz + lowerRight.nz + upperRight.nz) / 4.0f;

	glNormal3f(nx, ny, nz);

}

//initializing mountains
void initializing_Mountains() {

	//store the information in a glList
	mountainID = glGenLists(1);
	glNewList(mountainID, GL_COMPILE);

	glBindTexture(GL_TEXTURE_2D, mountainTexture);

	//distance and angle from origin( like a circle radius and angle)
	float originDistance; 
	float angle;

	//creating mountains
	forEvery(i, 3) {
		glPushMatrix();
		
		//mountains created at a random distance between 75 and 225 units
		originDistance = 100 + 150 * (GLfloat)rand() / RAND_MAX; 
		angle = ((365 / 4)  * i);

		

		//Scale mountains
		glScalef((30 + 100 * (GLfloat)rand() / RAND_MAX) / 100.0f,
				(30 + 100 * (GLfloat)rand() / RAND_MAX) / 100.0f,
				(30 + 100 * (GLfloat)rand() / RAND_MAX) / 100.0f
		);

		//get to mountain origin location

		glTranslatef(sin(angle) * originDistance, 0, cos(angle) * originDistance);
		
		//sink the mountains for realistic effect:
		glTranslatef(-10, 0, -10);

		float mesh[90][90];

		//initialize height
		forEvery(x, 90) {
			forEvery(z, 90) {
				GLfloat distance = sqrt((pow(45 - x, 2)) + sqrt((pow(45 - z, 2)))) * 0.9f;
				mesh[x][z] = (45 - distance) / 2.0f;
				if (mesh[x][z] < 0) mesh[x][z] = 0;
			}

		}
		
		// no elevation for outer edges
		forEvery(i, 90)
		{
			mesh[i][0] = 0.0;
			mesh[i][89] = 0.0;
			mesh[0][i] = 0.0;
			mesh[89][i] = 0.0;
		}

		//giving random vertices heights to get slopes to different parts of mountains
		randomHeight(mesh, 0, 1, 89, 1, 89); //within mesh bounds to avoid edges

		//drawing the mountain

		forEvery(x, 89) {
			forEvery(z, 89) {
				glBegin(GL_POLYGON);

				glNormal3f(0.0, 1.0, 0.0);
				glColor3f(1.0, 0.0, 0.0);

				//calculate normal
				// #1 x, z+1
				if (x != 0 && z != 0 && x != 89 && z != 89) {
					mountainNormalCalculator(mesh, x, z + 1);
				}
				glTexCoord2f(((GLfloat)x / 90), ((GLfloat)(z + 1) / 90));
				glVertex3f(x, mesh[x][z + 1], z + 1);

				// #2 x+1, z+1
				if (x != 0 && z != 0 && x != 89 && z != 89) {
					mountainNormalCalculator(mesh, x + 1, z + 1);
				}
				glTexCoord2f((((GLfloat)x + 1) / 90), ((GLfloat)(z + 1) / 90));
				glVertex3f(x + 1, mesh[x + 1][z + 1], z + 1);

				// #3 x+1, z
				if (x != 0 && z != 0 && x != 89 && z != 89) {
					mountainNormalCalculator(mesh, x + 1, z);
				} 
				glTexCoord2f(((GLfloat)(x + 1) / 90), ((GLfloat)z / 90));
				glVertex3f(x + 1, mesh[x + 1][z], z);

				// #4 x,z
				if (x != 0 && z != 0 && x != 89 && z != 89) {
					mountainNormalCalculator(mesh, x , z);
				}
				glTexCoord2f(((GLfloat)x / 90), ((GLfloat)z / 90));
				glVertex3f(x, mesh[x][z], z);

				glEnd(); //end drawing
			}

		}
		glPopMatrix();

	}
	//end the list
	glEndList();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///													Textures                                                              ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// taken from Brightspace

void readTexture(const char * filename, GLuint * ID) {

	int imageWidth, imageHeight;

	//temp char

	char tempChar;

	// array for header info

	char headerLine[100];

	//open image file to read

	FILE *fileID = fopen(filename, "r");

	//read header
	fscanf(fileID, "%[^\n] ", headerLine);

	// checking for ppm file (p3 check)
	if ((headerLine[0] != 'P') || (headerLine[1] != '3'))
	{
		printf("Selected file not a PPM file.\n");
		//exit(0);
	}
	fscanf(fileID, "%c", &tempChar);
	
	//reading comment lines
	while (tempChar == '#')
	{
		// read comment
		fscanf(fileID, "%[^\n] ", headerLine);
		// read in the next line first char
		fscanf(fileID, "%c", &tempChar);
	}

	// previous was not comment, undoing get
	ungetc(tempChar, fileID);
	
	// reading image
	int imageMaxValue;
	fscanf(fileID, "%d %d %d", &imageWidth, &imageHeight, &imageMaxValue);

	//enable texture state
	glEnable(GL_TEXTURE_2D);

	GLubyte *texture = (GLubyte *)malloc(3 * imageWidth * imageHeight * sizeof(GLubyte));

	//check if max value is above 255, if so, tell user we support only 255
	int c;
	if (imageMaxValue == 255) {
		forEvery(b, 3 * imageWidth * imageHeight)
		{
			fscanf(fileID, "%d", &c);
			texture[b] = (GLubyte)c;
		}
	}
	else printf("This program only support imageMaxValue of 255.");

	//close the file
	fclose(fileID);

	//create texture
	glGenTextures(1, ID);
	glBindTexture(GL_TEXTURE_2D, *ID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	//make mipmap
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, texture);
	
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///													Draw                                                              ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Origin draw ball and axis
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
	glutSolidSphere(0.6, 16, 16);
}


// draw flat grid in xz axis
void drawGrid()
{
	float x, y, z;

	glTranslatef(0, -0.3f, 0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	
	y = -0.3f;
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

//draw sky
void drawSky() {
	glTranslatef(0, 1000, 0);
	glRotatef(90, 1, 0, 0);

	GLfloat diffuseMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat ambientMaterial[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mShininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
	gluCylinder(sky, 500, 500, 500 * 2, 100, 100);
	glRotatef(-180.0, 0, 1, 0);
}

//draw sea

void drawSea() {
	gluQuadricNormals(sea, GLU_SMOOTH);
	gluQuadricTexture(sea, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, seaTexture);
	gluQuadricTexture(sea, seaTexture);
	glRotatef(270, 1, 0, 0);
	
	//basic coloe
	glColor4f(0.0, 0.0, 0.7, 1.0);
	
	//material properties
	GLfloat ambientMaterial[4] = { 0.2, 0.2, 0.4, 1.0 };
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, new GLfloat{ 50 });
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, lightgreen);
	gluDisk(sea, 0, 520, 60, 60);

}

void drawPropellers()
{
	glEnable(GL_COLOR_MATERIAL);
	// set material properties which will be assigned by glColor
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	
	glPushMatrix();
	glTranslatef(-0.01f, -0.14f, 0.35f);
	glRotatef(propellerRotation, 1, 0, 0);
	glTranslatef(0, 0.15f, -0.35f);
	glCallList(propellerID);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-0.01f, -0.14f, -0.35f);
    glRotatef(propellerRotation, 1, 0, 0);
	glTranslatef(0, 0.15f, -0.35f);
	glCallList(propellerID);
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);
}

void drawPlane()
{
	
	glPushMatrix();
	// move to where the enterprise will be drawn; in front of the camera
	glTranslatef(cameraPosition[0] + (5 * sin(cameraAngle)), cameraLookAt[1] - .6 + 6.5 * cameraVVelocity, cameraPosition[2] - (5 * cos(cameraAngle)));
	glScalef(1.4f, 1.4f, 1.4f);// scaling plane bigger
	glRotatef((180 / PI * -cameraAngle) - 90, 0, 1, 0); // orient the cessna to face away from the camera
	glTranslatef(-150 * cameraHVelocity, 0, -50 * cameraRotVelocity);
	glRotatef(-2000 * cameraRotVelocity, 0, 1, 0); // rotation
	glRotatef(-7500 * cameraRotVelocity, 1, 0, 0); // banking
	glRotatef(-800 * cameraVVelocity, 0, 0, 1); // up/ down
	
	glEnable(GL_COLOR_MATERIAL);

	//glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glCallList(cessnaID);
	glDisable(GL_COLOR_MATERIAL);
	drawPropellers();
	glPopMatrix();
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///													Display                                                             ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	// set the camera position
	gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2],
		cameraLookAt[0], cameraLookAt[1], cameraLookAt[2],
		0, 1, 0);

	initializeskyLight();

	if (gridView) {
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);

		drawAxis();
		drawGrid();

		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glTranslatef(0, -1, 0);

		if (fogToggle) glEnable(GL_FOG);

		drawSea();

		glDisable(GL_FOG);

		glPopMatrix();

		if (drawMountain)
		{
			glDisable(GL_CULL_FACE);
			if (mountainTextureToggle) glBindTexture(GL_TEXTURE_2D, mountainTexture);
			else glDisable(GL_TEXTURE_2D);

			glCallList(mountainID);
			glEnable(GL_CULL_FACE);
			glEnable(GL_TEXTURE_2D);
		}

		glPushMatrix();

		gluQuadricNormals(sky, GLU_SMOOTH);
		gluQuadricTexture(sky, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D, skyTexture);
		gluQuadricTexture(sky, skyTexture);
		glDisable(GL_CULL_FACE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, new GLfloat{ 50 });
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, yellow);

		drawSky();

		glEnable(GL_CULL_FACE);

		glPopMatrix();

	}

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, new GLfloat{ 10 });
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);

	
	drawPlane();
	
	
	glutSwapBuffers();
}

void moveCameraH(GLfloat amount)
{
	cameraPosition[0] -= amount * (cameraPosition[0] - cameraLookAt[0]);
	cameraPosition[2] -= amount * (cameraPosition[2] - cameraLookAt[2]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///													Keys                                                              ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void key(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'w':
			wireframe = !wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
			break;
		case 'k':
			cameraHVelocity -= 0.0002;
			break;
		case 'l':
			cameraHVelocity += 0.0002;
			break;
		case 'v':
			verticleMouseCtrl = !verticleMouseCtrl;
		case 'g':
			gridView= !gridView;
			break;
		case 'h':
			fogToggle = !fogToggle;
			break;
		case 'n':
			initializing_Mountains();
			break;
		case 't':
			mountainTextureToggle = !mountainTextureToggle;
			break;
		case 'f':
			if (fullscreenToggle) glutReshapeWindow(640, 480);
			else glutFullScreen();
			fullscreenToggle = !fullscreenToggle;
			break;
		case 'm':
			drawMountain = !drawMountain;
			break;
		case 'q':
			exit(0);
			break;

	}
}

void passiveMouse(int x, int y) {
	mouseX = x, mouseY = y; 
}

void sKey(int key, int x, int y)
{
	// add to or subtract from velocities and ensure they don't exceed bounds
	//if (key == GLUT_KEY_LEFT) camRotVel -= .0001;
	//if (key == GLUT_KEY_RIGHT) camRotVel += .0001;
	if (key == GLUT_KEY_DOWN) cameraVVelocity -= .005;
	if (key == GLUT_KEY_UP) cameraVVelocity += .005;
	

	if (cameraHVelocity > 0.01) cameraHVelocity = 0.01;
	else if (cameraHVelocity < 0.004) cameraHVelocity = 0.004;

	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///													Reshape                                                              ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)w / (float)h, 1, 10000);
	glMatrixMode(GL_MODELVIEW);
	w_Width = w, w_Height = w;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///													Idle                                                              ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void idle()
{
	// move the camera according to it's velocity, and decrease it's speed
	moveCameraH(cameraHVelocity);

	cameraAngle += cameraRotVelocity;

	cameraHeight += cameraVVelocity;
	cameraPosition[1] = cameraHeight;

	// update where the camera is looking
	cameraLookAt[0] = cameraPosition[0] + (10 * sin(cameraAngle));
	cameraLookAt[1] = cameraHeight + 30 *cameraVVelocity;
	cameraLookAt[2] = cameraPosition[2] - (10 * cos(cameraAngle));


	cameraVVelocity *= 0.989;

	cameraRotVelocity += -(GLfloat)(w_Width / 2 - mouseX) / 5000000;
	if (verticleMouseCtrl) cameraVVelocity += ((GLfloat)w_Height / 3 - mouseY) / 150000;

	// can bank harder when going slower
	if (abs(cameraRotVelocity) > 0.003 + 0.00001 / cameraHVelocity) cameraRotVelocity *= (1 - 200 * cameraHVelocity * abs(cameraRotVelocity));

	if (cameraVVelocity > 7.5 * cameraHVelocity) cameraVVelocity *= (1 - (GLfloat)cameraVVelocity / 25);
	else if (cameraVVelocity < -.1) cameraVVelocity = -.1;

	cameraVVelocity *= 0.989;
	cameraRotVelocity *= 0.990;



	propellerRotation += 3200 * cameraHVelocity;

	if (propellerRotation >= 360) propellerRotation = 0;

	

	glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///													Main                                                              ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char **argv)
{

	//srand(time(NULL));
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(w_Width, w_Height);
	glutCreateWindow("Flight Sim");
	// set CBs
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutPassiveMotionFunc(passiveMouse);
	glutKeyboardFunc(key);
	glutSpecialFunc(sKey);
	glutPassiveMotionFunc(passiveMouse);
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
	// read files and initialize stuff	

	//read textures:
	readTexture("sea02.ppm", &seaTexture);
	readTexture("mount03.ppm", &mountainTexture);
	readTexture("sky08.ppm", &skyTexture);

	// read objects
	readCessna();
	readPropeller();


	//initialize Things
	
	initializing_Mountains();

	//for fog

	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_MODE, GL_EXP);
	glHint(GL_FOG_HINT, GL_NICEST);
	glFogf(GL_FOG_DENSITY, 0.004f);


	// print controls
	printf("\n\nScene controls\n----------------\n\n");

	
	printf("\n\nCamera controls\n----------------\n\n");
	printf("UP:\tup\n");
	printf("DOWN:\tdown\n");
	printf("LEFT:\tleft\n");
	printf("RIGHT:\tright\n");
	printf("\n\nPlane Movement \n----------------\n\n");
	printf("l:\tforwards\n");
	printf("k :\tbackwards\n");
	printf("\n\nToggles \n----------------\n\n");
	printf("w :\tToggle wireframe\n");
	printf("g :\tToggle gridView\n");
	printf("v :\tVerticle mouse control\n");
	printf("f :\tFullscreen Toggle\n");
	printf("h :\tToggle fog\n");
	printf("n :\tInitialize Mountain\n");
	printf("m :\tDraw Mountains\n");
	printf("t :\tToggle Mountain Texture\n");
	printf("\n\nQuit \n----------------\n\n");
	printf("q :\tQuit\n");


	glutMainLoop();
}
