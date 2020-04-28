#include "Terrain.h"




const int TERRAIN_DEPTH = 100;
const int TERRAIN_WIDTH = 100;


//ripple displacement speed
const float SPEED = 10;

/*ripple mesh vertices and indices
glm::vec3 vertices[(NUM_X + 1) * (NUM_Z + 1)];
const int TOTAL_INDICES = NUM_X * NUM_Z * 2 * 3;
GLushort indices[TOTAL_INDICES];
*/

//total vertices and indices in the terrain
const int TOTAL = (TERRAIN_WIDTH * TERRAIN_DEPTH);
const int TOTAL_INDICES = TOTAL * 2 * 3;
glm::vec3 vertices[TOTAL];
GLuint indices[TOTAL_INDICES];

//IDs
GLuint vaoID;
GLuint vboVerticesID;
GLuint vboIndicesID;




//setup plane geometry
//setup plane vertices
void Terrain::generateTerrain() {
	//fill indices array
	GLuint* id = &indices[0];
	int i = 0, j = 0;

	//setup vertices 
	int count = 0;
	//fill terrain vertices
	for (j = 0; j < TERRAIN_DEPTH; j++) {
		for (i = 0; i < TERRAIN_WIDTH; i++) {
			vertices[count] = glm::vec3((float(i) / (TERRAIN_WIDTH - 1)),
				0,
				(float(j) / (TERRAIN_DEPTH - 1)));
			count++;
		}
	}

	//fill terrain indices
	for (i = 0; i < TERRAIN_DEPTH - 1; i++) {
		for (j = 0; j < TERRAIN_WIDTH - 1; j++) {
			int i0 = j + i * TERRAIN_WIDTH;
			int i1 = i0 + 1;
			int i2 = i0 + TERRAIN_WIDTH;
			int i3 = i2 + 1;
			*id++ = i0;
			*id++ = i2;
			*id++ = i1;
			*id++ = i1;
			*id++ = i2;
			*id++ = i3;
		}
	}

glGenVertexArrays(1, &vaoID);
glGenBuffers(1, &vboVerticesID);
glGenBuffers(1, &vboIndicesID);
glBindVertexArray(vaoID);
glBindBuffer(GL_ARRAY_BUFFER, vboVerticesID);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndicesID);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
}

void Terrain::drawTerrain()
{

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES, TOTAL_INDICES, GL_UNSIGNED_INT, 0);


}