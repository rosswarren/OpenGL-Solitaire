#include "Shader.h"

/**
* Some of the shader loading stuff was with help from a NeHe tutorial
* http://nehe.gamedev.net/
*/

Shader::Shader(void) {
	time = 0;
	increase = false;
}

Shader::~Shader(void) {
}

// enable the shader
void Shader::use() {
	glUseProgram(program);
}

// disable the shader
void Shader::disable() {
	glUseProgram(0);
}

// set the shader to use normal speed
void Shader::normalSpeed() {
	increase = false;
}

// set the speed to 30 times the speed (yeah yeah it's not really doubling, but it looks doubled...)
void Shader::doubleSpeed() {
	increase = true;
}

// update shader uniforms
void Shader::updateTimeAndResolution(int width, int height, int fps) {
	// if fps is 20, time should be 0.01
	// if fps is 200, time should be 0.001
	// well not anymore... but something along those lines
	GLfloat add;

	// to stop divide by 0 errors
	if (fps < 1) {
		add = 0;
	} else {
		add = 0.5f / (GLfloat)fps; // accounts for the game running different speeds on different computers
	}

	// check if it is supposed to run at increased speed or not and do what is appropriate
	if (increase) {
		time += add * 30.0f;
	} else {
		time += add;
	}

	// set the uniforms
	GLint location = glGetUniformLocation(program, "time");
    glUniform1f(location, time);

	location = glGetUniformLocation(program, "screenSize");
    glUniform2f(location, (GLfloat)width, (GLfloat)height);

	// get the error out if there is one
    DWORD bob = glGetError();
}

// initialise the shader
void Shader::init() {
	glewInit();
	char *vs = NULL,*fs = NULL;

	// set up the variables to hold the shaders
	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);

	// raed the shaders as text
	vs = textFileRead("background.vert");
	fs = textFileRead("background.frag");

	const char * vv = vs;
	const char * ff = fs;

	// specify where the source is coming from
	glShaderSource(vertex, 1, &vv, NULL);
	glShaderSource(fragment, 1, &ff, NULL);

	// no longer needed
	free(vs);
	free(fs);

	// attempt compiling
	glCompileShader(vertex);
	glCompileShader(fragment);

	// print out just in case of errors
	//printProgramInfoLog(vertex);
	printProgramInfoLog(fragment);

	// craete the shader program
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);

	// try linking
	glLinkProgram(program);

	// print incase of errors
	printProgramInfoLog(program);

	loc = glGetUniformLocation(program, "time");
}

// prints opengl errors
int Shader::printOglError(char *file, int line) {
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    GLenum glErr;
    int retCode = 0;
    glErr = glGetError();

    while (glErr != GL_NO_ERROR) {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }

    return retCode;
}

// read a text file
char * Shader::textFileRead(char *fn) {
	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) {
		fp = fopen(fn, "rt");

		if (fp != NULL) {
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}

			fclose(fp);
		}
	}
	return content;
}

// prints the shader or shader program information
void Shader::printProgramInfoLog(GLuint obj) {
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0) {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}