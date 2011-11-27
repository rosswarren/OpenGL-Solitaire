#pragma once
#include <gl/freeglut.h>
#include <math.h>

class Shapes
{
public:
	Shapes(void);
	~Shapes(void);

	void cube(void);
	void cuboid(GLfloat width, GLfloat height, GLfloat depth);
	void cylinder(double height, double radiusBase, double radiusTop, int slices, int stacks, GLUquadricObj* quadric);
};

