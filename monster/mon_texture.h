#ifndef MON_TEXTURE_H
#define MON_TEXTURE_H

#include <string>

namespace MonTexture
{
	struct Texture
	{
		// id of texture object
		unsigned int id;
		// dimensions in pixels
		unsigned int width;
		unsigned int height;
		// format of the texture object and format of loaded image
		unsigned int internalFormat;
		unsigned int imageFormat;
		// texture configurations
		// t & s axis
		unsigned int wrapS;
		unsigned int wrapT;
		unsigned int filterMin;
		unsigned int filterMax;
	};

	void GenerateTexture(Texture* texture, unsigned int width, unsigned int height, unsigned char* data);
	void LoadTextureFile(Texture* texture, const char* file, bool alpha);
	unsigned int LoadTextureFile(const char* path, const std::string& directory, bool gamma = false);

}
#endif
