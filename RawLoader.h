#pragma once
#include <gl/freeglut.h>
#include <iostream>

class RawLoader {
public:
	RawLoader(void);
	~RawLoader(void);

	GLuint LoadTextureRAW(char * filename, int wrap, int width, int height);
};

