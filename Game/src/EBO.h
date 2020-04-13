#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"


class EBO
{
public:
	EBO(const GLuint* data, GLuint size);
	~EBO();

	void bind() const;
	void unbind() const;

private:
	GLuint eboId;
};
