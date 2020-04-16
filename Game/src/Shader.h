#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>




class Shader
{
public:
	GLuint ID;

	Shader(const char* vertexPath, const char* fragmentPath);

	//activate the shader
	void use();

	//uniform functions
	void setFloat(const std::string& name, float value) const;

	void setVec2(const std::string& name, GLsizei count, glm::vec2& value) const;

	void setVec3(const std::string& name, GLsizei count, glm::vec3& value) const;

	void setMat4(const std::string& name, GLsizei count, GLboolean transpose, glm::mat4& value) const;
};



#endif // !SHADER_H