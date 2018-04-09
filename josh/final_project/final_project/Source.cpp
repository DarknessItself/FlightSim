#pragma warning(suppress : 4996)
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <freeglut.h>
#include <stdio.h>
#include <iostream>
#include <cmath>

#define PI 3.14159265
#define DEG_TO_RAD PI/180.0
#define GRID_SIZE 100
//window variable
int windowWidth = 1080, windowHeight = 800;

//mouse coods
int mouseX = windowWidth/2;

//texture variables
typedef GLubyte Pixel[3];
int Width, Height;
Pixel *Image;
int create = 1;
GLuint sea, sky, mount;

//camera
GLfloat eyex = 0, eyey = 2.5, eyez = 5.0f, centerx = 0, centery = 2, centerz = -1.0f;
float angle = 0, speed = 0.001;

//contorls
int wire = 1, seaSky = 0;

//plane data


void idle(void) {
	float tempx = centerx - eyex;
	float tempz = centerz - eyez;

	eyex = eyex + tempx * speed;
	eyez = eyez + tempz * speed;

	centerx = centerx + tempx * speed;
	centerz = centerz + tempz * speed;

	if (mouseX < ((windowWidth / 2) - 100)) {
		centerz = (float)(eyez + sin(-speed) * tempx + cos(-speed) * tempz);
		centerx = (float)(eyex + cos(-speed) * tempx - sin(-speed) * tempz);
	}
	else if (mouseX > ((windowWidth / 2) + 100)) {
		centerz = (float)(eyez + sin(speed) * tempx + cos(speed) * tempz);
		centerx = (float)(eyex + cos(speed) * tempx - sin(speed) * tempz);
	}
	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, 0.0, 1.0, 0.0);
	glutPostRedisplay();
}



void readImage(const char* filename){
	int w, h, max;
	int i, j;
	unsigned int r, g, b;
	int k;
	char ch;
	FILE *fp;

	fp = fopen(filename, "r");

	fscanf(fp, "P%c\n", &ch);
	if (ch != '3') {
		fprintf(stderr, "Only ascii mode 3 channel PPM files");
		exit(-1);
	}

	ch = getc(fp);
	while (ch == '#') {
		do {
			ch = getc(fp);
		} while (ch != '\n');
		ch = getc(fp);
	}
	ungetc(ch, fp);

	fscanf(fp, "%d", &w);
	fscanf(fp, "%d", &h);
	fscanf(fp, "%d", &max);

	Width = w;
	Height = h;

	printf("Width = %d, Height = %d\n", Width, Height);

	Image = (Pixel *)malloc(Width*Height * sizeof(Pixel));


	for (i = 0; i<Height; ++i) {
		for (j = 0; j<Width; ++j) {
			fscanf(fp, "%d %d %d", &r, &g, &b);
			k = i * Width + j; 
			(*(Image + k))[0] = (GLubyte)r;
			(*(Image + k))[1] = (GLubyte)g;
			(*(Image + k))[2] = (GLubyte)b;
		}
		for (j = Width; j<w; ++j) fscanf(fp, "%c %c %c", &r, &g, &b);
	}
	fclose(fp);

	switch (create) {
		case 1: {
			glGenTextures(1, &sea);
			glBindTexture(GL_TEXTURE_2D, sea);
			break;
		}
		case 2: {
			glGenTextures(2, &sky);
			glBindTexture(GL_TEXTURE_2D, sky);
			break;
		}
		case 3: {
			glGenTextures(3, &mount);
			glBindTexture(GL_TEXTURE_2D, mount);
			break;
		}
	}
	
}


void createTexture(const char* filename)
{
	GLint level = 0;
	GLint components = 3; 
	GLint border = 0;

	readImage(filename);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, Width, Height,
		GL_RGB, GL_UNSIGNED_BYTE, Image);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glEnable(GL_TEXTURE_2D);
}
void drawBaseScene(void) {
	typedef GLfloat pointVec[3];
	pointVec xVec = { 1.5, 0.0, 0.0 };
	pointVec yVec = { 0.0, 1.5, 0.0 };
	pointVec zVec = { 0.0, 0.0, -1.5 };
	GLfloat x, y;
	glLineWidth(5);
	glPushMatrix();
		glTranslatef(0.0, 0.05, 0.0);
		glColor3f(1.0, 1.0, 1.0);
		glutSolidSphere(0.1, 5, 10);
		glBegin(GL_LINES);
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3fv(xVec);

			glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3fv(yVec);

			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3fv(zVec);
		glEnd();
	glPopMatrix();
	glLineWidth(1);
	glPushMatrix();
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glBegin(GL_QUADS);
		glColor3f(0.2, 0.2, 0.5);
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
void drawSea(void) {
	if (create == 1) {
		createTexture("sea02.ppm");
		create++;
	}
	glBindTexture(GL_TEXTURE_2D, sea);
	GLUquadricObj* quadricPtr;
	quadricPtr = gluNewQuadric();
	gluQuadricDrawStyle(quadricPtr, GLU_FILL);
	gluQuadricNormals(quadricPtr, GLU_SMOOTH);
	gluQuadricTexture(quadricPtr, GL_TRUE);
	glPushMatrix();
		glTranslatef(0.0, -2.0, 0.0);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		gluDisk(quadricPtr, 0, 100, 20, 50);
	glPopMatrix();
}

void drawSky(void) {
	if (create == 2) {
		createTexture("sky08.ppm");
		create++;
	}
	glBindTexture(GL_TEXTURE_2D, sky);
	GLUquadricObj* quadricPtr;
	quadricPtr = gluNewQuadric();
	gluQuadricDrawStyle(quadricPtr, GLU_FILL);
	gluQuadricNormals(quadricPtr, GLU_SMOOTH);
	gluQuadricTexture(quadricPtr, GL_TRUE);
	glPushMatrix();
	glTranslatef(0.0, 48.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	gluCylinder(quadricPtr, 100, 100, 50, 20, 20);
	glPopMatrix();
}

void display(void) {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (wire) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (seaSky) {

		glEnable(GL_TEXTURE_2D);
		drawSky();
		drawSea();
	}
	else {

		glDisable(GL_TEXTURE_2D);
		drawBaseScene();
	}

	glutSwapBuffers();
}

void speKeyboard(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		centery += 0.1;
		eyey += 0.1;
		break;
	case GLUT_KEY_DOWN:
		centery -= 0.1;
		eyey -= 0.1;
		break;
	case GLUT_KEY_LEFT:
		
		break;
	case GLUT_KEY_RIGHT:
		
		break;
	case GLUT_KEY_PAGE_UP:
		speed /= 0.5;
		break;
	case GLUT_KEY_PAGE_DOWN:
		speed *= 0.5;
		break;
	default:
		break;
	}
	printf("%f, %f \n", eyez, centerz);
	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez, eyex+centerx, centery, eyez+centerz, 0.0, 1.0, 0.0);
	glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		wire = !wire;
		break;
	case 's':
		seaSky = !seaSky;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}
void mouse(int x, int y) {
	mouseX = x;
	printf("%d :: %d\n", x, y);
}
void init(void) {
	glViewport(0, 0, windowWidth, windowHeight);
	//glShadeModel(GL_FLAT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(10.0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, windowWidth / windowHeight, 5, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez, eyex+centerx, centery, eyez+centerz, 0.0, 1.0, 0.0);
	glClearDepth(1.0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Assignment 2");
	glutDisplayFunc(display);
	glutSpecialFunc(speKeyboard);
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutIdleFunc(idle);
	init();
	glutMainLoop();
	return 0;
}