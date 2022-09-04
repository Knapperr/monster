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

		AddMesh(assets);
		Mesh* house = GetMesh(assets, 10);
		InitModelMesh(house, "village_house.imp");


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


		// TODO(ck): !!! IMPORTANT(ck):
		/*  DEAL WITH THIS */
		AddImage(assets);
		Image* skybox1 = GetImage(assets, 24);
		InitImage(skybox1, "res/textures/skyboxsun5deg2/right.bmp", false);
		
		AddImage(assets);
		Image* skybox2 = GetImage(assets, 25);
		InitImage(skybox2, "res/textures/skyboxsun5deg2/left.bmp", false);

		AddImage(assets);
		Image* skybox3 = GetImage(assets, 26);
		InitImage(skybox3, "res/textures/skyboxsun5deg2/top.bmp", false);

		AddImage(assets);
		Image* skybox4 = GetImage(assets, 27);
		InitImage(skybox4, "res/textures/skyboxsun5deg2/bottom.bmp", false);

		AddImage(assets);
		Image* skybox5 = GetImage(assets, 28);
		InitImage(skybox5, "res/textures/skyboxsun5deg2/front.bmp", false);

		AddImage(assets);
		Image* skybox6 = GetImage(assets, 29);
		InitImage(skybox6, "res/textures/skyboxsun5deg2/back.bmp", false);
		/*  DEAL WITH THIS */
		// TODO(ck): !!! IMPORTANT(ck):


		AddImage(assets);
		Image* houseSheet = GetImage(assets, 30);
		InitImage(houseSheet, "res/textures/Sprite.png", false);

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


			float minX = 0.0f;
			float maxX = 0.0f;
			float minY = 0.0f;
			float maxY = 0.0f;
			float minZ = 0.0f;
			float maxZ = 0.0f;
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


					if (mesh->vertices[i].position.x > maxX) maxX = mesh->vertices[i].position.x;
					if (mesh->vertices[i].position.x < minX) minX = mesh->vertices[i].position.x;

					if (mesh->vertices[i].position.y > maxY) maxY = mesh->vertices[i].position.y;
					if (mesh->vertices[i].position.y < minY) minY = mesh->vertices[i].position.y;

					if (mesh->vertices[i].position.z > maxZ) maxZ = mesh->vertices[i].position.z;
					if (mesh->vertices[i].position.z < minZ) minZ = mesh->vertices[i].position.z;
				}

				for (int j = 0; j < mesh->indiceCount; ++j)
				{
					index = j;
					file >> mesh->indices[j];
				}
				mesh->min = v3(minX, minY, minZ);
				mesh->max = v3(maxX, maxY, maxZ);
			/*
				glm::vec3 size = glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z);
				glm::vec3 center = glm::vec3((min_x+max_x)/2, (min_y+max_y)/2, (min_z+max_z)/2);
				glm::mat4 transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);

				Apply object's transformation matrix 
				glm::mat4 m = mesh->object2world * transform;
				glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(m));

			*/


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
		mesh->id = "QUAD";

		v2 size = v2(1.0f); // 1x1 meter
		int verticeCount = 4;

		// TODO(ck): Memory Allocation
		mesh->vertices = new MonGL::Vertex3D[verticeCount];
		mesh->vertices[0].position = v3(size.x / 2.0f, size.y / 2.0f, 0.0f);
		mesh->vertices[0].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[0].texCoords = v2(1.0f, 1.0f);

		mesh->vertices[1].position = v3(size.x / 2.0f, -size.y / 2.0f, 0.0f);
		mesh->vertices[1].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[1].texCoords = v2(1.0f, 0.0f);

		mesh->vertices[2].position = v3(-size.x / 2.0f, -size.y / 2.0f, 0.0f);
		mesh->vertices[2].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[2].texCoords = v2(0.0f, 0.0f);

		mesh->vertices[3].position = v3(-size.x / 2.0f, size.y / 2.0f, 0.0f);
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

		mesh->min = v3(-0.5f, -0.5f, -0.5f);
		mesh->max = v3(0.5f, 0.5f, 0.5f);

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

		// 2 = 32pixels
		v2 size = v2(2.0f);
		//mesh->vertices[0].position = v3(0.0f, 0.0f, 0.0f);
		mesh->vertices[0].position = v3(size.x / 2.0f, size.y / 2.0f, 0.0f);
		mesh->vertices[0].color = v3(1.0f, 0.0f, 0.0f);
		mesh->vertices[0].texCoords = v2(1.0f, 1.0f);

		//mesh->vertices[1].position = v3((0.0f + 1.0f), (0.0f), 0.0f);
		mesh->vertices[1].position = v3(size.x / 2.0f, -size.y / 2.0f, 0.0f);
		mesh->vertices[1].color = v3(1.0f, 0.0, 0.0f);
		mesh->vertices[1].texCoords = v2(1.0f, 0.0f);
		
		//mesh->vertices[2].position = v3((0.0f + 1.0f), (0.0f + 1.0f), 0.0f);
		mesh->vertices[2].position = v3(-size.x / 2.0f, -size.y / 2.0f, 0.0f);
		mesh->vertices[2].color = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[2].texCoords = v2(0.0f, 0.0f);

		//mesh->vertices[3].position = v3((0.0f), (0.0f + 1.0f), 0.0f);
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
		mesh->id = "CUBE";
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
		mesh->min = v3(-0.5f, -0.5f, -0.5f);
		mesh->max = v3(0.5f, 0.5f, 0.5f);
		MonGL::UploadOpenGLMesh(mesh);
	}

	void InitModelMesh(Mesh* mesh, const char* fileName)
	{
		// IMPORTANT(ck):
		// TODO(ck):  This can't be 003 there are always going to be more than one
		//			  model mesh. the other meshes are fine but the model mesh is dynamic
		mesh->id = fileName;
		LoadImpFile(mesh, fileName);
		mesh->type = RenderType::Model;
		MonGL::UploadOpenGLMesh(mesh);
	}


	void InitGridMesh(Mesh* mesh, int xSize, int zSize)
	{
		mesh->id = "GRID";
		mesh->type = RenderType::Model;
		int verticeCount = (xSize + 1) * (zSize + 1);
		// TODO(ck): Memory allocation
		mesh->vertices = new MonGL::Vertex3D[verticeCount];
		for (int index = 0, z = -zSize/2; z <= zSize/2; z++)
		{
			for (int x = -xSize/2; x <= xSize/2; x++, index++)
			{
				mesh->vertices[index] = {};
				//mesh->vertices[index].position.x = (float)((x + 0.5f) - (xSize / 2.0f));
				mesh->vertices[index].position.x = (float)((x));
				mesh->vertices[index].position.y = -0.5f;
				//mesh->vertices[index].position.z = (float)((z + 0.5f) - (zSize / 2.0f));
				mesh->vertices[index].position.z = (float)((z));

				mesh->vertices[index].normal.x = 0;
				mesh->vertices[index].normal.y = 1;
				mesh->vertices[index].normal.z = 0;

				mesh->vertices[index].texCoords.x = (float)x / (float)xSize;
				mesh->vertices[index].texCoords.y = (float)z / (float)zSize;
			}
		}

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
	}

	void InitCubeMapMesh(Mesh* mesh)
	{
		mesh->id = "CUBEMAP";
		mesh->type = RenderType::CubeMap;

		// TODO(ck): Memory Allocation
		mesh->verticeCount = 36;
		mesh->vertices = new MonGL::Vertex3D[mesh->verticeCount];
		
		

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