#include "Heightmap.h"

HeightMap::HeightMap(const char* heightMapPath)
{
	glGenTextures(1, &heightMapID);
	bind();

	//Wrapping and Filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//load and generate texture
	stbi_set_flip_vertically_on_load(true); //because y=0 at images are on top of axis
	unsigned char* data = stbi_load(heightMapPath, &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Loading texture failed!" << std::endl;
	}
	stbi_image_free(data);
}

HeightMap::~HeightMap()
{
	glDeleteTextures(GL_TEXTURE_2D, &heightMapID);
}

void HeightMap::bind()
{
	glBindTexture(GL_TEXTURE_2D, heightMapID);
}

void HeightMap::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

