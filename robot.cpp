#define _CRT_SECURE_NO_WARNINGS
#define PI 3.141592654

#ifdef _WIN32
	#include <windows.h>
#endif

#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include "RgbImage.h"

using namespace std;

char filenameTexMetal1[] = "./metalTexture1.bmp";

GLuint _textureIdMetal1;
GLuint _textureIdSphere;
GLuint _textureIdCylinder;
GLUquadric *quadSphere;
GLUquadric *quadCylinder;

GLfloat fov;

bool textureOn = true;

float viewAngleX = 0.0;
float viewAngleZ = 15.0;

int tick = 0;

bool dancing = 0;
int danceStart = 0;

struct Claw {
	float angleArm, angleForearm, angleClamp;
};

Claw leftClaw = {
	.angleArm = 270.,
	.angleForearm = 90.,
	.angleClamp = 0.
};
	
Claw rightClaw = {
	.angleArm = 90.,
	.angleForearm = 90.,
	.angleClamp = 0.
};

float clampIncrement = 3.;

//makes the image into a texture, and returns the id of the texture
GLuint loadTexture(char *filename) {
	GLuint textureId;

	RgbImage theTexMap(filename); // image with texture

	glGenTextures(1, &textureId); // make room for our texture
	
	// tell OpenGL which texture to edit
	// map the image to the texture
	glBindTexture(GL_TEXTURE_2D, textureId);
	
	glTexImage2D(GL_TEXTURE_2D,
		0, // No mipmap
		GL_RGB, // format OpenGL uses for image
		theTexMap.GetNumCols(), // width
		theTexMap.GetNumRows(), // height
		0, // image border
		GL_RGB, // pixels are stored in RGB format
		GL_UNSIGNED_BYTE, // pixels are stored as unsigned numbers
		theTexMap.ImageData()); // pixel data
	
	return textureId;
}

void initRendering(void) {
	quadSphere = gluNewQuadric();
	quadCylinder = gluNewQuadric();
	_textureIdMetal1 = loadTexture(filenameTexMetal1);
	_textureIdCylinder = _textureIdMetal1;
	_textureIdSphere = _textureIdMetal1;
}

void enableLigthing(void) {
	// Ligth source
	GLfloat ambientLight[4] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat difuseLigth[4] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat especularLight[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat ligthPosition[4] = { 0.0, 50.0, 50.0, 1.0 };

	// Eneble ligthing primitive
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	// Set the parameters
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, difuseLigth);
	glLightfv(GL_LIGHT0, GL_SPECULAR, especularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, ligthPosition);

	// Material properties
	GLfloat espec[4] = { 1.0,1.0,1.0,1.0 };
	GLint especMaterial = 50;
	glMaterialfv(GL_FRONT, GL_SPECULAR, espec);
	glMateriali(GL_FRONT, GL_SHININESS, especMaterial);
	glShadeModel(GL_SMOOTH); 
	glEnable(GL_COLOR_MATERIAL); 
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	fov = 50;
}

void moveClamp(Claw *claw) {
	
	if (claw->angleClamp >= 60)
		clampIncrement = -3.;
	
	if (claw->angleClamp <= 0)
		clampIncrement = +3.;

	claw->angleClamp += clampIncrement;
}

void handleSpecialKeyPress(int key, int x, int y) {
	Claw *left = &leftClaw;
	Claw *right = &rightClaw;
	
	//Comandos de câmera
	switch (key) {
	case GLUT_KEY_UP: //Decrease view angle z axis
		viewAngleZ = fmax(viewAngleZ - 3, 0);
		break;
	case GLUT_KEY_LEFT: //Decrease view angle x axis
		viewAngleX = fmod(viewAngleX + 360 - 3, 360);
		break;
	case GLUT_KEY_DOWN: //Increase view angle z axis
		viewAngleZ = fmin(viewAngleZ + 3, 180);
		break;
	case GLUT_KEY_RIGHT: //Increase view angle x axis
		viewAngleX = fmod(viewAngleX + 3, 360);
		break;
	}
	glutPostRedisplay();
}

void handleKeypress(unsigned char key, int x, int y) {
	Claw *left = &leftClaw;
	Claw *right = &rightClaw;
	
	//Comandos de câmera e encerramento
	switch (key) {
	case 27: //Escape key
		exit(0);
	case 't': //Use texture or not
		textureOn = !textureOn;
		break;
	}
	
	if (dancing) {
		glutPostRedisplay();
		return;
	}
	
	//Comandos de Movimentação do Robô
	switch (key) {
	case 13: //Enter starts choreography
		dancing = 1;
		danceStart = tick;
		left->angleClamp = 0;
		right->angleClamp = 0;
		break;
	case '1': //Increase left arm angle
		left->angleArm = fmod(left->angleArm + 3, 360);
		break;
	case '2': //Decrease left arm angle
		left->angleArm = fmod(left->angleArm + 360 - 3, 360);
		break;
	case '3': //Increase left forearm angle
		left->angleForearm = fmin(left->angleForearm + 3, +90);
		break;
	case '4': //Decrease left forearm angle
		left->angleForearm = fmax(left->angleForearm - 3, -90);
		break;
	case '5':
		moveClamp(left);
		break;
	case '6':
		moveClamp(right);
		break;
	}
	glutPostRedisplay();
}

void handleButtonPress(int button, int state, int x, int y) {
	// Zoom-in
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && fov >= 25)
		fov -= 5;
	
	// Zoom-out
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && fov <= 75)
		fov += 5;
	
	glutPostRedisplay();
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, (float)w / (float)h, 2.0, 100.0);
}

void drawCylinder(float diameter, float lenght) {
	if (textureOn) {
		glBindTexture(GL_TEXTURE_2D, _textureIdCylinder);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gluQuadricTexture(quadCylinder, 1);
	}
	else
		gluQuadricTexture(quadCylinder, 0);
	gluCylinder(quadCylinder, diameter / 2., diameter / 2., lenght, 40.0, lenght*30.0);
}

void drawCone(float diameter, float lenght) {
	if (textureOn) {
		glBindTexture(GL_TEXTURE_2D, _textureIdCylinder);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gluQuadricTexture(quadCylinder, 1);
	}
	else
		gluQuadricTexture(quadCylinder, 0);
	gluCylinder(quadCylinder, diameter / 2., 0, lenght, 40.0, lenght*30.0);
}

void drawDisk(float diameterInner, float diameterOuter) {
	if (textureOn) {
		glBindTexture(GL_TEXTURE_2D, _textureIdCylinder);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gluQuadricTexture(quadCylinder, 1);
	}
	else
		gluQuadricTexture(quadCylinder, 0);
	gluDisk(quadCylinder, diameterInner / 2., diameterOuter / 2., 40.0, 30.0);
}

void drawSphere(float diameter) {
	if (textureOn) {
		glBindTexture(GL_TEXTURE_2D, _textureIdSphere);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gluQuadricTexture(quadSphere, 1);
	}
	else
		gluQuadricTexture(quadSphere, 0);
	gluSphere(quadSphere, diameter / 2., 40.0, 40.0);
}

void drawBase(float heightBase, float diameterBase) {
	glPushMatrix();
		drawCylinder(diameterBase, heightBase);
		glTranslatef(0., 0., heightBase);
		drawDisk(0., diameterBase);
	glPopMatrix();
}

void drawBody(float sphereDiameter, float cylinderHeight, float cylinderDiameter) {
	glPushMatrix();
		glTranslatef(0., 0., sphereDiameter / 2);
		drawSphere(sphereDiameter);
		glTranslatef(0., 0., sphereDiameter / 2);
		drawDisk(0., cylinderDiameter);
		drawCylinder(cylinderDiameter, cylinderHeight);
		glTranslatef(0., 0., cylinderHeight);
		drawDisk(0., cylinderDiameter);
	glPopMatrix();
}

void drawHead(float sphereDiameter, float cylinderHeight) {
	float cylinderDiameter = 0.6;
	
	glPushMatrix();
		drawCylinder(cylinderDiameter, cylinderHeight);
		glTranslatef(0., 0., cylinderHeight);
		drawSphere(sphereDiameter);
	glPopMatrix();
}

void drawClaw(Claw c) {
	float diameterCylinder = 0.6;
	float diameterSphere = 0.8;
	float sizeArm = 4.5;
	float sizeForearm = 3.0;
	float sizeHand = 2.0;
	float sizeClampPart = 1.0;

	float angleClampZ = 0.0;
	
	glPushMatrix();
		// move to arm referential
		glRotatef(c.angleArm, 0.0f, 0.0f, 1.0f);
		
		//draws the arm
		drawCylinder(diameterCylinder, sizeArm);

		// move to forearm referential
		glTranslatef(0.0f, 0.0f, sizeArm + diameterSphere / 5);
		glRotatef(c.angleForearm, 0.0f, 1.0f, 0.0f);

		//draws the forearm
		drawSphere(diameterSphere);
		glTranslatef(0.0f, 0.0f, diameterSphere / 5);
		drawCylinder(diameterCylinder, sizeForearm);

		//move to clamp referential
		glTranslatef(0.0f, 0.0f, sizeForearm + diameterSphere / 5);
		glRotatef(angleClampZ, 0.0f, 0.0f, 1.0f);

		//draws the clamp sphere
		drawSphere(diameterSphere);
		glTranslatef(0.0f, 0.0f, diameterSphere / 2);

		glPushMatrix();
			//draws top part of clamp
			glRotatef(c.angleClamp + 60, 0.0f, 1.0f, 0.0f);

			drawCylinder(diameterCylinder / 3, sizeClampPart);
			glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
			drawSphere(diameterSphere / 3);

			glTranslatef(0.0f, 0.0f, diameterSphere / 15);
			glRotatef(-60, 0.0f, 1.0f, 0.0f);

			drawCylinder(diameterCylinder / 3, sizeClampPart);
			glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
			drawSphere(diameterSphere / 3);

			glTranslatef(0.0f, 0.0f, diameterSphere / 15);
			glRotatef(-60, 0.0f, 1.0f, 0.0f);
			drawCone(diameterCylinder / 3, sizeClampPart);
		glPopMatrix();
		
		glPushMatrix();
			//draws bottom part of clamp
			glRotatef(-c.angleClamp - 60, 0.0f, 1.0f, 0.0f);

			drawCylinder(diameterCylinder / 3, sizeClampPart);
			glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
			drawSphere(diameterSphere / 3);

			glTranslatef(0.0f, 0.0f, diameterSphere / 15);
			glRotatef(60, 0.0f, 1.0f, 0.0f);

			drawCylinder(diameterCylinder / 3, sizeClampPart);
			glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
			drawSphere(diameterSphere / 3);

			glTranslatef(0.0f, 0.0f, diameterSphere / 15);
			glRotatef(60, 0.0f, 1.0f, 0.0f);
			drawCone(diameterCylinder / 3, sizeClampPart);
		glPopMatrix();
	glPopMatrix();
}

void drawScene(void) {
	float heightBase = 0.5;
	float diameterBase = 15.0;
	float diameterBody = 10.0;
	float diameterWheel = diameterBody * 0.5;
	float heightBody = 7.5;
	float diameterHead = 3.5;
	float heightNeck = 2.5;
	
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	float eyeX = fov * cos(viewAngleZ * PI / 180) * cos(viewAngleX * PI / 180);
	float eyeY = fov * cos(viewAngleZ * PI / 180) * sin(viewAngleX * PI / 180);
	float eyeZ = fov * sin(viewAngleZ * PI / 180);
	
	if (viewAngleZ < 90)
		gluLookAt(eyeX, eyeY, eyeZ, 0., 0., 0., 0., 0., 1.);
	else
		gluLookAt(eyeX, eyeY, eyeZ, 0., 0., 0., 0., 0., -1.);

	// drawing color
	glColor3f(1., 1., 1.);

	drawBase(heightBase, diameterBase);
	
	glTranslatef(0., 0., heightBase);
	
	drawBody(diameterWheel, heightBody, diameterBody);
	
	glTranslatef(0., 0., heightBody + diameterWheel);
	
	glPushMatrix();
		glTranslatef(0., +0.5 * diameterBody, -0.35 * heightBody);
		glRotatef(-90., 1., 0., 0.);
		drawClaw(leftClaw);
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(0., -0.5 * diameterBody, -0.35 * heightBody);
		glRotatef(90., 1., 0., 0.);
		drawClaw(rightClaw);
	glPopMatrix();
	
	//glTranslatef(0., 0., heightBody * 0.25);
	
	drawHead(diameterHead, heightNeck);
	
	glutSwapBuffers();
}

void dance(void) {
	Claw *left = &leftClaw;
	Claw *right = &rightClaw;
	
	if (dancing == 0) return;
	
	moveClamp(left);
	moveClamp(right);
	
	int elapsed = tick - danceStart;
	
	if (elapsed > 120) {
		dancing = 0;
		left->angleClamp = 0;
		right->angleClamp = 0;
	}
	
	glutPostRedisplay();
}

int dt = 1000 / 60; // 60 FPS

void tickTimer(int _) {
	glutTimerFunc(dt, tickTimer, 0);
	tick++;
}

int main(int argc, char** argv) {	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Robot");

	initRendering();
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutMouseFunc(handleButtonPress);
	glutSpecialFunc(handleSpecialKeyPress);
	glutReshapeFunc(handleResize);
	glutTimerFunc(dt, tickTimer, 0);
	glutIdleFunc(dance);

	enableLigthing();
	glutMainLoop();
	return 0;
}
