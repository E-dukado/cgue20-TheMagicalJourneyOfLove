#include "Terrain.h"




const int TERRAIN_DEPTH = 10;
const int TERRAIN_WIDTH = 10;


//ripple displacement speed
const float SPEED = 2;

/*ripple mesh vertices and indices
glm::vec3 vertices[(NUM_X + 1) * (NUM_Z + 1)];
const int TOTAL_INDICES = NUM_X * NUM_Z * 2 * 3;
GLushort indices[TOTAL_INDICES];
*/

//total vertices and indices in the terrain
const int TOTAL = (TERRAIN_WIDTH * TERRAIN_DEPTH);
const int TOTAL_INDICES = TOTAL * 2 * 3;
const int TEX_COORDS = (TERRAIN_WIDTH * TERRAIN_DEPTH);
glm::vec3 vertices[TOTAL];
glm::vec3 normals[TOTAL_INDICES];
glm::vec2 texCoords[TEX_COORDS];
GLuint indices[TOTAL_INDICES];

//IDs
GLuint vaoID;
GLuint vboVerticesID;
GLuint vboIndicesID;
GLuint vboUVID;




//setup plane geometry
//setup plane vertices
void Terrain::generateTerrain() {
	//fill indices array
	GLuint* id = &indices[0];
	int i = 0, j = 0;
	//setup vertices 
	int count = 0;
	//fill terrain vertices
	for (i = 0; i < TERRAIN_DEPTH; i++) {
		for (j = 0; j < TERRAIN_WIDTH; j++) {
			vertices[count] = glm::vec3((float(j) / (TERRAIN_WIDTH - 1)), 0, (float(i) / (TERRAIN_DEPTH - 1)));
			texCoords[count] = glm::vec2((float)i / TERRAIN_DEPTH, (float)j / TERRAIN_WIDTH);

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
			std::cout << i0 << std::endl;
			std::cout << i1 << std::endl;
			std::cout << i2 << std::endl;
			std::cout << i3 << std::endl;
		}
	}

	

	


glGenVertexArrays(1, &vaoID);
glGenBuffers(1, &vboVerticesID);
glGenBuffers(1, &vboIndicesID);
glGenBuffers(1, &vboUVID);

glBindVertexArray(vaoID);

//Vertices
glBindBuffer(GL_ARRAY_BUFFER, vboVerticesID);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
glEnableVertexAttribArray(0);

//Indices
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndicesID);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

//UV
glBindBuffer(GL_ARRAY_BUFFER, vboUVID);
glBufferData(GL_ARRAY_BUFFER, TEX_COORDS * sizeof(glm::vec2), &texCoords[0], GL_STATIC_DRAW);
glEnableVertexAttribArray(1);
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
}



//depth = rows
//calculate normals
/*void Terrain::generateNormals()
{
	std::vector<std::vector<glm::vec3>> _normals = std::vector<std::vector<glm::vec3>>(TERRAIN_DEPTH, std::vector<glm::vec3>(TERRAIN_WIDTH));
	std::vector< std::vector<glm::vec3> > tempNormals[2];
	for (auto i = 0; i < 2; i++) {
		tempNormals[i] = std::vector<std::vector<glm::vec3>>(TERRAIN_DEPTH - 1, std::vector<glm::vec3>(TERRAIN_WIDTH - 1));
	}

	for (auto i = 0; i < TERRAIN_DEPTH - 1; i++)
	{
		for (auto j = 0; j < TERRAIN_WIDTH - 1; j++)
		{
			const auto& vertexA = vertices[j + i * TERRAIN_WIDTH]; //
			const auto& vertexB = vertices[j + i * TERRAIN_WIDTH + 1];
			const auto& vertexC = vertices[j + i * TERRAIN_WIDTH + TERRAIN_WIDTH];
			const auto& vertexD = vertices[j + i * TERRAIN_WIDTH + TERRAIN_WIDTH + 1];
			std::cout << "thisA " << vertexA.x << vertexA.y << vertexA.z<< std::endl;
			std::cout << "thisb " << vertexB.x << vertexB.y << vertexB.z << std::endl;
			std::cout << "thisc " << vertexC.x << vertexC.y << vertexC.z << std::endl;
			std::cout << "thisd " << vertexD.x << vertexD.y << vertexD.z << std::endl;





			const auto triangleNormalA = glm::cross(vertexA - vertexB, vertexA - vertexD);
			const auto triangleNormalB = glm::cross(vertexD - vertexC, vertexC - vertexB);

			tempNormals[0][i][j] = glm::normalize(triangleNormalA);
			tempNormals[1][i][j] = glm::normalize(triangleNormalB);

			std::cout << " tempnormals  "<< tempNormals[0][i][j].x << tempNormals[0][i][j].y << tempNormals[0][i][j].z << std::endl;
		}
	}

	for (auto i = 0; i < TERRAIN_DEPTH; i++)
	{
		for (auto j = 0; j < TERRAIN_WIDTH; j++)
		{
			const auto isFirstRow = i == 0;
			const auto isFirstColumn = j == 0;
			const auto isLastRow = i == TERRAIN_DEPTH - 1;
			const auto isLastColumn = j == TERRAIN_WIDTH - 1;

			auto finalVertexNormal = glm::vec3(0.0f, 0.0f, 0.0f);

			// Look for triangle to the upper-lef
			if (!isFirstRow && !isFirstColumn) {
				finalVertexNormal += tempNormals[0][i - 1][j - 1];
			}

			// Look for triangles to the upper-righ
			if (!isFirstRow && !isLastColumn) {
				for (auto k = 0; k < 2; k++) {
					finalVertexNormal += tempNormals[k][i - 1][j];
				}
			}

			// Look for triangle to the bottom-righ
			if (!isLastRow && !isLastColumn) {
				finalVertexNormal += tempNormals[0][i][j];
			}

			// Look for triangles to the bottom-righ
			if (!isLastRow && !isFirstColumn) {
				for (auto k = 0; k < 2; k++) {
					finalVertexNormal += tempNormals[k][i][j - 1];
				}
			}

			// Store final normal of j-th vertex in i-th ro
			_normals[i][j] = glm::normalize(finalVertexNormal);
			std::cout << "hey " << finalVertexNormal.x << " - " << finalVertexNormal.y <<" - "<< finalVertexNormal.z<< std::endl;
		

		}
		//_vbo.addData(_normals[i].data(), TERRAIN_WIDTH * sizeof(glm::vec3));      //still need to figure this out
	}
	for (size_t i = 0; i < sizeof(normals); i++)
	{
		std::cout << normals[i].x << normals[i].y << normals[i].z << std::endl;

	}
}*/

void Terrain::drawTerrain()
{

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES, TOTAL_INDICES, GL_UNSIGNED_INT, 0);


}