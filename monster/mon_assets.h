#ifndef MON_ASSETS_H
#define MON_ASSETS_H

#include "mon_camera.h"

#include <string>

// IMPORTANT(ck): TODO(ck): Forward decl
namespace MonGL
{
	struct Vertex3D;
	struct Vertex;
	enum class TextureType;
};

namespace Mon
{
	enum class RenderType
	{
		None,
		Model,
		Quad,
		Cube,
		CubeMap,
		Debug,
	};

	enum class ImageType
	{
		PNG,
		JPG,
		BITMAP
	};

	struct Image
	{
		unsigned char* data;
		int width;
		int height;
		int nrChannels;
		ImageType type;

		// check to see if loaded and return back texture to it?
		// set this texture index and if its not 0 that means we have loaded it
		int textureIndex;
	};

	struct TextureAsset
	{
		MonGL::TextureType type;
		int imageIndex; // TODO(ck): Image Handle
		bool linearFilter;
		bool isPixelArt;
		std::string name;
	};

	// TODO(ck): Temp
	struct AssetTile
	{
		int x;
		int y;
		int id;
	};

	struct TextureAtlas
	{
		int textureAssetIndex;
		int tileSize;
		int size;
		int rows;
		int cols;
		// NOTE(ck): Can add offsets for this?
		// support only using part of the texture atlas?
		AssetTile tiles[256];
	};

	struct AABB
	{
		v3 min;
		v3 max;
	};

	struct MeshAsset
	{
		const char* id;
		std::string name;

		// what other nice data can this have?
		// like a colour...
		// this is a nice small struct to save/load
		// to a file
	};

	struct Mesh
	{
		std::string name;

		unsigned int VAO;
		unsigned int VBO;
		unsigned int IBO;
		int verticeCount;
		int indiceCount;
		v3 max;
		v3 min;
		AABB bbox; // mesh bounding box for frustum culling
		RenderType type;

		// Should these be deleted after uploaded to the gpu?
		// That might save us a lot of space
		MonGL::Vertex3D* vertices;
		unsigned short int* indices;
	};

	// TODO(ck): Do not have specific for 2D
	struct Mesh2D
	{
		const char* id;

		unsigned int VAO;
		unsigned int VBO;
		unsigned int IBO;
		int verticeCount;
		int indiceCount;
		RenderType type;

		MonGL::Vertex* vertices;
		unsigned int* indices;
	};

	void InitModelMesh(Mesh* mesh, const char* fileName);
	void InitGridMesh(Mesh* mesh, int xSize, int zSize);
	
	void InitImage(Image* image, const char* file, bool flip = true);
	void FreeImage(Image* image);
	void InitTextureAsset(TextureAsset* asset, std::string& name, MonGL::TextureType type, bool pixelArt, int imageIndex);
	void InitTextureAtlas(TextureAtlas* atlas, int assetIndex, int atlasSize, int tileSize);

	// global struct for accessing assets
	struct Assets
	{
		Mesh meshes[32];
		Image images[64];
		TextureAsset textureAssets[32];
		TextureAtlas atlases[8];

		int meshCount;
		int imageCount;
		int textureAssetCount;
		int atlasCount;
	};

	// TODO(ck): Clean up these... Need generic container
	// AssetManager to handle all of these things internally
	// 
	static unsigned int AddMesh(Assets* assets)
	{
		unsigned int index = assets->meshCount++; 

		Mesh* mesh = &assets->meshes[index];
		mesh = {};

		return index;
	}

	static Mesh* GetMesh(Assets* assets, unsigned int index)
	{
		Mesh* mesh = 0;
		if ((index >= 0) && (index < ArrayCount(assets->meshes)))
		{
			mesh = &assets->meshes[index];
		}
		return mesh;
	}

	static unsigned int AddImage(Assets* assets)
	{
		unsigned int index = assets->imageCount++;

		Image* img = &assets->images[index];
		img = {};

		return index;
	}

	static Image* GetImage(Assets* assets, unsigned int index)
	{
		Image* img = 0;
		if ((index >= 0) && (index < ArrayCount(assets->images)))
		{
			img = &assets->images[index];
		}
		return img;
	}

	static unsigned int AddTextureAsset(Assets* assets)
	{
		unsigned int index = assets->textureAssetCount++;

		TextureAsset* t = &assets->textureAssets[index];
		t = {};

		return index;
	}

	static TextureAsset* GetTextureAsset(Assets* assets, unsigned int index)
	{
		TextureAsset* t = 0;
		if ((index >= 0) && (index < ArrayCount(assets->textureAssets)))
		{
			t = &assets->textureAssets[index];
		}
		return t;
	}

	static unsigned int AddTextureAtlas(Assets* assets)
	{
		unsigned int index = assets->atlasCount++;

		TextureAtlas* a = &assets->atlases[index];
		a = {};

		return index;
	}

	static TextureAtlas* GetTextureAtlas(Assets* assets, unsigned int index)
	{
		TextureAtlas* a = 0;
		if ((index >= 0) && (index < ArrayCount(assets->atlases)))
		{
			a = &assets->atlases[index];
		}
		return a;
	}

	static AssetTile* GetTileAsset(Assets* assets, unsigned int atlasIndex, unsigned int tileIndex)
	{
		AssetTile* tile = 0;
		if ((atlasIndex >= 0) && (atlasIndex <ArrayCount(assets->atlases)))
		{
			tile = &assets->atlases[atlasIndex].tiles[tileIndex];
		}
		return tile;
	}

	void InitAssets(Assets* assets);

	extern Assets* g_Assets; // NULL
};

#endif // MON_ASSETS_H