#include "Shader.h"

Shader::Shader(void) {
	time = 0;
}

Shader::~Shader(void) {
}

void Shader::use() {
	glUseProgram(program);
}

void Shader::disable() {
	glUseProgram(0);
}

void Shader::updateTimeAndResolution(int width, int height, int fps) {
	// if fps is 20, time should be 0.01
	// if fps is 200, time should be 0.001

	GLfloat add;

	if (fps < 1) {
		add = 0;
	} else {
		add = 0.5f / (GLfloat)fps;
	}

	time += add;

	GLint location = glGetUniformLocation(program, "time");
    glUniform1f(location, time);

	location = glGetUniformLocation(program, "resolution");
    glUniform2f(location, (GLfloat)width, (GLfloat)height);

    DWORD bob = glGetError();
}

void Shader::Init() {
	glewInit();
	char *vs = NULL,*fs = NULL;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("background.vert");
	fs = textFileRead("background.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(vertex, 1, &vv, NULL);
	glShaderSource(fragment, 1, &ff, NULL);

	free(vs);
	free(fs);

	glCompileShader(vertex);
	glCompileShader(fragment);

	printShaderInfoLog(vertex);
	printShaderInfoLog(fragment);

	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);

	glLinkProgram(program);
	printProgramInfoLog(program);

	loc = glGetUniformLocation(program, "time");
}

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

void Shader::printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0) {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}

void Shader::printShaderInfoLog(GLuint obj) {
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0) {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}
