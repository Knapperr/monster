#ifndef MON_TEXTURE_H
#define MON_TEXTURE_H

#include "mon_log.h"
#include <string>

namespace MonGL
{
	enum class Type
	{
		Diffuse,
		Normal,
		Specular,
		Height
	};

	struct Texture
	{
		// id of texture object
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

		Type type;
	};

	void LoadTextureFile(Texture* texture, const char* file, Type type, bool alpha, bool flip, bool pixelArtTexture = false);


}
#endif
