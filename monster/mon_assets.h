#ifndef MON_ASSETS_H
#define MON_ASSETS_H

#include "mon_camera.h"

namespace MonGL
{
	struct Vertex3D;
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

	void InitQuadMesh(Mesh* mesh, bool tangents = false);
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
		Mesh meshes[10];				// TODO(ck): SQLite config for size
		Image images[30];				// TODO(ck): SQLite config for size
		
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


	static void InitAssets(Assets* assets)
	{
		/*
		// might need dynamic mesh for animations
		// if a vao needs to change it has to be updated
		// and these are going to stay the same

		// quad mesh - only need one because everything is using it
		we can batch later
		*/
		
		//empty mesh #0
		AddMesh(assets);

		// quad mesh #1
		AddMesh(assets);
		Mesh* quadMesh = GetMesh(assets, 1);
		InitQuadMesh(quadMesh);

		//cube mesh #2
		AddMesh(assets);
		Mesh* cubeMesh = GetMesh(assets, 2);
		InitCubeMesh(cubeMesh);

		//grass mesh #3 
		AddMesh(assets);
		Mesh* grassMesh = GetMesh(assets, 3);
		InitModelMesh(grassMesh, "test_grass.imp");

		// This is for the terrain
		//grid mesh #4 ??
		AddMesh(assets);
		Mesh* gridMesh = GetMesh(assets, 4);
		InitGridMesh(gridMesh, 64, 64);

		// debug meshes
		// collider mesh
		AddMesh(assets);
		Mesh* collider = GetMesh(assets, 5);
		InitBoundingBoxMesh(collider);

		// line mesh
		AddMesh(assets);
		Mesh* line = GetMesh(assets, 6);
		InitLineMesh(line);

		// plane_64 mesh 64x64 units
		AddMesh(assets);
		Mesh* plane64 = GetMesh(assets, 7);
		InitModelMesh(plane64, "plane_64.imp");

		// empty #0 for image
		AddImage(assets);

		AddImage(assets);
		Image* witchImg = GetImage(assets, 1);
		InitImage(witchImg, "res/textures/ch_witch.png");

		AddImage(assets);
		Image* p1 = GetImage(assets, 2);
		InitImage(p1, "res/textures/p1.png");

		AddImage(assets);
		Image* p1Side = GetImage(assets, 3);
		InitImage(p1Side, "res/textures/p1SIDE.png");

		AddImage(assets);
		Image* p1Back = GetImage(assets, 4);
		InitImage(p1Back, "res/textures/p1BACK.png");

		AddImage(assets);
		Image* waterDeriv = GetImage(assets, 5); // diffuse 
		InitImage(waterDeriv, "res/textures/water/water.png");

		AddImage(assets);
		Image* waterFlow = GetImage(assets, 6); // normal1
		InitImage(waterFlow, "res/textures/water/flow-speed-noise.png");

		AddImage(assets);
		Image* tree = GetImage(assets, 7);
		InitImage(tree, "res/textures/tree.png");

		AddImage(assets);
		Image* container = GetImage(assets, 8);
		InitImage(container, "res/textures/container2.png");

		AddImage(assets);
		Image* minion = GetImage(assets, 9);
		InitImage(minion, "res/textures/ch_minion.png");

		AddImage(assets);
		Image* witch2 = GetImage(assets, 10);
		InitImage(witch2, "res/textures/ch_witch2.png");

		AddImage(assets);
		Image* terr1 = GetImage(assets, 11);
		InitImage(terr1, "res/textures/terrain/1024multi.png", false);

		AddImage(assets);
		Image* terr2 = GetImage(assets, 12);
		InitImage(terr2, "res/textures/terrain/grass.jpg", false);

		AddImage(assets);
		Image* terr3 = GetImage(assets, 13);
		InitImage(terr3, "res/textures/terrain/pix_grass.png", false);

		AddImage(assets);
		Image* terr4 = GetImage(assets, 14);
		InitImage(terr4, "res/textures/terrain/snow.jpg", false);

		AddImage(assets);
		Image* waterNorm2 = GetImage(assets, 15);
		InitImage(waterNorm2, "res/textures/water/water-derivative-height.png");


		// 2D TILE SHEET
		AddImage(assets);
		Image* tileSheet = GetImage(assets, 16);
		InitImage(tileSheet, "res/textures/basic_16.png");

	}


	extern Assets* g_Assets; // NULL
};

#endif // MON_ASSETS_H