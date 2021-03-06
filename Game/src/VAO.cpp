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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normals (middle three each row)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture attributes (last two each row)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void VAO::addLamp(VBO& vbo)
{
	bind();
	vbo.bind();

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

}

void VAO::addQuad(VBO& vbo)
{
	bind();
	vbo.bind();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

}

void VAO::addWood(VBO& vbo)
{
	bind();
	vbo.bind();

	// position attributes (first three each row)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normals (middle three each row)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void VAO::addText(VBO& vbo)
{
	bind();
	vbo.bind();

	// position attributes (first three each row)
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}




