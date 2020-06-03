#pragma once


#include <vector>
#include <memory>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <GL\glew.h>
#include "Shader.h"

using namespace glm;
using namespace std;

struct GeometryData {
	vector<vec3> positions;
	vector<unsigned int> indices;
};


class Geometry
{
protected:
	GLuint _vao;
	GLuint _vboPositions;
	GLuint _vboIndices;
	unsigned int _elements;		//Number of elements to be rendered
	mat4 _modelMatrix;

public:
	//Geometry object constructor
	//Creates VAO and VBOs and binds them
	Geometry(mat4 modelMatrix, GeometryData& data);
	~Geometry();

	//Draws the object
	//Uses the shader, sets the uniform and issues a draw call
	void draw();

	//Transforms the object, i.e. updates the model matrix
	void transform(glm::mat4 transformation);

	void resetModelMatrix();

	static GeometryData createPlaneGeometry(float width, float depth);

	static GeometryData createCubeGeometry(float width, float height, float depth);

	static GeometryData createCylinderGeometry(unsigned int segments, float height, float radius);

	static GeometryData createSphereGeometry(unsigned int longitudeSegments, unsigned int latitudeSegments, float radius);
};