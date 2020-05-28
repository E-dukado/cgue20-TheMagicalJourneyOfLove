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
	void addLamp(VBO& vbo);
	void addQuad(VBO& vbo);
	void addWood(VBO& vbo);
	void addText(VBO& vbo);
	void addParticles(VBO& vbo);


private:
	GLuint vaoId;
};
