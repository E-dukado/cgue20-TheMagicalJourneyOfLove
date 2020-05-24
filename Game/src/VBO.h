#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"


class VBO
{
public:
	VBO(const void* data, GLuint size);
	VBO(GLuint size);
	~VBO();

	void bind() const;
	void unbind() const;

private:
	GLuint vboId;
};