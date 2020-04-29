#include "Texture.h"

Texture::Texture(const char* texturePath)
{
	glGenTextures(1, &textureId);
	bind();

	//Wrapping and Filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//load and generate texture
	stbi_set_flip_vertically_on_load(true); //because y=0 at images are on top of axis
	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	if (data) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Loading texture failed!" << std::endl;
	}
	stbi_image_free(data);
}

Texture::Texture(const char* texturePath1, const char* texturePath2)
{
	glGenTextures(2, texIDs);

	// Load brick texture file
	unsigned char* data1 = stbi_load(texturePath1, &width, &height, &nrChannels, 0);

	// Copy brick texture to OpenGL
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texIDs[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
	//glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data1);


	// Load moss texture file
	unsigned char* data2 = stbi_load(texturePath2, &width, &height, &nrChannels, 0);

	// Copy moss texture to OpenGL
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texIDs[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
	//glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data2);
}


Texture::~Texture()
{
	glDeleteTextures(GL_TEXTURE_2D, &textureId);
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void Texture::doubleBind()
{
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, texIDs[0]);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, texIDs[1]);

}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

