#pragma once
#ifndef UTIL_H
#define UTIL_H

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <cmath>
#include <windowsx.h>
#include <iostream>
#include <cstdio> // For printf

#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "GLU32.lib")


int windowSize = 800;
int halfWindow = 400;
const float PI = 3.14159265359f;

enum Boolean {
	False = 0,
	True = 1
};


class Vector3 {
public:
	float x, y, z; // Public member variables for coordinates

	// Default constructor
	Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

	// Parameterized constructor
	Vector3(float xVal, float yVal, float zVal) : x(xVal), y(yVal), z(zVal) {}

	void set(float xVal, float yVal, float zVal) {
		x = xVal;
		y = yVal;
		z = zVal;
	}
};

const GLenum FILL = GLU_FILL, LINE = GLU_LINE, LINELOOP = GL_LINE_LOOP, QUAD = GL_QUADS;


bool projectLight = false;
bool projectTexture = true;

GLenum drawStyle = FILL;
GLenum mode = QUAD;

GLuint selectedTexture, metal, weatheredMetal, darkMetal, blackMetal, darkRustyMetal, darkRustyMetalBright, blade, gun, texture, plastic, bulletRow, logo, bronze3;



void setupLighting() {
	glEnable(GL_LIGHT0);

	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat specularLight[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat lightPosition[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glEnable(GL_NORMALIZE);
}

bool loadTexture(const std::string& filename, GLuint& textureID) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	// Load BMP using Windows API
	HBITMAP hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		filename.c_str(),
		IMAGE_BITMAP,
		0, 0,
		LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	if (!hBMP) {
		std::cerr << "Failed to load texture: " << filename << std::endl;
		return false;
	}

	// Retrieve BMP information
	BITMAP BMP;
	GetObject(hBMP, sizeof(BMP), &BMP);

	// Generate and bind texture
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Send texture data to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	// Cleanup
	DeleteObject(hBMP);

	return true;
}

// ==========================
//           UTIL
// ==========================

void LoadAllTextures() {

	loadTexture("metal.bmp", metal);
	loadTexture("weatheredMetal2.bmp", weatheredMetal);
	loadTexture("bronze1.bmp", bronze3);
	loadTexture("darkMetal.bmp", darkMetal);
	loadTexture("blackMetal.bmp", blackMetal);
	loadTexture("darkRustyMetal.bmp", darkRustyMetal);
	loadTexture("darkRustyMetalBright.bmp", darkRustyMetalBright);
	loadTexture("blade.bmp", blade);
	loadTexture("gun.bmp", gun);
	loadTexture("fluffy.bmp", plastic);
	loadTexture("bulletRow.bmp", bulletRow);
	loadTexture("logo7.bmp", logo);

}

// ==========================
//           UTIL
// ==========================
// Function to calculate the cross product of two vectors
void Texture(GLuint texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
}

Vector3 crossProduct(const Vector3 a, const Vector3 b) {
	return Vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

// Function to calculate the normal of a triangle
Vector3 calculateTriangleNormal(Vector3 v1, Vector3 v2, Vector3 v3) {
	Vector3 v1v2 = Vector3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
	Vector3 v1v3 = Vector3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
	return crossProduct(v1v2, v1v3);
}

void UtilColor(int r, int g, int b) {
	float floatR = (float)r / 255, floatG = (float)g / 255, floatB = (float)b / 255;
	float color[] = { floatR, floatG, floatB };
	glMaterialfv(GL_FRONT, GL_AMBIENT, color);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
	glColor3f(floatR, floatG, floatB);
}

void UtilColor(Vector3 color) {
	UtilColor(color.x, color.y, color.z);
}

void UtilVertex(int x, int y, int z) {
	float floatX = (float)x / halfWindow;
	float floatY = (float)y / halfWindow;
	float floatZ = (float)z / halfWindow;

	glVertex3d(floatX, floatY, floatZ);
}

void UtilTranslate(int x, int y, int z) {
	float floatX = (float)x / halfWindow;
	float floatY = (float)y / halfWindow;
	float floatZ = (float)z / halfWindow;

	glTranslatef(floatX, floatY, floatZ);
}

void UtilPosition(int x, int y, int z) {
	float floatX = ((float)x - halfWindow) / halfWindow;
	float floatY = ((float)y - halfWindow) / halfWindow;
	float floatZ = ((float)z - halfWindow) / halfWindow;

	glTranslatef(floatX, floatY, floatZ);
}

void UtilPosition(int x, int y, int z, Vector3 relative) {
	float floatX = (float)x - relative.x;
	float floatY = (float)y - relative.y;
	float floatZ = (float)z - relative.z;

	UtilTranslate(floatX, floatY, floatZ);
}

void UtilCircle(Vector3 startPoint, int startAngle, int endAngle, int length, Vector3 color) {
	float startX = (startPoint.x - halfWindow) / halfWindow;
	float startY = (startPoint.y - halfWindow) / halfWindow;
	float lengthF = (float)length / halfWindow;

	glBegin(GL_TRIANGLE_FAN);
	UtilColor(color.x, color.y, color.z);
	glTexCoord2f(0.5f, 0.5f); // Texture center (normalized)
	glVertex2d(0 + startX, 0 + startY);

	for (float currentDegree = (float)startAngle; currentDegree <= (float)endAngle; currentDegree++) {
		float currentRadian = currentDegree * (PI / 180.0f);
		float x = (sin(currentRadian) * lengthF) + startX;
		float y = (cos(currentRadian) * lengthF) + startY;
		// Map the vertex position to texture coordinates
		float texX = (sin(currentRadian) * 0.5f) + 0.5f; // Map to range [0, 1]
		float texY = (cos(currentRadian) * 0.5f) + 0.5f; // Map to range [0, 1]

		glTexCoord2f(texX, texY); // Assign texture coordinate
		glVertex2f(x, y);  // Current point on the circle
	}

	glEnd();
	return;
}

void UtilCircle(float radius, int stacks, Vector3 color) {

	glBegin(GL_TRIANGLE_FAN);
	UtilColor(color.x, color.y, color.z);
	// Center of the circle corresponds to the center of the texture
	glTexCoord2f(0.5f, 0.5f); // Texture center (normalized)
	UtilVertex(0, 0, 0);

	float angleDifference = (float)360 / stacks;

	for (float currentDegree = 0; currentDegree >= -360;) {
		float currentRadian = currentDegree * (PI / 180.0f);
		float x = sin(currentRadian) * radius;
		float z = cos(currentRadian) * radius;

		// Map the vertex position to texture coordinates
		float texX = (x / (2.0f * radius)) + 0.5f; // Normalize to range [0, 1]
		float texZ = (z / (2.0f * radius)) + 0.5f; // Normalize to range [0, 1]

		glTexCoord2f(texX, texZ); // Assign texture coordinate
		UtilVertex(x, 0, z);     // Current point on the circle

		currentDegree -= angleDifference;
	}

	glEnd();
	return;
}

void UtilCylinder(GLUquadric* qobj, Vector3 color, float radiusTop, float radiusBot, float height, int slices, int stacks, Boolean closed) {
	glPushMatrix();
	glRotatef(90, 1, 0, 0); // Rotate the cylinder so it stands upright

	UtilColor(color);


	// Apply the texture scaling to the cylinder
	gluQuadricDrawStyle(qobj, drawStyle);
	float newRadiusTop = radiusTop / halfWindow;
	float newRadiusBot = radiusBot / halfWindow;
	float newHeight = height / halfWindow;

	// Draw the cylinder with texture scaling applied
	gluCylinder(qobj, newRadiusTop, newRadiusBot, newHeight, slices, stacks);
	glPopMatrix();


	if (closed) {
		glNormal3f(0, 1, 0);
		UtilCircle(radiusTop + 1, slices, color);

		glPushMatrix();
		glRotatef(180, 0, 1, 0);
		UtilTranslate(0, -height, 0);
		glNormal3f(0, -1, 0);
		UtilCircle(radiusBot + 1, slices, color);
		glPopMatrix();
	}

}

void UtilCylinder(GLUquadric* qobj, Vector3 color, float radiusTop, float radiusBot, float height, int slices, int stacks) {
	UtilCylinder(qobj, color, radiusTop, radiusBot, height, slices, stacks, True);

}

void UtilCylinder(GLUquadric* qobj, Vector3 color, float radius, float height, int slices, int stacks, Boolean closed) {
	UtilCylinder(qobj, color, radius, radius, height, slices, stacks, closed);
}


void UtilCylinder(GLUquadric* qobj, Vector3 color, float radius, float height, int slices, int stacks) {
	UtilCylinder(qobj, color, radius, radius, height, slices, stacks);
}


void UtilSphere(GLUquadric* qobj, Vector3 color, float radius, int slices, int stacks) {
	UtilColor(color.x, color.y, color.z);
	gluQuadricDrawStyle(qobj, drawStyle);
	float newRadius = radius / halfWindow;

	gluSphere(qobj, newRadius, slices, stacks);
}

void UtilCuboid(Vector3 color, int height, int botWidth, int topWidth, int botThickness, int topThickness, float textureXmul, float textureYmul, Boolean stretch) {
	//float bigSide = 0, smallSide = 0;
	float topLeftXfb = 0, topRightXfb = 1;
	float botLeftXfb = 0, botRightXfb = 1;

	float topLeftXlr = 0, topRightXlr = 1;
	float botLeftXlr = 0, botRightXlr = 1;

	if (stretch == False) {
		float checkSizefb = botWidth - topWidth;

		if (checkSizefb > 0) {
			botLeftXfb = 0;
			botRightXfb = 1;

			topLeftXfb = (float)(botWidth - topWidth) / (botWidth * 2);
			topRightXfb = (float)(botWidth + topWidth) / (botWidth * 2);
		}
		else if (checkSizefb < 0) {
			topLeftXfb = 0;
			topRightXfb = 1;

			botLeftXfb = (float)(topWidth - botWidth) / (topWidth * 2);
			botRightXfb = (float)(topWidth + botWidth) / (topWidth * 2);

		}
		float checkSizelr = botThickness - topThickness;

		if (checkSizelr > 0) {
			botLeftXlr = 0;
			botRightXlr = 1;

			topLeftXlr = (float)(botThickness - topThickness) / (botThickness * 2);
			topRightXlr = (float)(botThickness + topThickness) / (botThickness * 2);
		}
		else if (checkSizelr < 0) {
			topLeftXlr = 0;
			topRightXlr = 1;

			botLeftXlr = (float)(topThickness - botThickness) / (topThickness * 2);
			botRightXlr = (float)(topThickness + botThickness) / (topThickness * 2);
		}
	}

	UtilColor(color);
	glBegin(mode); //front
	
	Vector3 e1 = Vector3(-topWidth, height, topThickness);
	Vector3 e2 = Vector3(-botWidth, 0, botThickness);
	Vector3 e3 = Vector3(botWidth, 0, botThickness);
	Vector3 normal = calculateTriangleNormal(e1, e2, e3);
	glNormal3f(normal.x, normal.y, normal.z);

	glTexCoord2f(topLeftXfb, 1.0f * textureYmul);
	UtilVertex(-topWidth, height, topThickness);
	glTexCoord2f(botLeftXfb, 0.0f);
	UtilVertex(-botWidth, 0, botThickness);
	glTexCoord2f(botRightXfb * textureXmul, 0.0f);
	UtilVertex(botWidth, 0, botThickness);
	glTexCoord2f(topRightXfb * textureXmul, 1.0f * textureYmul);
	UtilVertex(topWidth, height, topThickness);
	glEnd();

	glBegin(mode); //back
	e1 = Vector3(topWidth, height, -topThickness);
	e2 = Vector3(botWidth, 0, -botThickness);
	e3 = Vector3(-botWidth, 0, -botThickness);
	normal = calculateTriangleNormal(e1, e2, e3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(topLeftXfb, 1.0f * textureYmul);
	UtilVertex(topWidth, height, -topThickness);
	glTexCoord2f(botLeftXfb, 0.0f);
	UtilVertex(botWidth, 0, -botThickness);
	glTexCoord2f(botRightXfb * textureXmul, 0.0f);
	UtilVertex(-botWidth, 0, -botThickness);
	glTexCoord2f(topRightXfb * textureXmul, 1.0f * textureYmul);
	UtilVertex(-topWidth, height, -topThickness);
	glEnd();

	glBegin(mode); //left
	e1 = Vector3(-topWidth, height, -topThickness);
	e2 = Vector3(-botWidth, 0, -botThickness);
	e3 = Vector3(-botWidth, 0, botThickness);
	normal = calculateTriangleNormal(e1, e2, e3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(topLeftXlr, 1.0f * textureYmul);
	UtilVertex(-topWidth, height, -topThickness);
	glTexCoord2f(botLeftXlr, 0.0f);
	UtilVertex(-botWidth, 0, -botThickness);
	glTexCoord2f(botRightXlr * textureXmul, 0.0f);
	UtilVertex(-botWidth, 0, botThickness);
	glTexCoord2f(topRightXlr * textureXmul, 1.0f * textureYmul);
	UtilVertex(-topWidth, height, topThickness);
	glEnd();

	glBegin(mode); //right
	e1 = Vector3(topWidth, height, topThickness);
	e2 = Vector3(botWidth, 0, botThickness);
	e3 = Vector3(botWidth, 0, -botThickness);
	normal = calculateTriangleNormal(e1, e2, e3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(topLeftXlr, 1.0f * textureYmul);
	UtilVertex(topWidth, height, topThickness);
	glTexCoord2f(botLeftXlr, 0.0f);
	UtilVertex(botWidth, 0, botThickness);
	glTexCoord2f(botRightXlr * textureXmul, 0.0f);
	UtilVertex(botWidth, 0, -botThickness);
	glTexCoord2f(topRightXlr * textureXmul, 1.0f * textureYmul);
	UtilVertex(topWidth, height, -topThickness);
	glEnd();

	glBegin(mode); //up
	e1 = Vector3(-topWidth, height, -topThickness);
	e2 = Vector3(-topWidth, height, topThickness);
	e3 = Vector3(topWidth, height, topThickness);
	normal = calculateTriangleNormal(e1, e2, e3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.0f, 1.0f * textureYmul);
	UtilVertex(-topWidth, height, -topThickness);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(-topWidth, height, topThickness);
	glTexCoord2f(1.0f * textureXmul, 0.0f);
	UtilVertex(topWidth, height, topThickness);
	glTexCoord2f(1.0f * textureXmul, 1.0f * textureYmul);
	UtilVertex(topWidth, height, -topThickness);
	glEnd();

	glBegin(mode); //down
	e1 = Vector3(-botWidth, 0, botThickness);
	e2 = Vector3(-botWidth, 0, -botThickness);
	e3 = Vector3(botWidth, 0, -botThickness);
	normal = calculateTriangleNormal(e1, e2, e3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.0f, 1.0f * textureYmul);
	UtilVertex(-botWidth, 0, botThickness);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(-botWidth, 0, -botThickness);
	glTexCoord2f(1.0f * textureXmul, 0.0f);
	UtilVertex(botWidth, 0, -botThickness);
	glTexCoord2f(1.0f * textureXmul, 1.0f * textureYmul);
	UtilVertex(botWidth, 0, botThickness);
	glEnd();
}

void UtilCuboid(Vector3 color, int height, int botWidth, int topWidth, int botThickness, int topThickness, float textureXmul, float textureYmul) {

	UtilColor(color);
	glBegin(mode); //front
	Vector3 e1 = Vector3(-topWidth, height, topThickness);
	Vector3 e2 = Vector3(-botWidth, 0, botThickness);
	Vector3 e3 = Vector3(botWidth, 0, botThickness);
	Vector3 normal = calculateTriangleNormal(e1, e2, e3);
	glNormal3f(normal.x, normal.y, normal.z);

	glTexCoord2f(0.0f, 1.0f * textureYmul);
	UtilVertex(-topWidth, height, topThickness);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(-botWidth, 0, botThickness);
	glTexCoord2f(1.0f * textureXmul, 0.0f);
	UtilVertex(botWidth, 0, botThickness);
	glTexCoord2f(1.0f * textureXmul, 1.0f * textureYmul);
	UtilVertex(topWidth, height, topThickness);
	glEnd();

	glBegin(mode); //back
	e1 = Vector3(topWidth, height, -topThickness);
	e2 = Vector3(botWidth, 0, -botThickness);
	e3 = Vector3(-botWidth, 0, -botThickness);
	normal = calculateTriangleNormal(e1, e2, e3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.0f, 1.0f * textureYmul);
	UtilVertex(topWidth, height, -topThickness);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(botWidth, 0, -botThickness);
	glTexCoord2f(1.0f * textureXmul, 0.0f);
	UtilVertex(-botWidth, 0, -botThickness);
	glTexCoord2f(1.0f * textureXmul, 1.0f * textureYmul);
	UtilVertex(-topWidth, height, -topThickness);
	glEnd();

	glBegin(mode); //left
	e1 = Vector3(-topWidth, height, -topThickness);
	e2 = Vector3(-botWidth, 0, -botThickness);
	e3 = Vector3(-botWidth, 0, botThickness);
	normal = calculateTriangleNormal(e1, e2, e3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.0f, 1.0f * textureYmul);
	UtilVertex(-topWidth, height, -topThickness);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(-botWidth, 0, -botThickness);
	glTexCoord2f(1.0f * textureXmul, 0.0f);
	UtilVertex(-botWidth, 0, botThickness);
	glTexCoord2f(1.0f * textureXmul, 1.0f * textureYmul);
	UtilVertex(-topWidth, height, topThickness);
	glEnd();

	glBegin(mode); //right
	e1 = Vector3(topWidth, height, topThickness);
	e2 = Vector3(botWidth, 0, botThickness);
	e3 = Vector3(botWidth, 0, -botThickness);
	normal = calculateTriangleNormal(e1, e2, e3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.0f, 1.0f * textureYmul);
	UtilVertex(topWidth, height, topThickness);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(botWidth, 0, botThickness);
	glTexCoord2f(1.0f * textureXmul, 0.0f);
	UtilVertex(botWidth, 0, -botThickness);
	glTexCoord2f(1.0f * textureXmul, 1.0f * textureYmul);
	UtilVertex(topWidth, height, -topThickness);
	glEnd();

	glBegin(mode); //up
	e1 = Vector3(-topWidth, height, -topThickness);
	e2 = Vector3(-topWidth, height, topThickness);
	e3 = Vector3(topWidth, height, topThickness);
	normal = calculateTriangleNormal(e1, e2, e3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.0f, 1.0f * textureYmul);
	UtilVertex(-topWidth, height, -topThickness);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(-topWidth, height, topThickness);
	glTexCoord2f(1.0f * textureXmul, 0.0f);
	UtilVertex(topWidth, height, topThickness);
	glTexCoord2f(1.0f * textureXmul, 1.0f * textureYmul);
	UtilVertex(topWidth, height, -topThickness);
	glEnd();

	glBegin(mode); //down
	e1 = Vector3(-botWidth, 0, botThickness);
	e2 = Vector3(-botWidth, 0, -botThickness);
	e3 = Vector3(botWidth, 0, -botThickness);
	normal = calculateTriangleNormal(e1, e2, e3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.0f, 1.0f * textureYmul);
	UtilVertex(-botWidth, 0, botThickness);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(-botWidth, 0, -botThickness);
	glTexCoord2f(1.0f * textureXmul, 0.0f);
	UtilVertex(botWidth, 0, -botThickness);
	glTexCoord2f(1.0f * textureXmul, 1.0f * textureYmul);
	UtilVertex(botWidth, 0, botThickness);
	glEnd();
}

void UtilCuboid(Vector3 color, int height, int botWidth, int topWidth, int botThickness, int topThickness) {
	UtilCuboid(color, height, botWidth, topWidth, botThickness, topThickness, 1, 1);
}

void UtilCuboid(Vector3 color, int height, int width, int thickness) {
	UtilCuboid(color, height, width, width, thickness, thickness);
}
void UtilCuboidScale(Vector3 color, int height, int width, int thickness, float xScale, float yScale) {
	UtilCuboid(color, height, width, width, thickness, thickness, xScale, yScale);
}

void UtilTriangle(Vector3 color, int height, int width, int thickness) {

	UtilColor(color);
	glBegin(GL_TRIANGLES); //front
	Vector3 v1 = Vector3(0, height, thickness);
	Vector3 v2 = Vector3(-width, 0, thickness);
	Vector3 v3 = Vector3(width, 0, thickness);
	Vector3 normal = calculateTriangleNormal(v1, v2, v3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.5f, 1.0f);
	UtilVertex(0, height, thickness);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(-width, 0, thickness);
	glTexCoord2f(1.0f, 0.0f);
	UtilVertex(width, 0, thickness);
	glEnd();

	glBegin(GL_TRIANGLES); //back
	v2 = Vector3(width, 0, -thickness);
	v3 = Vector3(-width, 0, -thickness);
	normal = calculateTriangleNormal(v1, v2, v3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.5f, 1.0f);
	UtilVertex(0, height, -thickness);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(width, 0, -thickness);
	glTexCoord2f(1.0f, 0.0f);
	UtilVertex(-width, 0, -thickness);
	glEnd();

	glBegin(GL_QUADS); //left
	Vector3 e1 = Vector3(0, height, -thickness);
	Vector3 e2 = Vector3(-width, 0, -thickness);
	Vector3 e3 = Vector3(-width, 0, thickness);
	normal = calculateTriangleNormal(e1, e2, e3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.0f, 1.0f);
	UtilVertex(0, height, -thickness);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(-width, 0, -thickness);
	glTexCoord2f(1.0f, 0.0f);
	UtilVertex(-width, 0, thickness);
	glTexCoord2f(1.0f, 1.0f);
	UtilVertex(0, height, thickness);
	glEnd();

	glBegin(GL_QUADS); //right
	e1 = Vector3(0, height, thickness);
	e2 = Vector3(width, 0, thickness);
	e3 = Vector3(width, 0, -thickness);
	normal = calculateTriangleNormal(e1, e2, e3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.0f, 1.0f);
	UtilVertex(0, height, thickness);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(width, 0, thickness);
	glTexCoord2f(1.0f, 0.0f);
	UtilVertex(width, 0, -thickness);
	glTexCoord2f(1.0f, 1.0f);
	UtilVertex(0, height, -thickness);
	glEnd();


	glBegin(GL_QUADS); //down
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.0f, 1.0f);
	UtilVertex(-width, 0, thickness);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(-width, 0, -thickness);
	glTexCoord2f(1.0f, 0.0f);
	UtilVertex(width, 0, -thickness);
	glTexCoord2f(1.0f, 1.0f);
	UtilVertex(width, 0, thickness);
	glEnd();
}

void UtilPyramid(Vector3 color, int height, int width, int thickness) {

	UtilColor(color);
	glBegin(GL_QUADS);//bottom
	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	UtilVertex(-width, 0, thickness);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(-width, 0, -thickness);
	glTexCoord2f(1.0f, 0.0f);
	UtilVertex(width, 0, -thickness);
	glTexCoord2f(1.0f, 1.0f);
	UtilVertex(width, 0, thickness);
	glEnd();

	// Front face
	glBegin(GL_TRIANGLES);
	Vector3 v1 = Vector3(0, height, 0);
	Vector3 v2 = Vector3(-width, 0, thickness);
	Vector3 v3 = Vector3(width, 0, thickness);
	Vector3 normal = calculateTriangleNormal(v1, v2, v3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.5f, 1.0f);
	UtilVertex(0, height, 0);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(-width, 0, thickness);
	glTexCoord2f(1.0f, 0.0f);
	UtilVertex(width, 0, thickness);
	glEnd();

	// Back face
	glBegin(GL_TRIANGLES);
	v2 = Vector3(width, 0, -thickness);
	v3 = Vector3(-width, 0, -thickness);
	normal = calculateTriangleNormal(v1, v2, v3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.5f, 1.0f);
	UtilVertex(0, height, 0);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(width, 0, -thickness);
	glTexCoord2f(1.0f, 0.0f);
	UtilVertex(-width, 0, -thickness);
	glEnd();

	// Left face
	glBegin(GL_TRIANGLES);
	v2 = Vector3(-width, 0, -thickness);
	v3 = Vector3(-width, 0, thickness);
	normal = calculateTriangleNormal(v1, v2, v3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.5f, 1.0f);
	UtilVertex(0, height, 0);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(-width, 0, -thickness);
	glTexCoord2f(1.0f, 0.0f);
	UtilVertex(-width, 0, thickness);
	glEnd();

	// Right face
	glBegin(GL_TRIANGLES);
	v2 = Vector3(width, 0, thickness);
	v3 = Vector3(width, 0, -thickness);
	normal = calculateTriangleNormal(v1, v2, v3);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.5f, 1.0f);
	UtilVertex(0, height, 0);
	glTexCoord2f(0.0f, 0.0f);
	UtilVertex(width, 0, thickness);
	glTexCoord2f(1.0f, 0.0f);
	UtilVertex(width, 0, -thickness);
	glEnd();
}




#endif // UTIL_H