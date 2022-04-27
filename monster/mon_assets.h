#ifndef MON_ASSETS_H
#define MON_ASSETS_H

#include "mon_texture.h"
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

	struct Mesh
	{
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

	// global struct for accessing assets
	struct Assets
	{
		Mesh meshes[10];				// TODO(ck): SQLite config for size
		MonGL::Texture textures[30];	// TODO(ck): SQLite config for size

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

	static unsigned int AddTexture(Assets* assets)
	{
		unsigned int index = assets->textureCount++;

		MonGL::Texture* texture = &assets->textures[index];
		texture = {};

		return index;
	}

	static MonGL::Texture* GetTexture(Assets* assets, unsigned int index)
	{
		MonGL::Texture* t = 0;
		if ((index > 0) && (index < ArrayCount(assets->textures)))
		{
			t = &assets->textures[index];
		}
		return t;
	}


	static void LoadTexture(MonGL::Texture* texture, MonGL::TextureType type, std::string path, bool pixelTexture = false)
	{
		LoadTextureFile(texture, path.c_str(), type, false, true, true, pixelTexture);
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

		// empty #0 for texture
		AddTexture(assets);

		//
		// TODO(ck): Need a texture atlas rather than loading all of these
		// textures for the entities
		//

		AddTexture(assets);
		MonGL::Texture* t1 = GetTexture(assets, 1);
		AddTexture(assets);
		MonGL::Texture* t2 = GetTexture(assets, 2);
		AddTexture(assets);
		MonGL::Texture* t3 = GetTexture(assets, 3);
		AddTexture(assets);
		MonGL::Texture* t4 = GetTexture(assets, 4);
		LoadTexture(t1, MonGL::TextureType::Diffuse, "res/textures/ch_witch.png");
		LoadTexture(t2, MonGL::TextureType::Diffuse, "res/textures/p1.png");
		LoadTexture(t3, MonGL::TextureType::Diffuse, "res/textures/p1SIDE.png");
		LoadTexture(t4, MonGL::TextureType::Diffuse, "res/textures/p1BACK.png");

		AddTexture(assets);
		MonGL::Texture* t5 = GetTexture(assets, 5);
		LoadTexture(t5, MonGL::TextureType::Diffuse, "res/textures/water/ripples-derivative-height.png");
		AddTexture(assets);
		MonGL::Texture* t6 = GetTexture(assets, 6);
		LoadTexture(t6, MonGL::TextureType::Normal, "res/textures/water/flow-speed-noise.png");

		// more textures 

		// minion

		// tree

		// flower

		// terrain grid textures
	}

	extern Assets* g_Assets; // NULL
};

#endif // MON_ASSETS_H