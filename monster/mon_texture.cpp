#include "mon_texture.h"
#include <stb_image/stb_image.h>

#include <glad/glad.h>

void Generate2DTexture(Texture2D* texture, unsigned int width, unsigned int height, unsigned char* data)
{
	glGenTextures(1, &texture->id);

	texture->width = width;
	texture->height = height;
	// Create texture
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glTexImage2D(GL_TEXTURE_2D, 0, texture->internalFormat, width, height, 0, texture->imageFormat, GL_UNSIGNED_BYTE, data);
	//glGenerateMipmap(GL_TEXTURE_2D);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	// Set Texture wrap and filter mnodes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->filterMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->filterMax);
	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

// Generates the texture as well
void LoadTextureFile(Texture2D* texture, const char* file, bool alpha)
{
	// TODO(CK): Clean up
	// set wrap and filter here for now same with internal formats
	texture->wrapS = GL_CLAMP_TO_BORDER; // GL_REPEAT was before
	texture->wrapT = GL_CLAMP_TO_BORDER;
	texture->filterMin = GL_NEAREST; // GL_LINEAR was before
	texture->filterMax = GL_NEAREST; // GL_LINEAR

	texture->internalFormat = GL_RGB;
	texture->imageFormat = GL_RGB;
	if (alpha)
	{
		texture->internalFormat = GL_RGBA;
		texture->imageFormat = GL_RGBA;
	}

	int width;
	int height;
	int nrChannels;

	// TODO(CK): Set this?
	stbi_set_flip_vertically_on_load(false); // use stbi to flip a texture on y-axis
	unsigned char* image = stbi_load(file, &width, &height, &nrChannels, 0);
	if (image)
	{
		Generate2DTexture(texture, width, height, image);
	}
	else
	{
		//std::cout << "Failed to load texture\n";
	}

	stbi_image_free(image);
}