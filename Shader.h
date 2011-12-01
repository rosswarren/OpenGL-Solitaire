#pragma once
#include <gl\glew.h>
#include <gl\freeglut.h>
#include <iostream>
#pragma comment(lib, "glew32.lib")

class Shader
{
public:
	Shader(void);
	~Shader(void);

	void Init(void);
	void use(void);
	void disable(void);
	void updateTimeAndResolution(int width, int height, int fps);
private:
	GLuint vertex, fragment, f2, program;
	GLint loc;
	int printOglError(char *file, int line);
	char * textFileRead(char *fn);
	void printProgramInfoLog(GLuint obj);
	void printShaderInfoLog(GLuint obj);
	GLfloat time;
};

