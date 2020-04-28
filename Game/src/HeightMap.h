#pragma once
#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "stb_image.h"

class HeightMap
{
public:
	HeightMap(const char* heightMapPath);
	~HeightMap();


	void bind();
	void unbind();
private:
	GLuint heightMapID;
	int width, height, nrChannels;
};
