#include "mon_texture.h"

#include <glad/glad.h>

#include <string>

using namespace Mon;

namespace MonGL
{
	void Generate2DTexture(Texture* texture, int width, int height, int nrChannels, unsigned char* data)
	{
		// TODO(ck): STUDY(ck): 1 in this 
		glGenTextures(1, &texture->id);

		texture->internalFormat = GL_RGB;
		if (nrChannels == 1)
			texture->internalFormat = GL_RED;
		else if (nrChannels == 2)
			texture->internalFormat = GL_RG;
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

		// Set Texture wrap and filter modes
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->filterMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->filterMax);

		// unbind after using
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// TODO(ck): Clean up
	// I THINK I CAN DELETE THIS?
	//unsigned int LoadTextureFile(const char* path, const std::string& directory, bool gamma)
	//{
	//	std::string filename = std::string(path);
	//	filename = directory + '/' + filename;

	//	unsigned int textureID;
	//	glGenTextures(1, &textureID);

	//	int width, height, nrComponents;
	//	stbi_set_flip_vertically_on_load(false); // use stbi to flip a texture on y-axis
	//	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

	//	if (data)
	//	{
	//		GLenum format = GL_RGB;
	//		if (nrComponents == 1)
	//			format = GL_ALPHA;
	//		else if (nrComponents == 3)
	//			format = GL_RGB;
	//		else if (nrComponents == 4)
	//			format = GL_RGBA;

	//		//GLenum format;
	//		//if (nrComponents == 1)
	//		//	format = GL_RED;
	//		//else if (nrComponents == 3)
	//		//	format = GL_RGB;
	//		//else if (nrComponents == 4)
	//		//	format = GL_RGBA;


	//		glBindTexture(GL_TEXTURE_2D, textureID);
	//		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	//		//glGenerateMipmap(GL_TEXTURE_2D);

	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//		// unbind after using
	//		glBindTexture(GL_TEXTURE_2D, 0);
	//		stbi_image_free(data);
	//	}
	//	else
	//	{
	//		//std::string msg = "Texture failed to load at path: " + (std::string)path;
	//		//LOG_WARN(msg.c_str());
	//		stbi_image_free(data);
	//	}
	//	return textureID;
	//}

	void LoadFrameBufferTexture(Texture* texture, int screenWidth, int screenHeight)
	{
		glGenTextures(1, &texture->id);
		glBindTexture(GL_TEXTURE_2D, texture->id);

		// size of screen
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id, 0);

		texture->width = screenWidth;
		texture->height = screenHeight;
	}

	void Load2DTextureArrayFile(Texture* texture, const char* file)
	{
#if 0
		// Get pixel data
		int nrChannels;
		unsigned char* image = stbi_load(file, &texture->width, &texture->height, &nrChannels, 0); // STBI_rgb_alpha <-- for last param?
		if (image)
		{
			int tile_w = 16;
			int tile_h = 16;
			int tiles_x = texture->width / tile_w;
			int tiles_y = texture->height / tile_h;
			int tileCount = tiles_x * tiles_y;

			// Generating Texture

			// TODO(ck): STUDY(ck): 1 in this 
			// Don't need these?
			//glGenTextures(1, &texture->id);
			//glBindTexture(GL_TEXTURE_2D_ARRAY, texture->id);

			texture->internalFormat = GL_RGB;
			if (nrChannels == 1)
				texture->internalFormat = GL_RED;
			else if (nrChannels == 2)
				texture->internalFormat = GL_RG;
			else if (nrChannels == 3)
				texture->internalFormat = GL_RGB;
			else if (nrChannels == 4)
				texture->internalFormat = GL_RGBA;
			
			glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture->id);
			glTextureStorage3D(texture->id, 1, GL_RGBA8, tile_w, tile_h, tileCount);
			
				unsigned int temp_tex = 0;
				glCreateTextures(GL_TEXTURE_2D, 1, &temp_tex);
				glTextureStorage2D(temp_tex, 1, GL_RGBA8, texture->width, texture->height);
				glTextureSubImage2D(temp_tex, 0, 0, 0, texture->width, texture->height, GL_RGBA, GL_UNSIGNED_BYTE, image);

				for (int i = 0; i < tileCount; ++i)
				{
					int x = (i % tiles_x) * tile_w;
					int y = (i / tiles_x) * tile_h;
					glCopyImageSubData(temp_tex, GL_TEXTURE_2D, 0, x, y, 0, texture->id, GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, tile_w, tile_h, 1);
				}
				glDeleteTextures(1, &temp_tex);
			
			glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

			//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 4);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// unbind after using
			glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		}
		else
		{
			// TODO(ck): no strings
			std::string fileString = file;
			std::string msg = "failed to load texture: " + fileString;
			Mon::Log::warn(msg.c_str());
		}

		stbi_image_free(image);
#endif
	}

}
