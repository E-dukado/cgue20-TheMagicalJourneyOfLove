#pragma once
#include "VBO.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"


class VAO
{
public:
	VAO();
	~VAO();

	void bind();
	void unbind();
	void addBuffer(VBO& vbo);
private:
	GLuint vaoId;
};
