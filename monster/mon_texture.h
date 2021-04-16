#ifndef MON_TEXTURE_H
#define MON_TEXTURE_H

struct Texture2D
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

void Generate2DTexture(Texture2D* texture, unsigned int width, unsigned int height, unsigned char* data);
void LoadTextureFile(Texture2D* texture, const char* file, bool alpha);


#endif
