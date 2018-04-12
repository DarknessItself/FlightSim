//ver1

//Flight Sim
#pragma warning(disable:4996) // allow use of unsafe methods fscanf and sscanf

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <glut.h>
#include <math.h>

// simplify for loops
#define forEach(i, n) for(int i = 0; i < n; i++)

const GLfloat PI = 3.14159;
int w_Width = 640, w_Height = 480;

int mouseX, mouseY;

// Where the camera is, where it's looking, how high in Y it is, and what direction it's pointing in
GLfloat camPos[] = { 10.0,10.0,20.0 };
GLfloat camAt[] = { 0.0,0.0,0.0 };
GLfloat camHeight = 10.0;
GLfloat camAngle = -PI / 4;

// Camera's velocity in XZ (trans.), Y, and XY (rot.)
GLfloat camHVel = 0.002;
GLfloat camVVel = 0.0;
GLfloat camRotVel = 0.0;

const GLfloat gridSize = 100.0f;

//vMouseCtrl toggles if you can control the verticle motion of the plane with mouse
bool wireframe = false, gridView = false, vMouseCtrl= false;



GLfloat propTheta = 0;

int planeId, propId;



GLUquadric *sky = gluNewQuadric();

GLfloat ambientLight0[]  = { 0.8, 0.8, 0.8 };
GLfloat diffuseLight0[]  = { 1, 1, 1 };
GLfloat specularLight0[] = { 1, 1, 1 };
GLfloat lightPosition0[] = { 0, 0.3, -1, 0 };
GLfloat none[]			 = { 0,0,0,0 };
GLfloat dull[]		     = { 1 };
GLfloat littleshiny[]    = { 10 };
GLfloat shiny[]		     = { 50 };

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


GLint readCessna() {
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


GLint readPropeller()
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///												Setting up lights                                                     ///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void initialize_Light0() {

	//set up lighting 
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);

	glShadeModel(GL_SMOOTH);

	//enable GL states
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat diffuseMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat ambientMaterial[4] = { 0.15, 0.15, 0.15, 1.0 };
	GLfloat specularMaterial[4] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat emissiveMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };


	// putting attributes to materials
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, new GLfloat{1});







}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///													Sky                                                               ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void initializing_sky() {

	gluQuadricNormals(sky, GLU_SMOOTH);
	gluQuadricTexture(sky, GL_TRUE);
}



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

void drawGrid()
{
	glTranslatef(0, -0.5f, 0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	float x, y, z;
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

void drawPropellers()
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
	// move to where the enterprise will be drawn; in front of the camera
	glTranslatef(camPos[0] + (5 * sin(camAngle)), camAt[1] - .6 + 6.5 * camVVel, camPos[2] - (5 * cos(camAngle)));
	glRotatef((180 / PI * -camAngle) - 90, 0, 1, 0); // orient the cessna to face away from the camera
	glTranslatef(-150 * camHVel, 0, -50 * camRotVel);
	glRotatef(-7500 * camRotVel, 1, 0, 0); // banking
	glRotatef(-600 * camVVel, 0, 0, 1); // up/ down
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glCallList(planeId);
	glDisable(GL_COLOR_MATERIAL);
	drawPropellers();
	glPopMatrix();
}

void drawSky() {

	glTranslatef(0, (200) - 2.0f, 0);
	glRotatef(90, 1, 0, 0);

	GLfloat diffuseMaterial[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat ambientMaterial[4] = { 1.0, 1.0, 1.0, 1.0 };
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, new GLfloat{ 50 });
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
	gluCylinder(sky, 290, 290, 290 * 2, 150, 150);

	glRotatef(-180, 0, 1, 0);

	glScalef(1.01, 1, 1.01);

	gluCylinder(sky, 290, 290, 290 * 1.99f, 150, 150);

}



void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	// set the camera position
	gluLookAt(camPos[0], camPos[1], camPos[2],
		camAt[0], camAt[1], camAt[2],
		0, 1, 0);

	initialize_Light0();

	if (gridView) {

	drawAxis();
	drawGrid();

	}
	else {


	}
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
			glPolygonMode(GL_FRONT, wireframe ? GL_LINE : GL_FILL);
			break;
		case 'k':
			camHVel -= 0.0002;
			break;
		case 'l':
			camHVel += 0.0002;
			break;
		case 'v':
			vMouseCtrl = !vMouseCtrl;
		case 's':
			gridView= !gridView;
			break;
	}
}

void pMouse(int x, int y) {
	mouseX = x, mouseY = y; 
}

void sKey(int key, int x, int y)
{
	// add to or subtract from velocities and ensure they don't exceed bounds
	//if (key == GLUT_KEY_LEFT) camRotVel -= .0001;
	//if (key == GLUT_KEY_RIGHT) camRotVel += .0001;
	if (key == GLUT_KEY_DOWN) camVVel -= .0005;
	if (key == GLUT_KEY_UP) camVVel += .0005;
	if (key == GLUT_KEY_PAGE_DOWN) camHVel -= 0.0002;
	if (key == GLUT_KEY_PAGE_UP) camHVel += 0.0002;

	if (camHVel > 0.02) camHVel = 0.02;
	else if (camHVel < 0.002) camHVel = 0.002;

	if (camRotVel > 0.005 + 0.000005 / camHVel) camRotVel = 0.005 + 0.000005 / camHVel;
	else if (camRotVel < -0.005 - 0.000005 / camHVel) camRotVel = -0.005 - 0.000005 / camHVel;

	if (camVVel > .07) camVVel = .07;
	else if (camVVel < -.07) camVVel = -.07;
}


void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)w / (float)h, 1, 10000);
	glMatrixMode(GL_MODELVIEW);
	w_Width = w, w_Height = w;
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
	camAt[1] = camHeight;
	camAt[2] = camPos[2] - (10 * cos(camAngle));

	camVVel *= 0.989;

	camRotVel = -(GLfloat)(w_Width / 2 - mouseX) / 100000;
	if (vMouseCtrl) camVVel = (GLfloat)(w_Height / 3 - mouseY) / 10000;
	if (camRotVel > 0.002 + 0.00001 / camHVel) camRotVel = 0.002 + 0.00001 / camHVel;
	else if (camRotVel < -0.002 - 0.00001 / camHVel) camRotVel = -0.002 - 0.00001 / camHVel;
	if (camVVel > .07) camVVel = .07;
	else if (camVVel < -.07) camVVel = -.07;



	propTheta += 3200 * camHVel;

	if (propTheta >= 360) propTheta = 0;

	

	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(w_Width, w_Height);
	glutCreateWindow("Flight Sim");
	// set CBs
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutPassiveMotionFunc(pMouse);
	glutKeyboardFunc(key);
	glutSpecialFunc(sKey);
	// init params
	glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 640.0 / 480.0, 1, 10000);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glMatrixMode(GL_MODELVIEW);
	// read files and initialize stuff	
	readCessna();
	readPropeller();
	initialize_Light0();
	// print controls
	printf("\n\nScene controls\n----------------\n\n");
	printf("r:\trings\n");
	printf("c:\tcorona\n");
	printf("k:\tshield\n");
	printf("z:\tcruise control\n");
	printf("g:\tred giant\n");
	printf("\n\nCamera controls\n----------------\n\n");
	printf("UP:\tup\n");
	printf("DOWN:\tdown\n");
	printf("LEFT:\tleft\n");
	printf("RIGHT:\tright\n");
	printf("l:\tforwards\n");
	printf("k :\tbackwards\n");
	printf("s :\ttoggle grid view\n");

	glutMainLoop();
}
