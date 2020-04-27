#include "Terrain.h"


const int NUM_X = 100; //total quads on X axis
const int NUM_Z = 100; //total quads on Z axis

const float SIZE_X = 50; //size of plane in world space
const float SIZE_Z = 50;
const float HALF_SIZE_X = SIZE_X / 2.0f;
const float HALF_SIZE_Z = SIZE_Z / 2.0f;

//ripple displacement speed
const float SPEED = 10;

//ripple mesh vertices and indices
glm::vec3 vertices[(NUM_X + 1) * (NUM_Z + 1)];
const int TOTAL_INDICES = NUM_X * NUM_Z * 2 * 3;
GLushort indices[TOTAL_INDICES];

//IDs
GLuint vaoID;
GLuint vboVerticesID;
GLuint vboIndicesID;




//setup plane geometry
//setup plane vertices
void Terrain::generateTerrain() {
int count = 0;
int i = 0, j = 0;
for (j = 0; j <= NUM_Z; j++) {
	for (i = 0; i <= NUM_X; i++) {
		vertices[count++] = glm::vec3(((float(i) / (NUM_X - 1)) * 2 - 1) * HALF_SIZE_X, 0, ((float(j) / (NUM_Z - 1)) * 2 - 1) * HALF_SIZE_Z);
	}
}

//fill plane indices array
GLushort* id = &indices[0];
for (i = 0; i < NUM_Z; i++) {
	for (j = 0; j < NUM_X; j++) {
		int i0 = i * (NUM_X + 1) + j;
		int i1 = i0 + 1;
		int i2 = i0 + (NUM_X + 1);
		int i3 = i2 + 1;
		if ((j + i) % 2) {
			*id++ = i0; *id++ = i2; *id++ = i1;
			*id++ = i1; *id++ = i2; *id++ = i3;
		}
		else {
			*id++ = i0; *id++ = i2; *id++ = i3;
			*id++ = i0; *id++ = i3; *id++ = i1;
		}
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
	glDrawElements(GL_TRIANGLES, TOTAL_INDICES, GL_UNSIGNED_SHORT, 0);


}