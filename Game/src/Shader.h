/*
* Copyright 2019 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once

#include <GL\glew.h>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Utils.h"


/*!
 * Shader class that encapsulates all shader access
 */
class Shader
{
protected:
	/*!
	 * The shader program handle
	 */
	GLuint _handle;

	/*!
	 * Path to vertex shader and fragment shader
	 */
	std::string _vs, _fs;

	/*!
	 * Whether an internal shader is being used
	 */
	bool _useFileAsSource;

	/*!
	 * Stores the shader location names with their location IDs
	 */
	std::unordered_map<std::string, GLint> _locations;

	/*!
	 * Loads the specified vertex and fragment shaders
	 * (usually called in the constructor)
	 */
	GLuint loadShaders();

	/*!
	 * Loads a shader from a given file and compiles it
	 * @param file: path to the shader
	 * @param shaderType: type of the shader (e.g. GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
	 * @param handle: shader handle
	 * @return if the shader could be loaded
	 */
	bool loadShader(std::string file, GLenum shaderType, GLuint& handle);
	
	/*!
	 * @param uniform: uniform string in shader
	 * @return the location ID of the uniform
	 */
	GLint getUniformLocation(std::string uniform);

public:

	/*!
	 * Default constructor of a simple color shader
	 */
	Shader();

	/*!
	 * Shader constructor with specified vertex and fragment shader
	 * Loads and compiles the shader
	 * @param vs: path to the vertex shader
	 * @param fs: path to the fragment shader
	 */
	Shader(std::string vs, std::string fs);
	
	~Shader();

	/*!
	 * Uses the shader with glUseProgram
	 */
	void use() const;

	/*!
	 * Un-uses the shader
	 */
	void unuse() const;

	/*!
	 * Sets an integer uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param i: the value to be set
	 */
	void setUniform(std::string uniform, const int i);
	/*!
	 * Sets an integer uniform in the shader
	 * @param location: location ID of the uniform
	 * @param i: the value to be set
	 */
	void setUniform(GLint location, const int i);
	/*!
	 * Sets an unsigned integer uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param i: the value to be set
	 */
	void setUniform(std::string uniform, const unsigned int i);
	/*!
	 * Sets an unsigned integer uniform in the shader
	 * @param location: location ID of the uniform
	 * @param i: the value to be set
	 */
	void setUniform(GLint location, const unsigned int i);
	/*!
	 * Sets a float uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param f: the value to be set
	 */
	void setUniform(std::string uniform, const float f);
	/*!
	 * Sets a float uniform in the shader
	 * @param location: location ID of the uniform
	 * @param f: the value to be set
	 */
	void setUniform(GLint location, const float f);
	/*!
	 * Sets a 4x4 matrix uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param mat: the value to be set
	 */
	void setUniform(std::string uniform, const glm::mat4& mat);
	/*!
	 * Sets a 4x4 matrix uniform in the shader
	 * @param location: location ID of the uniform
	 * @param mat: the value to be set
	 */
	void setUniform(GLint location, const glm::mat4& mat);
	/*!
	 * Sets a 3x3 matrix uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param mat: the value to be set
	 */
	void setUniform(std::string uniform, const glm::mat3& mat);
	/*!
	 * Sets a 3x3 matrix uniform in the shader
	 * @param location: location ID of the uniform
	 * @param mat: the value to be set
	 */
	void setUniform(GLint location, const glm::mat3& mat);
	/*!
	 * Sets a 2D vector uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(std::string uniform, const glm::vec2& vec);
	/*!
	 * Sets a 2D vector uniform in the shader
	 * @param location: location ID of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(GLint location, const glm::vec2& vec);
	/*!
	 * Sets a 3D vector uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(std::string uniform, const glm::vec3& vec);
	/*!
	 * Sets a 3D vector uniform in the shader
	 * @param location: location ID of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(GLint location, const glm::vec3& vec);
	/*!
	 * Sets a 4D vector uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(std::string uniform, const glm::vec4& vec);
	/*!
	 * Sets a 4D vector uniform in the shader
	 * @param location: location ID of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(GLint location, const glm::vec4& vec);
	/*!
	 * Sets a uniform array property
	 * @param arr: name of the uniform array
	 * @param i: index of the value to be set
	 * @param prop: property name
	 * @param vec: the value to be set
	 */
	void setUniformArr(std::string arr, unsigned int i, std::string prop, const glm::vec3& vec);
	/*!
	 * Sets a uniform array property
	 * @param arr: name of the uniform array
	 * @param i: index of the value to be set
	 * @param prop: property name
	 * @param f: the value to be set
	 */
	void setUniformArr(std::string arr, unsigned int i, std::string prop, const float f);
};