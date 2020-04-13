#pragma once
#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "stb_image.h"

class Texture
{
public:
	Texture(const char* texturePath);
	~Texture();

	void bind();
	void unbind();
private:
	GLuint textureId;
	int width, height, nrChannels;
};
