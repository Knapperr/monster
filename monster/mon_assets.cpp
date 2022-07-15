#include "mon_assets.h"
#include "mon_gl_render.h"

#include <stb_image/stb_image.h>

namespace Mon
{
	// IMPORTANT(ck): Asset manager defined here
	// TODO(ck): Memory management
	// does this get set to null for allocation we want this to happen after the game memory has been created
	Assets* g_Assets = new Assets();

	void InitAssets(Assets* assets)
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

		// gem mesh
		AddMesh(assets);
		Mesh* gem = GetMesh(assets, 8);
		InitModelMesh(gem, "gem_1x1.imp");

		AddMesh(assets);
		Mesh* light = GetMesh(assets, 9);
		InitModelMesh(light, "light_sphere.imp");


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

		AddImage(assets);
		Image* OnexOneSheet = GetImage(assets, 17);
		InitImage(OnexOneSheet, "res/textures/grass1x1.png", false);

		AddImage(assets);
		Image* pDevil = GetImage(assets, 18);
		InitImage(pDevil, "res/textures/ch_devil.png");

		AddImage(assets);
		Image* cube1m = GetImage(assets, 19);
		InitImage(cube1m, "res/textures/debug/TWW_Txi_c1m.png");

		AddImage(assets);
		Image* wwBrick = GetImage(assets, 20);
		InitImage(wwBrick, "res/textures/debug/TLoZTWW_H_bg01.png");

		AddImage(assets);
		Image* ww2m = GetImage(assets, 21);
		InitImage(ww2m, "res/textures/debug/TWW_Txi_c2m.png");

		AddImage(assets);
		Image* ww4m = GetImage(assets, 22);
		InitImage(ww4m, "res/textures/debug/TWW_Txi_c4m.png");

		AddImage(assets);
		Image* ww10m = GetImage(assets, 23);
		InitImage(ww10m, "res/textures/debug/TWW_Txk_c10m.png");

		//AddImage(assets);
		//Image* tileSheet = GetImage(assets, 18);
		//InitImage(tileSheet, "res/textures/basic_16.png", false);

		//AddImage(assets);
		//Image* tileSheet = GetImage(assets, 19);
		//InitImage(tileSheet, "res/textures/basic_16.png", false);

		// 2d qaud mesh
		Init2DQuadMesh(&assets->quad2D);
	}


#include <fstream>
#include <string>
	void LoadImpFile(Mesh* mesh, const char* fileName)
	{
		std::ifstream file(fileName);
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		int index;
		try
		{
			if (!file.is_open())
			{
				Mon::Log::print("Failure to open file");
			}

			std::string line;
			while (file >> line)
			{
				std::string nameLine = line;
				file >> mesh->verticeCount;
				file >> mesh->indiceCount;
				break;
			}

			mesh->vertices = new MonGL::Vertex3D[mesh->verticeCount];
			mesh->indices = new unsigned int[mesh->indiceCount];

			while (file >> line)
			{
				for (int i = 0; i < mesh->verticeCount; ++i)
				{
					index = i;

					if (i == 0)
						mesh->vertices[i].position.x = std::stof(line);
					else
						file >> mesh->vertices[i].position.x;

					file >> mesh->vertices[i].position.y;
					file >> mesh->vertices[i].position.z;
					file >> mesh->vertices[i].normal.x;
					file >> mesh->vertices[i].normal.y;
					file >> mesh->vertices[i].normal.z;
					file >> mesh->vertices[i].texCoords.x;
					file >> mesh->vertices[i].texCoords.y;
					file >> mesh->vertices[i].tangent.x;
					file >> mesh->vertices[i].tangent.y;
					file >> mesh->vertices[i].tangent.z;
					file >> mesh->vertices[i].bitangent.x;
					file >> mesh->vertices[i].bitangent.y;
					file >> mesh->vertices[i].bitangent.z;
				}

				for (int j = 0; j < mesh->indiceCount; ++j)
				{
					index = j;
					file >> mesh->indices[j];
				}

				// finished
				break;
			}
		}
		catch (std::ifstream::failure& ex)
		{
			Mon::Log::print("File read failed");
			Mon::Log::print(ex.what());
		}

		file.close();
	}


	void InitImage(Image* image, const char* file, bool flip)
	{
		stbi_set_flip_vertically_on_load(flip);
		image->data = stbi_load(file, &image->width, &image->height, &image->nrChannels, 0);
	}

	// If a texture is not using an image should it be freed?
	void FreeStbiImage(Image* image)
	{
		stbi_image_free(image->data);
	}

	void InitQuadMesh(Mesh* mesh, bool tangents)
	{
		// TODO(ck): Actual GUID
		mesh->id = "001";

		v2 size = v2(1.0f); // 1x1 meter
		int verticeCount = 4;

		// TODO(ck): Memory Allocation
		mesh->vertices = new MonGL::Vertex3D[verticeCount];
		mesh->vertices[0].position = v3(size.x/2.0f, size.y/2.0f, 0.0f);
		mesh->vertices[0].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[0].texCoords = v2(1.0f, 1.0f);

		mesh->vertices[1].position = v3(size.x/2.0f, -size.y/2.0f, 0.0f);
		mesh->vertices[1].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[1].texCoords = v2(1.0f, 0.0f);

		mesh->vertices[2].position = v3(-size.x/2.0f, -size.y/2.0f, 0.0f);
		mesh->vertices[2].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[2].texCoords = v2(0.0f, 0.0f);

		mesh->vertices[3].position = v3(-size.x/2.0f, size.y/2.0f, 0.0f);
		mesh->vertices[3].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[3].texCoords = v2(0.0f, 1.0f);

		if (tangents)
		{
			//data->vertices.clear();
			v3 pos1(0.5f, 0.5f, 0.0f);
			v3 pos2(0.5f, -0.5f, 0.0f);
			v3 pos3(-0.5f, -0.5f, 0.0f);
			v3 pos4(-0.5f, 0.5f, 0.0f);

			v2 uv1(1.0f, 1.0f);
			v2 uv2(1.0f, 0.0f);
			v2 uv3(0.0f, 0.0f);
			v2 uv4(0.0f, 1.0f);

			v3 normal(1.0f, 1.0f, 1.0f);

			v3 edge1 = pos2 - pos1;
			v3 edge2 = pos3 - pos1;
			v2 deltaUV1 = uv2 - uv1;
			v2 deltaUV2 = uv3 - uv1;

			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			//v3 tangent1, bitangent1;
			//v3 tangent2, bitangent2;
			f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		}

		int indiceCount = 6;
		mesh->indices = new unsigned int[indiceCount];
		mesh->indices[0] = 0;
		mesh->indices[1] = 1;
		mesh->indices[2] = 3;
		mesh->indices[3] = 1;
		mesh->indices[4] = 2;
		mesh->indices[5] = 3;

		mesh->verticeCount = verticeCount;
		mesh->indiceCount = indiceCount;
		mesh->type = RenderType::Quad;
		MonGL::UploadOpenGLMesh(mesh);
	}

	void Init2DQuadMesh(Mesh2D* mesh)
	{
		mesh->id = "2D_QUAD";

		int verticeCount = 4;
		mesh->vertices = new MonGL::Vertex[verticeCount];

		//const int size = 32;
		const int MAP_SIZE = 40;
		float x = (float)((MAP_SIZE / 2));
		float y = (float)((MAP_SIZE / 2));
		// TODO(ck): Vertices should be in -0.5 to 0.5 ? 
		// I think we can do the conversion from 16pixels wide to 0.5 here? 
		// if i used the size/2 method you would do 1=16, 2=32, 3=64 etc...
		// 
		// tileSizeInPixels=16 , tileSideInMeters=1
		// real32 metersToPixels = (real32)tileSideInPixels / (real32)tileMap->tileSideInMeters;
		// so this doesn't seem to matter here because metersToPixels will be equal to 16

		// it would be -0.5 to 0.5 with an offset?
		v2 size = v2(1.0f);
		mesh->vertices[0].position = v3(size.x / 2.0f, size.y / 2.0f, 0.0f);
		mesh->vertices[0].color = v3(1.0f, 0.0f, 0.0f);
		mesh->vertices[0].texCoords = v2(1.0f, 1.0f);

		mesh->vertices[1].position = v3(size.x / 2.0f, -size.y / 2.0f, 0.0f);
		mesh->vertices[1].color = v3(1.0f, 0.0, 0.0f);
		mesh->vertices[1].texCoords = v2(1.0f, 0.0f);
		
		mesh->vertices[2].position = v3(-size.x / 2.0f, -size.y / 2.0f, 0.0f);
		mesh->vertices[2].color = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[2].texCoords = v2(0.0f, 0.0f);

		mesh->vertices[3].position = v3(-size.x / 2.0f, size.y / 2.0f, 0.0f);
		mesh->vertices[3].color = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[3].texCoords = v2(0.0f, 1.0f);

		int indiceCount = 6;
		mesh->indices = new unsigned int[indiceCount];
		mesh->indices[0] = 0;
		mesh->indices[1] = 1;
		mesh->indices[2] = 3;
		mesh->indices[3] = 1;
		mesh->indices[4] = 2;
		mesh->indices[5] = 3;

		mesh->verticeCount = verticeCount;
		mesh->indiceCount = indiceCount;
		mesh->type = RenderType::Quad;
		MonGL::UploadOpenGLMesh2D(mesh);
	}

	void InitCubeMesh(Mesh* mesh)
	{
		mesh->id = "002";
		// Load from .vt file (need to do efficient as possible)
		// maybe dont need to do this but?? tilemap does a quad and its a huge
		// cubes can just be created with a macro PUSH_CUBE PUSH_QUAD x4?
		int verticeCount = 36;
		// TODO(ck): Memory Allocation
		mesh->vertices = new MonGL::Vertex3D[verticeCount];

		// TODO(ck): ADD QUAD method
		mesh->vertices[0].position = v3(-0.5f, -0.5f, -0.5f);
		mesh->vertices[0].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[0].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[1].position = v3(0.5f, -0.5f, -0.5f);
		mesh->vertices[1].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[1].texCoords = v2(1.0f, 0.0f);
		mesh->vertices[2].position = v3(0.5f, 0.5f, -0.5f);
		mesh->vertices[2].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[2].texCoords = v2(1.0f, 1.0f);
		mesh->vertices[3].position = v3(0.5f, 0.5f, -0.5f);
		mesh->vertices[3].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[3].texCoords = v2(1.0f, 1.0f);

		mesh->vertices[4].position = v3(-0.5f, 0.5f, -0.5f);
		mesh->vertices[4].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[4].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[5].position = v3(-0.5f, -0.5f, -0.5f);
		mesh->vertices[5].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[5].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[6].position = v3(-0.5f, -0.5f, 0.5f);
		mesh->vertices[6].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[6].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[7].position = v3(0.5f, -0.5f, 0.5f);
		mesh->vertices[7].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[7].texCoords = v2(1.0f, 0.0f);

		mesh->vertices[8].position = v3(0.5f, 0.5f, 0.5f);
		mesh->vertices[8].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[8].texCoords = v2(1.0f, 1.0f);
		mesh->vertices[9].position = v3(0.5f, 0.5f, 0.5f);
		mesh->vertices[9].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[9].texCoords = v2(1.0f, 1.0f);
		mesh->vertices[10].position = v3(-0.5f, 0.5f, 0.5f);
		mesh->vertices[10].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[10].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[11].position = v3(-0.5f, -0.5f, 0.5f);
		mesh->vertices[11].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[11].texCoords = v2(0.0f, 0.0f);

		mesh->vertices[12].position = v3(-0.5f, 0.5f, 0.5f);
		mesh->vertices[12].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[12].texCoords = v2(1.0f, 0.0f);
		mesh->vertices[13].position = v3(-0.5f, 0.5f, -0.5f);
		mesh->vertices[13].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[13].texCoords = v2(1.0f, 1.0f);
		mesh->vertices[14].position = v3(-0.5f, -0.5f, -0.5f);
		mesh->vertices[14].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[14].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[15].position = v3(-0.5f, -0.5f, -0.5f);
		mesh->vertices[15].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[15].texCoords = v2(0.0f, 1.0f);

		mesh->vertices[16].position = v3(-0.5f, -0.5f, 0.5f);
		mesh->vertices[16].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[16].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[17].position = v3(-0.5f, 0.5f, 0.5f);
		mesh->vertices[17].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[17].texCoords = v2(1.0f, 0.0f);
		mesh->vertices[18].position = v3(0.5f, 0.5f, 0.5f);
		mesh->vertices[18].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[18].texCoords = v2(1.0f, 0.0f);
		mesh->vertices[19].position = v3(0.5f, 0.5f, -0.5f);
		mesh->vertices[19].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[19].texCoords = v2(1.0f, 1.0f);

		mesh->vertices[20].position = v3(0.5f, -0.5f, -0.5f);
		mesh->vertices[20].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[20].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[21].position = v3(0.5f, -0.5f, -0.5f);
		mesh->vertices[21].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[21].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[22].position = v3(0.5f, -0.5f, 0.5f);
		mesh->vertices[22].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[22].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[23].position = v3(0.5f, 0.5f, 0.5f);
		mesh->vertices[23].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[23].texCoords = v2(1.0f, 0.0f);

		mesh->vertices[24].position = v3(-0.5f, -0.5f, -0.5f);
		mesh->vertices[24].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[24].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[25].position = v3(0.5f, -0.5f, -0.5f);
		mesh->vertices[25].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[25].texCoords = v2(1.0f, 1.0f);
		mesh->vertices[26].position = v3(0.5f, -0.5f, 0.5f);
		mesh->vertices[26].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[26].texCoords = v2(1.0f, 0.0f);
		mesh->vertices[27].position = v3(0.5f, -0.5f, 0.5f);
		mesh->vertices[27].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[27].texCoords = v2(1.0f, 0.0f);

		mesh->vertices[28].position = v3(-0.5f, -0.5f, 0.5f);
		mesh->vertices[28].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[28].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[29].position = v3(-0.5f, -0.5f, -0.5f);
		mesh->vertices[29].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[29].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[30].position = v3(-0.5f, 0.5f, -0.5f);
		mesh->vertices[30].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[30].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[31].position = v3(0.5f, 0.5f, -0.5f);
		mesh->vertices[31].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[31].texCoords = v2(1.0f, 1.0f);

		mesh->vertices[32].position = v3(0.5f, 0.5f, 0.5f);
		mesh->vertices[32].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[32].texCoords = v2(1.0f, 0.0f);
		mesh->vertices[33].position = v3(0.5f, 0.5f, 0.5f);
		mesh->vertices[33].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[33].texCoords = v2(1.0f, 0.0f);
		mesh->vertices[34].position = v3(-0.5f, 0.5f, 0.5f);
		mesh->vertices[34].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[34].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[35].position = v3(-0.5f, 0.5f, -0.5f);
		mesh->vertices[35].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[35].texCoords = v2(0.0f, 1.0f);

		mesh->type = RenderType::Cube;
		mesh->verticeCount = verticeCount;
		mesh->indiceCount = 0;
		MonGL::UploadOpenGLMesh(mesh);
	}

	void InitModelMesh(Mesh* mesh, const char* fileName)
	{
		// IMPORTANT(ck):
		// TODO(ck):  This can't be 003 there are always going to be more than one
		//			  model mesh. the other meshes are fine but the model mesh is dynamic
		mesh->id = "003";
		LoadImpFile(mesh, fileName);
		mesh->type = RenderType::Model;
		MonGL::UploadOpenGLMesh(mesh);
	}


	void InitGridMesh(Mesh* mesh, int xSize, int zSize)
	{
		// TODO(ck): 
/*
	Keep this as basic grid but make a new grid that is batched?
	can create the Grid struct and then have a batcher that it uploads
	itself too. that way we can start creating terrain?

	each cell is about the same size as 16 pixel texture wide... is there a way to define
	this in our vertices?

	can try and use ArrayTextures for this as well so there is no texture bleeding???
	we can get that DS game look using this


*/
		mesh->id = "004";
		mesh->type = RenderType::Model;
		int verticeCount = (xSize + 1) * (zSize + 1);
		mesh->vertices = new MonGL::Vertex3D[verticeCount];
		for (int index = 0, z = 0; z <= zSize; z++)
		{
			for (int x = 0; x <= xSize; x++, index++)
			{
				mesh->vertices[index] = {};
				mesh->vertices[index].position.x = (float)((x - 0.5) - (xSize/2));
				mesh->vertices[index].position.y = -0.5f;
				mesh->vertices[index].position.z = (float)((z + 0.5) - (zSize/2));

				mesh->vertices[index].normal.x = 0;
				mesh->vertices[index].normal.y = 1;
				mesh->vertices[index].normal.z = 0;

				mesh->vertices[index].texCoords.x = (float)x / (float)xSize;
				mesh->vertices[index].texCoords.y = (float)z / (float)zSize;
			}
		}

		/*
		*   NOTE(ck):
			build our grid using this method?

			for (int index = 0; i < sizeof(Grid); ++i)
			{
				data->vertices[index].position.x = grid[index].x;
				data->vertices[index].position.y = -0.5;
				data->vertices[index].position.z = grid[index].z;

			}


			NOTE(ck): This is from RPG Paper Maker they use a square size
			void Grid::initializeVertices(int w, int h, int squareSize) {
				m_vertices.clear();

				float w_f = (float)w, h_f = (float)h, squareSize_f = (float)squareSize;

				for (int i = 0; i <= w; i++){
					m_vertices.push_back(QVector3D((i * squareSize_f), 0.0f, 0.0f));
					m_vertices.push_back(QVector3D((i * squareSize_f), 0.0f,
												   squareSize_f * h_f));
				}
				for (int i = 0; i <= h; i++){
					m_vertices.push_back(QVector3D(0.0f, 0.0f,(i*squareSize_f)));
					m_vertices.push_back(QVector3D(squareSize_f * w_f, 0.0f,
												   (i * squareSize_f)));
				}
			}

			NOTE(ck): looks like they are adding 0.5 to the world position just like i am in the verts
			I wonder why they are doing the grid size like that? why does mine work?

			void Grid::paintGL(QMatrix4x4& modelviewProjection, int y) {
				m_program->bind();
				m_program->setUniformValue(u_modelviewProjection, modelviewProjection);
				m_program->setUniformValue(u_yPosition, y + 0.5f);
				{
				  m_vao.bind();
				  glDrawArrays(GL_LINES, 0, m_vertices.size());
				  m_vao.release();
				}
				m_program->release();
			}


		*/

		int indiceCount = xSize * zSize * 6;
		mesh->indices = new unsigned int[indiceCount];
		for (int ti = 0, vi = 0, z = 0; z < zSize; z++, vi++)
		{
			for (int x = 0; x < xSize; x++, ti += 6, vi++)
			{
				mesh->indices[ti] = vi;
				mesh->indices[ti + 3] = mesh->indices[ti + 2] = vi + 1;
				mesh->indices[ti + 4] = mesh->indices[ti + 1] = vi + xSize + 1;
				mesh->indices[ti + 5] = vi + xSize + 2;
			}
		}

		mesh->verticeCount = verticeCount;
		mesh->indiceCount = indiceCount;
		MonGL::UploadOpenGLMesh(mesh);

		//data->mat = {};
		//data->mat.ambient = v3(1.0f, 0.5f, 0.6f);
		//data->mat.diffuse = v3(1.0f, 0.5f, 0.31f);
		//data->mat.specular = v3(0.5f, 0.5f, 0.5f);
		//data->mat.shininess = 32.0f;


		// Set texture indexes
		//std::string textPath = "res/textures/terrain/1024multi.png";
		//Texture text = {};
		//data->textures[0] = text;
		//LoadTextureFile(&data->textures[0], textPath.c_str(), TextureType::Diffuse, false, false, true, false);

		//textPath = "res/textures/terrain/grass.jpg";
		//Texture text1 = {};
		//data->textures[1] = text1;
		//LoadTextureFile(&data->textures[1], textPath.c_str(), TextureType::Diffuse, false, false, true, false);

		//textPath = "res/textures/terrain/pix_grass.png";
		//Texture text2 = {};
		//data->textures[2] = text2;
		//LoadTextureFile(&data->textures[2], textPath.c_str(), TextureType::Diffuse, false, false, true, true);

		//textPath = "res/textures/terrain/snow.jpg";
		//Texture text3 = {};
		//data->textures[3] = text3;
		//LoadTextureFile(&data->textures[3], textPath.c_str(), TextureType::Diffuse, false, false, true, false);

	}



	void InitBoundingBoxMesh(Mesh* mesh)
	{
		mesh->id = "005";
		mesh->type = RenderType::Debug;
		int verticeCount = 8;
		// TODO(ck): Memory Allocation
		mesh->vertices = new MonGL::Vertex3D[verticeCount];
		mesh->vertices[0].position = v3(-0.5, -0.5, -0.5);
		mesh->vertices[0].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[0].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[1].position = v3(0.5, -0.5, -0.5);
		mesh->vertices[1].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[1].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[2].position = v3(0.5, 0.5, -0.5);
		mesh->vertices[2].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[2].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[3].position = v3(-0.5, 0.5, -0.5);
		mesh->vertices[3].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[3].texCoords = v2(0.0f, 0.0f);

		mesh->vertices[4].position = v3(-0.5, -0.5, 0.5);
		mesh->vertices[4].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[4].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[5].position = v3(0.5, -0.5, 0.5);
		mesh->vertices[5].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[5].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[6].position = v3(0.5, 0.5, 0.5);
		mesh->vertices[6].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[6].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[7].position = v3(-0.5, 0.5, 0.5);
		mesh->vertices[7].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[7].texCoords = v2(0.0f, 0.0f);

		unsigned short elements[] = {
			0, 1, 2, 3, // front 
			4, 5, 6, 7, // back
			0, 4, 1, 5, 2, 6, 3, 7 // back
		};
		int indiceCount = sizeof(elements) / sizeof(elements[0]);
		mesh->indices = (unsigned int*)elements;

		// Set world matrix to be the same size as the bounding box
		//data->worldMatrix = GetTransform(&data->size);

		mesh->verticeCount = verticeCount;
		mesh->indiceCount = indiceCount;
		MonGL::UploadOpenGLMesh(mesh);
	}

	void InitLineMesh(Mesh* mesh)
	{
		mesh->id = "006";
		int verticeCount = 2;
		mesh->vertices = new MonGL::Vertex3D[verticeCount];

		mesh->vertices[0].position = v3(0.0f, 0.0f, 1.0f);
		mesh->vertices[0].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[0].texCoords = v2(1.0f, 1.0f);

		mesh->vertices[1].position = v3(0.0f, 0.0f, 50.0f);
		mesh->vertices[1].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[1].texCoords = v2(1.0f, 1.0f);

		mesh->verticeCount = verticeCount;
		mesh->indiceCount = 0;
		mesh->type = RenderType::Debug;
		MonGL::UploadOpenGLMesh(mesh);
	}


};