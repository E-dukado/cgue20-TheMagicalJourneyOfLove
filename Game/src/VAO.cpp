#include "VAO.h"

VAO::VAO()
{
	glGenVertexArrays(1, &vaoId);
}

VAO::~VAO()
{
	glDeleteVertexArrays(1, &vaoId);
}

void VAO::bind()
{
	glBindVertexArray(vaoId);
}

void VAO::unbind()
{
	glBindVertexArray(0);
}

void VAO::addBuffer(VBO& vbo)
{
	bind();
	vbo.bind();
	// position attributes (first three each row)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attributes (last three each row)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}
