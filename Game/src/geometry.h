#pragma once

#include <vector>
#include <memory>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <GL\glew.h>
//#include "Material.h"
//#include "Shader.h"

struct GeometryData {
	/*!
	 * Vertex positions
	 */
	std::vector<glm::vec3> positions;
	/*!
	 * Geometry indices
	 */
	std::vector<unsigned int> indices;
	/*!
	 * Vertex normals
	 */
//	std::vector<glm::vec3> normals;

	/**************************Change********************/
//	std::vector<glm::vec2> UV;
};

class Geometry
{
protected:
	/*!
	 * Vertex array object
	 */
	GLuint _vao;
	/*!
	 * Vertex buffer object that stores the vertex positions
	 */
	GLuint _vboPositions;
	/*!
	 * Vertex buffer object that stores the vertex normals
	 */
	//GLuint _vboNormals;
	/*!
	 * Vertex buffer object that stores the indices
	 */
	GLuint _vboIndices;

	//GLuint _vboUVs;      /******************Change******************************/

	/*!
	 * Number of elements to be rendered
	 */
	unsigned int _elements;

	/*!
	 * Material of the geometry object
	 */
	//std::shared_ptr<Material> _material;

	/*!
	 * Model matrix of the object
	 */
	glm::mat4 _modelMatrix;


	GLuint texture;    /****************Change***********************/

public:
	/*!
	 * Geometry object constructor
	 * Creates VAO and VBOs and binds them
	 * @param modelMatrix: model matrix of the object
	 * @param data: data for the geometry object
	 * @param material: material of the geometry object
	 */
	//Geometry(glm::mat4 modelMatrix, GeometryData& data, std::shared_ptr<Material> material, DDSImage& image); /*Hier image*/
	//~Geometry();

	/*!
	 * Draws the object
	 * Uses the shader, sets the uniform and issues a draw call
	 */
	void draw();

	/*!
	 * Transforms the object, i.e. updates the model matrix
	 * @param transformation: the transformation matrix to be applied to the object
	 */
	void transform(glm::mat4 transformation);

	/*!
	 * Resets the model matrix to the identity matrix
	 */
	void resetModelMatrix();

	/*!
	 * Creates a cube geometry
	 * @param width: width of the cube
	 * @param height: height of the cube
	 * @param depth: depth of the cube
	 * @return all cube data
	 */
	static GeometryData createCubeGeometry(float width, float height, float depth);
	/*!
	 * Creates a cylinder geometry
	 * @param segments: number of segments of the cylinder
	 * @param height: height of the cylinder
	 * @param radius: radius of the cylinder
	 * @return all cylinder data
	 */
	static GeometryData createCylinderGeometry(unsigned int segments, float height, float radius);
	/*!
	 * Creates a sphere geometry
	 * @param longitudeSegments: number of longitude segments of the sphere
	 * @param latitudeSegments: number of latitude segments of the sphere
	 * @param radius: radius of the sphere
	 * @return all sphere data
	 */
	static GeometryData createSphereGeometry(unsigned int longitudeSegments, unsigned int latitudeSegments, float radius);
};