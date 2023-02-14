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

	fov = 70;
}

void handleKeypress(unsigned char key, int x, int y) {
	Claw *left = &leftClaw;
	
	switch (key) {
	case 27: //Escape key
		exit(0);
	case 's': //Increase view angle z axis
		if (viewAngleZ < 180) viewAngleZ += 3;
		break;
	case 'w': //Decrease view angle z axis
		if (viewAngleZ > 0) viewAngleZ -= 3;
		break;
	case 'd': //Decrease view angle x axis
		if (viewAngleX > 0) viewAngleX -= 3;
		break;
	case 'a': //Increase view angle x axis
		if (viewAngleX < 180) viewAngleX += 3;
		break;
	case 't': //Use texture or not
		textureOn = !textureOn;
		break;
	case '1': //Increase arm angle
		left->angleArm += 3;
		if (left->angleArm >= 360) left->angleArm = 0;
		break;
	case '2': //Decrease arm angle
		left->angleArm -= 3;
		if (left->angleArm <= 0) left->angleArm = 360;
		break;
	case '3': //Increase forearm angle
		if (left->angleForearm < 90) left->angleForearm += 3;
		break;
	case '4': //Decrease forearm angle
		if (left->angleForearm > -90) left->angleForearm -= 3;
		break;
	case '5': //Increase clamp angle y axis
		if (left->angleClamp < 60) left->angleClamp += 3;
		break;
	case '6': //Decrease clamp angle y axis
		if (left->angleClamp > 0) left->angleClamp -= 3;
		break;
	}
	glutPostRedisplay();
}

void handleButtonPress(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) // Zoom-in
		if (state == GLUT_DOWN) { if (fov >= 10) fov -= 5; }
	if (button == GLUT_RIGHT_BUTTON) // Zoom-out
		if (state == GLUT_DOWN) { if (fov <= 130) fov += 5; }
	glutPostRedisplay(); // Redesenha
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

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Robot");

	initRendering();
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutMouseFunc(handleButtonPress);
	glutReshapeFunc(handleResize);

	enableLigthing();
	glutMainLoop();
	return 0;
}
