
#include "Geometry.h"

Geometry::Geometry(mat4 modelMatrix, GeometryData& data)
	: _elements(data.indices.size()), _modelMatrix(modelMatrix)
{
	// create VAO
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// create positions VBO
	glGenBuffers(1, &_vboPositions);
	glBindBuffer(GL_ARRAY_BUFFER, _vboPositions);
	glBufferData(GL_ARRAY_BUFFER, data.positions.size() * sizeof(glm::vec3), data.positions.data(), GL_STATIC_DRAW);

	// bind positions to location 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	// create and bind indices VBO
	glGenBuffers(1, &_vboIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), data.indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Geometry::~Geometry()
{
	glDeleteBuffers(1, &_vboPositions);
	glDeleteBuffers(1, &_vboIndices);
	glDeleteVertexArrays(1, &_vao);
}

void Geometry::draw()
{
	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, _elements, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Geometry::transform(mat4 transformation)
{
	_modelMatrix = transformation * _modelMatrix;
}

void Geometry::resetModelMatrix()
{
	_modelMatrix = mat4(1.0f);
}

GeometryData Geometry::createPlaneGeometry(float width, float depth)
{
	GeometryData data;

	data.positions = {
		// front
		vec3(-width / 2.0f, 0.0f,  -depth / 2.0f),
		vec3(width / 2.0f, 0.0f,  -depth / 2.0f),
		vec3(width / 2.0f, 0.0f,  depth / 2.0f),
		vec3(-width / 2.0f, 0.0f,  depth / 2.0f)
	};


	data.indices = {		//Triangles rendered in both ways so the plane is visible from both sides when backface culling is enabled
		0, 1, 2,
		2, 3, 0,
		0, 2, 1,
		2, 0, 3
	};

	return move(data);
}

GeometryData Geometry::createCubeGeometry(float width, float height, float depth)
{
	GeometryData data;

	data.positions = {
		// front
		vec3(-width / 2.0f, -height / 2.0f,  depth / 2.0f),
		vec3(width / 2.0f, -height / 2.0f,  depth / 2.0f),
		vec3(width / 2.0f, height / 2.0f,  depth / 2.0f),
		vec3(-width / 2.0f, height / 2.0f,  depth / 2.0f),
		// back
		vec3(width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		vec3(-width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		vec3(-width / 2.0f, height / 2.0f,  -depth / 2.0f),
		vec3(width / 2.0f, height / 2.0f,  -depth / 2.0f),

		//left
		vec3(-width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		vec3(-width / 2.0f, -height / 2.0f,  depth / 2.0f),
		vec3(-width / 2.0f, height / 2.0f,  depth / 2.0f),
		vec3(-width / 2.0f, height / 2.0f,  -depth / 2.0f),

		//right
		vec3(width / 2.0f, -height / 2.0f,  depth / 2.0f),
		vec3(width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		vec3(width / 2.0f, height / 2.0f,  -depth / 2.0f),
		vec3(width / 2.0f, height / 2.0f,  depth / 2.0f),

		//top
		vec3(-width / 2.0f, height / 2.0f,  -depth / 2.0f),
		vec3(-width / 2.0f, height / 2.0f,  depth / 2.0f),
		vec3(width / 2.0f, height / 2.0f,  depth / 2.0f),
		vec3(width / 2.0f, height / 2.0f,  -depth / 2.0f),

		//bottom
		vec3(-width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		vec3(width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		vec3(width / 2.0f, -height / 2.0f,  depth / 2.0f),
		vec3(-width / 2.0f, -height / 2.0f,  depth / 2.0f)
	};


	data.indices = {
		// front
		0, 1, 2,
		2, 3, 0,

		// back
		4, 5, 6,
		6, 7, 4,

		// left
		12, 13, 14,
		14, 15, 12,

		// right
		8, 9, 10,
		10, 11, 8,

		// top
		16, 17, 18,
		18, 19, 16,

		// bottom
		20, 21, 22,
		22, 23, 20
	};

	
	return move(data);
}

GeometryData Geometry::createCylinderGeometry(unsigned int segments, float height, float radius)
{
	GeometryData data;

	// center vertices
	data.positions.push_back(glm::vec3(0, -height / 2.0f, 0));
	data.positions.push_back(glm::vec3(0, height / 2.0f, 0));

	// circle segments
	float angle_step = 2.0f * glm::pi<float>() / float(segments);
	for (unsigned int i = 0; i < segments; i++) {
		glm::vec3 circlePos = glm::vec3(
			glm::cos(i * angle_step) * radius,
			-height / 2.0f,
			glm::sin(i * angle_step) * radius
		);

		// bottom ring vertex
		data.positions.push_back(circlePos);
		data.positions.push_back(circlePos);
		// top ring vertex
		circlePos.y = height / 2.0f;
		data.positions.push_back(circlePos);
		data.positions.push_back(circlePos);
		// bottom face
		data.indices.push_back(0);
		data.indices.push_back(2 + i * 4);
		data.indices.push_back(i == segments - 1 ? 2 : 2 + (i + 1) * 4);

		// top face
		data.indices.push_back(1);
		data.indices.push_back(i == segments - 1 ? 4 : (i + 2) * 4);
		data.indices.push_back((i + 1) * 4);

		// side faces
		data.indices.push_back(3 + i * 4);
		data.indices.push_back(i == segments - 1 ? 5 : 5 + (i + 1) * 4);
		data.indices.push_back(i == segments - 1 ? 3 : 3 + (i + 1) * 4);

		data.indices.push_back(i == segments - 1 ? 5 : 5 + (i + 1) * 4);
		data.indices.push_back(3 + i * 4);
		data.indices.push_back(5 + i * 4);
	}


	return move(data);
}

GeometryData Geometry::createSphereGeometry(unsigned int longitudeSegments, unsigned int latitudeSegments, float radius)
{
	GeometryData data;

	data.positions.push_back(glm::vec3(0.0f, radius, 0.0f));
	data.positions.push_back(glm::vec3(0.0f, -radius, 0.0f));

	// first and last ring
	for (unsigned int j = 0; j < longitudeSegments; j++) {
		data.indices.push_back(0);
		data.indices.push_back(j == longitudeSegments - 1 ? 2 : (j + 3));
		data.indices.push_back(2 + j);

		data.indices.push_back(2 + (latitudeSegments - 2) * longitudeSegments + j);
		data.indices.push_back(j == longitudeSegments - 1 ? 2 + (latitudeSegments - 2) * longitudeSegments : 2 + (latitudeSegments - 2) * longitudeSegments + j + 1);
		data.indices.push_back(1);
	}

	// vertices and rings
	for (unsigned int i = 1; i < latitudeSegments; i++) {
		float verticalAngle = float(i) * glm::pi<float>() / float(latitudeSegments);
		for (unsigned int j = 0; j < longitudeSegments; j++) {
			float horizontalAngle = float(j) * 2.0f * glm::pi<float>() / float(longitudeSegments);
			glm::vec3 position = glm::vec3(
				radius * glm::sin(verticalAngle) * glm::cos(horizontalAngle),
				radius * glm::cos(verticalAngle),
				radius * glm::sin(verticalAngle) * glm::sin(horizontalAngle)
			);
			data.positions.push_back(position);

			if (i == 1) continue;

			data.indices.push_back(2 + (i - 1) * longitudeSegments + j);
			data.indices.push_back(j == longitudeSegments - 1 ? 2 + (i - 2) * longitudeSegments : 2 + (i - 2) * longitudeSegments + j + 1);
			data.indices.push_back(j == longitudeSegments - 1 ? 2 + (i - 1) * longitudeSegments : 2 + (i - 1) * longitudeSegments + j + 1);

			data.indices.push_back(j == longitudeSegments - 1 ? 2 + (i - 2) * longitudeSegments : 2 + (i - 2) * longitudeSegments + j + 1);
			data.indices.push_back(2 + (i - 1) * longitudeSegments + j);
			data.indices.push_back(2 + (i - 2) * longitudeSegments + j);
		}
	}

	return move(data);
}


