//main.cpp

// Extra feature: press g to turn the sun into a red giant (also added cruise control (speeds don't decrease when keys
// released), tilting spaceship, rings around saturn, a rather fancy shield, better star twinkling, and various other improvements)

#pragma warning(disable:4996) // allow use of unsafe methods fscanf and sscanf

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <glut.h>
#include <math.h>

// simplify for loops
#define forEach(i, n) for(int i = 0; i < n; i++)
#define forEachPlanet forEach(planet, 9)

const GLfloat PI = 3.14159;

// Where the camera is, where it's looking, how high in Y it is, and what direction it's pointing in
GLfloat camPos[] = { 200.0,40.0,200.0 };
GLfloat camAt[] = { 0.0,40.0,0.0 };
GLfloat camHeight = 40.0;
GLfloat camAngle = -PI / 4;

// Camera's velocity in XZ (trans.), Y, and XY (rot.)
GLfloat camHVel = 0.0;
GLfloat camVVel = 0.0;
GLfloat camRotVel = 0.0;

// FLAGS
bool fStars = true,
fOrbits = true,
fCorona = true,
fShield = true,
fCruise = false, // cruise control
fRG = false; // red giant mode

			 // Planet data
const GLint PLANET_RADII[] = { 5, 8, 9, 6, 17, 11, 9, 8, 3 };
const GLint PLANET_ORBITS[] = { 70, 100, 160, 220, 300, 370, 450, 520, 640 }; // radius of orbit (distance from sun)
GLfloat planetAngles[] = { 0, PI / 2, PI / 4, PI, 3 * PI / 4, PI / 2, 0, PI, PI / 4 }; // initialized to various values
GLfloat planetVels[] = { 0.006, 0.004, 0.003, 0.002, 0.001, 0.0008, 0.0006, 0.0005, 0.0004 };

// Moon data
const GLint MOON_ORBIT = 20;
const GLint MOON_RADIUS = 3;
GLfloat moonAngle = 0.0;
GLfloat moonVel = 0.008;

typedef GLfloat colour[3];
const colour PLANET_COLOURS[] =
{
	{ 0.8, 0.4, 0.2 },
{ 1.0, 0.4, 0.1 },
{ 0.2, 0.3, 0.8 },
{ 1.0, 0.2, 0.2 },
{ 0.8, 0.5, 0.3 },
{ 0.7, 0.6, 0.3 },
{ 0.3, 0.9, 0.7 },
{ 0.1, 0.7, 0.9 },
{ 0.3, 0.5, 0.6 }
};

typedef GLint point[3];

const GLint STAR_COUNT = 5000; // how many stars to draw
point stars[STAR_COUNT]; // where to draw them

						 // bounds on how far form the origin stars can be
const GLint MIN_STAR_DIST = 2000;
const GLint MAX_STAR_DIST = 5000;

/*
the corona is comprised of lines, which are drawn around the sun. the drawing function takes care of distributing them around the sun
and requires only the length of each line. these length vary over time to give a fiery effect; each line is controlled by a master LFO
but has independent multipliers for the speed at which they're changed and how much they're changed by
*/
const GLint CORONA_COUNT = 750;  // how many lines to draw for the corona
GLfloat corona[CORONA_COUNT][3]; // line length, lfo speed multiplier, lfo depth
GLfloat coronaTheta = 0.01; // master LFO value
GLfloat coronaSpeed = 0.01; // master LFO speed

const GLint MAX_CORONA_DIST = 40; // min is implicitly the radius of the sun
const GLfloat MIN_CORONA_SPEED = 1.0, MAX_CORONA_SPEED = 3.0, MIN_CORONA_DEPTH = 1, MAX_CORONA_DEPTH = 15;

// not const because they change in red giant mode
GLfloat sunRadius = 35;
colour sunColour = { 1.0, 0.8, 0.2 };

// Enterprise data
GLint eNumVertices = 0; // updated when reading in enterprise.txt
GLint eNumFaces = 0;

GLfloat eVertices[5000][3]; // a sufficiently large allotment of memory for vertices
GLint eFaces[5000][3]; // ditto for faces

const GLint E_SCALE = 20; // how much to scale the enterprise by

GLfloat innerShieldTheta = 0.0, innerShieldSpeed = 0.02; // LFO for the shield
GLfloat outerShieldTheta = 0.0, outerShieldSpeed = 0.005;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int initEnterprise()
{
	FILE *inFile = fopen("enterprise.txt", "r"); // open the file
	if (!inFile) return 1 + printf("error reading enterprise.txt"); // report any issues, returning
	char inStr[100]; // string buffer for line of text in file
	int vIndex = 0, fIndex = 0; // vertex, face indices
	while (fgets(inStr, sizeof(inStr), inFile)) // while there's more input to parse
	{
		if (inStr[0] == 'v') // vertex
		{   // add the vertex to the vertex array
			sscanf(inStr, "v %f %f %f", &eVertices[vIndex][0], &eVertices[vIndex][1], &eVertices[vIndex][2]);
			vIndex++;
		}
		else if (inStr[0] == 'f') // face
		{   // add the face to the face array
			sscanf(inStr, "f %d %d %d", &eFaces[fIndex][0], &eFaces[fIndex][1], &eFaces[fIndex][2]);
			fIndex++;
		}
	}
	eNumVertices = vIndex, eNumFaces = fIndex;
}

// determine where the stars will be placed.
void initStars()
{
	forEach(star, STAR_COUNT)
	{
		do
		{
			forEach(i, 3) stars[star][i] = (int)(MAX_STAR_DIST * ((GLfloat)rand() / RAND_MAX)) * (((GLfloat)rand() / RAND_MAX > 0.5) ? 1 : -1);
		} // while there are stars closer than MIN_STAR_DIST units to the origin (the sun) (not super efficient but easy and only executed once on start)
		while (pow(stars[star][0], 2) + pow(stars[star][2], 2) < pow(MIN_STAR_DIST, 2));
	}
}

// set values for corona line length and lfo properties.
void initCorona()
{
	forEach(line, CORONA_COUNT)
	{
		corona[line][0] = (int)((MAX_CORONA_DIST - sunRadius) * ((GLfloat)rand() / RAND_MAX)) * (((GLfloat)rand() / RAND_MAX > 0.5) ? 1 : -1);
		corona[line][1] = MIN_CORONA_SPEED + ((GLfloat)rand() / RAND_MAX) * (MAX_CORONA_SPEED - MIN_CORONA_SPEED);
		corona[line][2] = MIN_CORONA_DEPTH + ((GLfloat)rand() / RAND_MAX) * (MAX_CORONA_DEPTH - MIN_CORONA_DEPTH);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void drawEnterprise()
{
	glPushMatrix();
	// move to where the enterprise will be drawn; in front of the camera
	glTranslatef(camPos[0] + (100 * sin(camAngle)), camAt[1] - 25, camPos[2] - (100 * cos(camAngle)));
	GLfloat shade; // shade of the current face, increases with every face from 0.2 to 0.8 (greys)
	forEach(face, eNumFaces)
	{
		glPushMatrix();
		shade = 0.2 + 0.6 * ((GLfloat)face / eNumFaces);
		glColor3f(shade, shade, shade);
		glRotatef(180 / PI * -camAngle, 0, 1, 0); // orient the enterprise to face away from the camera
		glRotatef(-5000 * camRotVel, 0, 0, 1); // tilt the enterprise with rotation
		glRotatef(25 * camVVel, 1, 0, 0); // tilt the enterprise with vertical movement
		glBegin(GL_TRIANGLES); // draw the triangles with the vertices specified by the faces
		glVertex3i((int)E_SCALE * eVertices[eFaces[face][0]][0], (int)E_SCALE * eVertices[eFaces[face][0]][1], (int)E_SCALE * eVertices[eFaces[face][0]][2]);
		glVertex3i((int)E_SCALE * eVertices[eFaces[face][1]][0], (int)E_SCALE * eVertices[eFaces[face][1]][1], (int)E_SCALE * eVertices[eFaces[face][1]][2]);
		glVertex3i((int)E_SCALE * eVertices[eFaces[face][2]][0], (int)E_SCALE * eVertices[eFaces[face][2]][1], (int)E_SCALE * eVertices[eFaces[face][2]][2]);
		glEnd();
		glPopMatrix();
	}
	glPopMatrix();
}

// simple sphere at the origin
void drawSun()
{
	glColor3f(sunColour[0], sunColour[1], sunColour[2]);
	glutSolidSphere(sunRadius, 2 * sunRadius, sunRadius / 2);
}

void drawPlanets()
{
	forEachPlanet
	{
		glColor3f(PLANET_COLOURS[planet][0], PLANET_COLOURS[planet][1], PLANET_COLOURS[planet][2]);
	glPushMatrix();
	// move to where the planet currently is
	glTranslatef(PLANET_ORBITS[planet] * sin(planetAngles[planet]), 0.0, PLANET_ORBITS[planet] * cos(planetAngles[planet]));
	glutSolidSphere(PLANET_RADII[planet], 100, 50);
	if (planet == 5) // Saturn; draw rings
	{
		glColor3f(PLANET_COLOURS[5][0] + 0.2, PLANET_COLOURS[5][1] + 0.1, PLANET_COLOURS[5][2] + 0.1);
		glRotatef(90, 0.8, 0.0, 0.2);
		gluDisk(gluNewQuadric(), 4 + PLANET_RADII[5], 12 + PLANET_RADII[5], 20, 8);
	}
	glPopMatrix();
	}
}

// same idea as drawing the planets, but with a more complicated translation that takes into account where Earth is and where around it the moon is
void drawMoon()
{
	glPushMatrix();
	glColor3f(0.6, 0.6, 0.7);
	glTranslatef(PLANET_ORBITS[2] * sin(planetAngles[2]) + MOON_ORBIT * sin(moonAngle), 0.0, PLANET_ORBITS[2] * cos(planetAngles[2]) + MOON_ORBIT * cos(moonAngle));
	glutSolidSphere(MOON_RADIUS, 20, 10);
	glPopMatrix();
}

// orbits are disks with inner radius slightly smaller than outer radius
void drawOrbits()
{
	glColor3f(1.0, 1.0, 1.0); // set colour to white
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // switch to line mode (orbits just look better in line mode, that's all)
	glPushMatrix();
	glRotatef(90, 1.0, 0.0, 0.0); // orient the orbits in the XZ plane
								  // draw the planets
	forEachPlanet gluDisk(gluNewQuadric(), PLANET_ORBITS[planet] - 0.1, PLANET_ORBITS[planet] + 0.1, 50 + PLANET_ORBITS[planet] / 10, 5);
	glPopMatrix();
	glPushMatrix();
	// Move to Earth to draw the orbit for the moon
	glTranslatef(PLANET_ORBITS[2] * sin(planetAngles[2]), 0.0, PLANET_ORBITS[2] * cos(planetAngles[2]));
	glColor3f(1.0, 1.0, 1.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	gluDisk(gluNewQuadric(), MOON_ORBIT - 0.1, MOON_ORBIT + 0.1, 25, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // switch back to fill mode
	glPopMatrix();
}

void drawStars()
{
	glBegin(GL_POINTS);
	forEach(star, STAR_COUNT)
	{
		GLfloat brightness = 0.6 + 0.4 * (GLfloat)rand() / RAND_MAX; // randomize brightness and colour to make stars twinkle
		glColor3f(brightness * 1.0 - ((GLfloat)rand() / (RAND_MAX)) / 3, brightness * 1.0 - ((GLfloat)rand() / (RAND_MAX)) / 3, brightness * 1.0 - ((GLfloat)rand() / (RAND_MAX)) / 3);
		glVertex3i(stars[star][0], stars[star][1], stars[star][2]);
	}
	glEnd();
}

// lines are drawn in XY plane and rotated around the Y axis to face the camera
void drawCorona()
{
	glEnable(GL_BLEND); // enable blending for transparent lines
	glLineWidth(2.0); // make corona lines a bit thicker to help blur them into a nice corona
	forEach(line, CORONA_COUNT)
	{
		glPushMatrix();
		glRotatef(((GLfloat)180 / PI) * atan2((GLfloat)camPos[0], (GLfloat)camPos[2]), 0.0, 1.0, 0.0); // make the corona face the camera
		glRotatef((GLfloat)360 * ((GLfloat)line / CORONA_COUNT), 0.0, 0.0, 1.0); // rotate around the sun, evenly distributing each corona line
		glBegin(GL_LINES);
		glColor4f(sunColour[0], sunColour[1], sunColour[2], 1.0);
		// starting at the origin lets the lines become a different colour than the sun by the time they reach the surface, preserving the visibility
		// of the edge of the sun itself and giving it a more fiery appearance;
		glVertex3f(0.0, 0.0, 0.0);
		glColor4f(1.0f, 0.6f, 0.2f, 0.0f);
		glVertex3f(corona[line][0], 50 + corona[line][2] * sin(coronaTheta * corona[line][1]), 0);
		glEnd();
		glPopMatrix();
	}
	glLineWidth(1.0);
	glDisable(GL_BLEND);
}

void drawShield()
{
	glPushMatrix();
	glTranslatef(camPos[0] + (100 * sin(camAngle)), camAt[1] - 25, camPos[2] - (100 * cos(camAngle)));
	glPushMatrix();
	// draw a transparent sphere around the enterprise that rotates, exploiting an interesting glitch in transparency
	glRotatef(((GLfloat)180 / PI) * innerShieldTheta, 1, 1, 0);
	glEnable(GL_BLEND); // enable blending for transparency
	glColor4f(0.2f, 0.4f, 0.8 + 0.1 * sin(10 * innerShieldTheta), 0.2f + 0.01 * sin(3 * outerShieldTheta));
	glutSolidSphere(15, 300, 150);
	glPopMatrix();
	glRotatef(((GLfloat)180 / PI) * outerShieldTheta, 0, 1, 0);
	glColor4f(0.3f + 0.1 * sin(6 * innerShieldTheta), 0.4f, 0.9f + 0.1 * cos(4 * outerShieldTheta), 0.6f + 0.1 * cos(2 * outerShieldTheta));
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glutSolidSphere(16, 20, 10);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_BLEND);
	glPopMatrix();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	// set the camera position
	gluLookAt(camPos[0], camPos[1], camPos[2],
		camAt[0], camAt[1], camAt[2],
		0, 1, 0);

	drawSun();
	drawPlanets();
	drawMoon();
	drawEnterprise();
	if (fStars)  drawStars();
	if (fOrbits) drawOrbits();
	if (fCorona) drawCorona();
	if (fShield) drawShield();

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
	case 's':
		fStars = !fStars;
		break;
	case 'r':
		fOrbits = !fOrbits;
		break;
	case 'c':
		fCorona = !fCorona;
		break;
	case 'k':
		fShield = !fShield;
		break;
	case 'z':
		fCruise = !fCruise;
		break;
	case 'g':
		fRG = true;
		break;
	}
}

void sKey(int key, int x, int y)
{
	// add to or subtract from velocities and ensure they don't exceed bounds
	if (key == GLUT_KEY_LEFT) camRotVel -= .0001;
	if (key == GLUT_KEY_RIGHT) camRotVel += .0001;
	if (key == GLUT_KEY_DOWN) camVVel -= .02;
	if (key == GLUT_KEY_UP) camVVel += .02;
	if (key == GLUT_KEY_PAGE_DOWN) camHVel -= 0.002;
	if (key == GLUT_KEY_PAGE_UP) camHVel += 0.002;

	if (camHVel > 0.2) camHVel = 0.2;
	else if (camHVel < -0.2) camHVel = -0.2;

	if (camRotVel > 0.03) camRotVel = 0.03;
	else if (camRotVel < -0.03) camRotVel = -0.03;

	if (camVVel > 1) camVVel = 1;
	else if (camVVel < -1) camVVel = -1;
}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)w / (float)h, 1, 10000);
	glMatrixMode(GL_MODELVIEW);
}

void idle()
{
	// move the camera according to it's velocity, and decrease it's speed
	moveCamH(camHVel);
	camHVel *= fCruise ? 1 : 0.995;

	camAngle += camRotVel;
	camRotVel *= fCruise ? 1 : 0.995;

	camHeight += camVVel;
	camPos[1] = camHeight;
	camVVel *= fCruise ? 1 : 0.995;

	// update where the camera is looking
	camAt[0] = camPos[0] + (10 * sin(camAngle));
	camAt[1] = camHeight;
	camAt[2] = camPos[2] - (10 * cos(camAngle));

	// move the planets around their orbits
	forEachPlanet
	{
		planetAngles[planet] += planetVels[planet];
	if (planetAngles[planet] >= 2 * PI) planetAngles[planet] = 0;
	}

		// ditto, moon
	moonAngle += moonVel;
	if (moonAngle >= 2 * PI) moonAngle = 0;

	// using 4096 * PI because multipliers make for jarring jumps, so make them happen less often

	// corona LFO
	coronaTheta += coronaSpeed;
	if (coronaTheta >= 4096 * PI) coronaTheta = 0;

	// shield LFOs

	innerShieldTheta += innerShieldSpeed;
	if (innerShieldTheta >= 4096 * PI) innerShieldTheta = 0;

	outerShieldTheta += outerShieldSpeed;
	if (outerShieldTheta >= 4096 * PI) outerShieldTheta = 0;

	if (fRG)
	{
		if (sunColour[1] > 0.2) sunColour[1] -= 0.001;
		if (sunColour[2] > 0.0) sunColour[2] -= 0.002;
		if (sunRadius < 200) sunRadius += 0.1;
	}

	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow("A00396139 A02");
	// set CBs
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
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
	// init enterprise and randoms
	initEnterprise();
	initStars();
	initCorona();
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
	printf("PG DNs:\tbackwards\n");
	// blast off
	glutMainLoop();
}
