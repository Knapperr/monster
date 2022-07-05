#ifndef MON_ASSETS_H
#define MON_ASSETS_H

#include "mon_camera.h"

// IMPORTANT(ck): TODO(ck): Forward decl
namespace MonGL
{
	struct Vertex3D;
	struct Vertex;
};

namespace Mon
{
	enum class RenderType
	{
		None,
		Model,
		Quad,
		Cube,
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

	struct Mesh
	{
		const char* id;

		unsigned int VAO;
		unsigned int VBO;
		unsigned int IBO;
		int verticeCount;
		int indiceCount;
		RenderType type;

		MonGL::Vertex3D* vertices;
		unsigned int* indices;
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

	void InitQuadMesh(Mesh* mesh, bool tangents = false);
	void Init2DQuadMesh(Mesh2D* mesh);
	void InitCubeMesh(Mesh* mesh);
	void InitModelMesh(Mesh* mesh, const char* fileName);
	void InitGridMesh(Mesh* mesh, int xSize, int zSize);
	void InitBoundingBoxMesh(Mesh* mesh);
	void InitLineMesh(Mesh* mesh);

	void InitImage(Image* image, const char* file, bool flip = true);
	void FreeStbiImage(Image* image);


	// global struct for accessing assets
	struct Assets
	{
		Mesh meshes[16];
		Image images[32];

		Mesh2D quad2D; // Remove
		int meshCount;
		int textureCount;
	};

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
		if ((index > 0) && (index < ArrayCount(assets->meshes)))
		{
			mesh = &assets->meshes[index];
		}
		return mesh;
	}

	static unsigned int AddImage(Assets* assets)
	{
		unsigned int index = assets->textureCount++;

		Image* img = &assets->images[index];
		img = {};

		return index;
	}

	static Image* GetImage(Assets* assets, unsigned int index)
	{
		Image* img = 0;
		if ((index > 0) && (index < ArrayCount(assets->images)))
		{
			img = &assets->images[index];
		}
		return img;
	}


	void InitAssets(Assets* assets);

	extern Assets* g_Assets; // NULL
};

#endif // MON_ASSETS_H