#include "RawLoader.h"


RawLoader::RawLoader(void) {
}


RawLoader::~RawLoader(void) {
}

// load a texture and return its OpengGL mapped integer
GLuint RawLoader::LoadTextureRAW(char * filename, int wrap, int width, int height) {
    GLuint texture;
    GLubyte * data;
    FILE * file;

	//char * folder = "Textures\\";

    // open texture data
    file = fopen(filename, "rb");

    if (file == NULL) {
		throw "File not found";
	}

    // allocate buffer
    width = width;
    height = height;

	int memorySize = width * height * 3;

	data = (GLubyte *)malloc(memorySize);

    // read texture data
    fread(data, memorySize, 1, file);
    fclose(file);

    // allocate a texture name
    glGenTextures(1, &texture);

    // select our current texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // select modulate to mix texture with color for shading
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);

    // build our texture mipmaps
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

    // free buffer
    free(data);

	std::cout << filename << " loaded \n";

    return texture;
}