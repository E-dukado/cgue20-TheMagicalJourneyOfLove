#include "VBO.h"

VBO::VBO(const void* data, GLuint size)
{
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VBO::~VBO()
{
	glDeleteBuffers(1, &vboId);
}

void VBO::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
}

void VBO::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
