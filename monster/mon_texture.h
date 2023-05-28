#ifndef MON_TEXTURE_H
#define MON_TEXTURE_H

#include "mon_log.h"
#include <string>

namespace MonGL
{
	enum class TextureType
	{
		Diffuse,
		Normal,
		Specular,
		Height,
		CubeMap
	};

	struct Texture
	{
		// TODO(ck): Remove std::string
		std::string name;
		// id of OpenGL texture object
		unsigned int id;
		// dimensions in pixels
		int width;
		int height;
		// format of the texture object and format of loaded image
		unsigned int internalFormat;
		unsigned int imageFormat;
		// texture configurations
		// t & s axis
		unsigned int wrapS;
		unsigned int wrapT;
		unsigned int filterMin;
		unsigned int filterMax;

		TextureType type;
	};

	// TODO(ck): Pass a texture config or flags instead of bools?
	void Generate2DTexture(Texture* texture, int width, int height, int nrChannels, unsigned char* data);
	void LoadFrameBufferTexture(Texture* texture, int screenWidth, int screenHeight);
	void Load2DTextureArrayFile(Texture* texture, const char* file);

}
#endif
