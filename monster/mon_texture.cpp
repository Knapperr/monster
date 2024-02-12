#include "mon_texture.h"

#include <glad/glad.h>

#include <string>

using namespace Mon;

namespace MonGL
{
	void InitTexture(Texture* texture, int width, int height, int nrChannels, unsigned char* data)
	{
		texture->width = width;
		texture->height = height;

		unsigned int internalFormat = GL_RGB;
		switch (nrChannels)
		{
		case 1:
			internalFormat = GL_RED;
			break;
		case 2:
			internalFormat = GL_RG;
			break;
		case 3:
			internalFormat = GL_RGB;
			break;
		case 4:
			internalFormat = GL_RGBA;
			break;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &texture->id);

		// ONLY TURN OFF FOR 2D
		//glGenerateMipmap(GL_TEXTURE_2D);

		glTextureParameteri(texture->id, GL_TEXTURE_WRAP_S, texture->wrapS);
		glTextureParameteri(texture->id, GL_TEXTURE_WRAP_T, texture->wrapT);
		glTextureParameteri(texture->id, GL_TEXTURE_MIN_FILTER, texture->filterMin);
		glTextureParameteri(texture->id, GL_TEXTURE_MAG_FILTER, texture->filterMax);

		glTextureStorage2D(texture->id, 1, GL_RGBA16, width, height);
		glTextureSubImage2D(texture->id, 0, 0, 0, width, height, internalFormat, GL_UNSIGNED_BYTE, data);

		
		std::string textureName = "texture";
		switch (texture->type)
		{
		case TextureType::Diffuse:
			textureName += "_diffuse";
			break;
		case TextureType::Normal:
			textureName += "_normal";
			break;
		case TextureType::Specular:
			textureName += "_specular";
			break;
		case TextureType::Height:
			textureName += "_heightmap";
			break;
		}

		textureName += "_" + std::to_string(texture->id);
		glObjectLabel(GL_TEXTURE, texture->id, -1, textureName.c_str());
	}
	/*
	void LoadCubeMapTexture(Texture* texture)
	{
		texture->type = TextureType::CubeMap;

		glGenTextures(1, &texture->id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture->id);

		for (unsigned int i = 0; i < 6; ++i)
		{
			// TODO(ck): NEED TO LOOKUP CUBEMAP IMAGES
			// HAVE A CUBEMAP ASSET THAT IS SET IN THE FILE??? THAT WAY THE GUI CAN SET THE 
			// INDEX TO THE TEXTURE AND SAVE IT SO ITS NOT A BIG DEAL... JUST TAG IMAGES 
			Image* img = GetImage(g_Assets, 24 + i);

			if (img->data)
			{
				texture->width = img->width;
				texture->height = img->height;
				texture->internalFormat = GL_RGB;
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
							 0, GL_RGB, img->width, img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, img->data);
			}
			else
			{
				// TODO(ck): no strings
				std::string msg = "failed to load cubemap textures";
				Mon::Log::warn(msg.c_str());
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	*/

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


	// The below method uses glCopyImageSubData
	/*
		From my understanding glCopyImageSubData is used for taking one layer and putting into the next. I think what you would do is
		take a texture sheet from the first layer and copy each sprite to a new layer? I think this is the idea/

		The simple approach is to use glTexSubImage3D. I am not exactly sure how to use a sprite sheet iwht it
		You can copy a sprite sheet using glTexSubImage3D as well
	
	*/
		

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
