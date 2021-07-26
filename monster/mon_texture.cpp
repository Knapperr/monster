#include "mon_texture.h"
#include <stb_image/stb_image.h>

#include <glad/glad.h>

#include <string>

namespace MonTexture
{
	void Generate2DTexture(Texture* texture, unsigned int width, unsigned int height, int nrChannels, unsigned char* data)
	{
		glGenTextures(1, &texture->id);

		texture->internalFormat = GL_RGB;
		if (nrChannels == 1)
			texture->internalFormat = GL_ALPHA;
		else if (nrChannels == 3)
			texture->internalFormat = GL_RGB;
		else if (nrChannels == 4)
			texture->internalFormat = GL_RGBA;

		texture->width = width;
		texture->height = height;
		// Create texture
		glBindTexture(GL_TEXTURE_2D, texture->id);
		glTexImage2D(GL_TEXTURE_2D, 0, texture->internalFormat, width, height, 0, texture->internalFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set Texture wrap and filter mnodes
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->filterMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->filterMax);
		// Unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Generates the texture as well
	void LoadTextureFile(Texture* texture, const char* file, bool alpha, bool flip, bool pixelArtTexture)
	{
		// TODO(CK): Clean up
		// set wrap and filter here for now same with internal formats
		if (pixelArtTexture == false)
		{
			texture->wrapS = GL_REPEAT; // GL_REPEAT was before
			texture->wrapT = GL_REPEAT;
			texture->filterMin = GL_LINEAR_MIPMAP_LINEAR; // GL_LINEAR was before
			texture->filterMax = GL_LINEAR; // GL_LINEAR

			// TODO(ck): REmove this its getting changed in Generate and its also not using ->imageFormat anymore
			texture->internalFormat = GL_RGB;
			texture->imageFormat = GL_RGB;
			if (alpha)
			{
				texture->internalFormat = GL_RGBA;
				texture->imageFormat = GL_RGBA;
			}
		}
		else
		{
			texture->wrapS = GL_CLAMP_TO_BORDER; // GL_REPEAT was before
			texture->wrapT = GL_CLAMP_TO_BORDER;
			texture->filterMin = GL_NEAREST;
			texture->filterMax = GL_NEAREST;

			// TODO(ck): REmove this its getting changed in Generate and its also not using ->imageFormat anymore
			texture->internalFormat = GL_RGB;
			texture->imageFormat = GL_RGB;
			if (alpha)
			{
				texture->internalFormat = GL_RGBA;
				texture->imageFormat = GL_RGBA;
			}
		}

		int width;
		int height;
		int nrChannels;

		stbi_set_flip_vertically_on_load(flip);
		unsigned char* image = stbi_load(file, &width, &height, &nrChannels, 0);
		if (image)
		{
			Generate2DTexture(texture, width, height, nrChannels, image);
		}
		else
		{
			// TODO(ck): Logging
			printf("Failed to load texture\n");
			//std::cout << "Failed to load texture\n";
		}

		stbi_image_free(image);
	}

	// TODO(ck): Clean up
	unsigned int LoadTextureFile(const char* path, const std::string& directory, bool gamma)
	{
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		stbi_set_flip_vertically_on_load(false); // use stbi to flip a texture on y-axis
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

		if (data)
		{
			GLenum format = GL_RGB;
			if (nrComponents == 1)
				format = GL_ALPHA;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			//GLenum format;
			//if (nrComponents == 1)
			//	format = GL_RED;
			//else if (nrComponents == 3)
			//	format = GL_RGB;
			//else if (nrComponents == 4)
			//	format = GL_RGBA;


			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// unbind after using
			glBindTexture(GL_TEXTURE_2D, 0);
			stbi_image_free(data);
		}
		else
		{
			//std::string msg = "Texture failed to load at path: " + (std::string)path;
			//LOG_WARN(msg.c_str());
			stbi_image_free(data);
		}
		return textureID;
	}
}
