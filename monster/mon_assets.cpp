#include "mon_assets.h"
#include "mon_gl_render.h"

#include <stb_image/stb_image.h>

namespace Mon
{
	// IMPORTANT(ck): Asset manager defined here
	// TODO(ck): Memory management
	// does this get set to null for allocation we want this to happen after the game memory has been created
	Assets* g_Assets = new Assets();

// TODO(ck): Move to platform layer
#include <fstream>
#include <string>
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

		AddMesh(assets);
		Mesh* pumpkin = GetMesh(assets, 11);
		InitModelMesh(pumpkin, "pumpkin.imp");

		// empty #0 for image
		AddImage(assets);
		// empty #0 for textureAssets
		AddTextureAsset(assets);

		std::ifstream file("config_assets.mon");
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			if (!file.is_open())
				Mon::Log::print("Failure to open asset file");

			// Parse Images 
			std::string line;
			int imageCount = 0;
			while (file >> line)
			{
				std::string categoryLine = line;
				file >> imageCount;
				break;
			}
			
			bool flipImage = true;
			while (file >> line)
			{
				for (int i = 1; i <= imageCount; ++i)
				{
					file >> flipImage;

					AddImage(assets);
					Image* img = GetImage(assets, i);
					InitImage(img, line.c_str(), flipImage);

					file >> line;
				}
				break;
			}

			
			// Parse Textures
			// NOTE(ck): Already parsed the category line above because i do file >> line after InitImage
			int textureAssetCount = 0;
			file >> textureAssetCount;

			int type = 0;
			bool isPixelArt = true;
			int imageIndex = 0;

			for (int i = 1; i < textureAssetCount; ++i)
			{
				file >> type;
				file >> isPixelArt;
				file >> imageIndex;

				AddTextureAsset(assets);
				TextureAsset* asset = GetTextureAsset(assets, i);
				InitTextureAsset(asset, (MonGL::TextureType)type, isPixelArt, imageIndex);
			}
		}
		catch (std::ifstream::failure& ex)
		{
			Mon::Log::print("File read failed");
			Mon::Log::print(ex.what());
		}
		file.close();

		// 2d qaud mesh
		Init2DQuadMesh(&assets->quad2D);
	}


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
	void FreeImage(Image* image)
	{
		stbi_image_free(image->data);
	}

	void InitTextureAsset(TextureAsset* asset, MonGL::TextureType type, bool pixelArt, int imageIndex)
	{
		asset->type = type;
		asset->isPixelArt = pixelArt;
		asset->imageIndex = imageIndex;
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
		//v2 size = v2(2.0f)
		v2 pos = v2(1.0f, 1.0f);
		float size = 2.0f; // vertice
		float tileSize = 16.0f;
		//mesh->vertices[0].position = v3(0.0f, 0.0f, 0.0f);
		//mesh->vertices[0].position = v3((size.x / 2.0f) * pixelSize, (size.y / 2.0f) * pixelSize, 0.0f);
		
		// this one works but scaling is bad
		mesh->vertices[0].position = v3(pos.x * tileSize, pos.y * tileSize, 0.0f);
		//mesh->vertices[0].position = v3(pos.x, pos.y, 0.0f);
		mesh->vertices[0].color = v3(1.0f, 0.0f, 0.0f);
		mesh->vertices[0].texCoords = v2(0.0f, 0.0f);

		//mesh->vertices[1].position = v3((0.0f + 1.0f), (0.0f), 0.0f);
		//mesh->vertices[1].position = v3((size.x / 2.0f) * pixelSize, (-size.y / 2.0f) * pixelSize, 0.0f);
		mesh->vertices[1].position = v3((pos.x + size) * tileSize, pos.y * tileSize, 0.0f);
		mesh->vertices[1].color = v3(1.0f, 0.0, 0.0f);
		mesh->vertices[1].texCoords = v2(1.0f, 0.0f);
		
		//mesh->vertices[2].position = v3((0.0f + 1.0f), (0.0f + 1.0f), 0.0f);
		//mesh->vertices[2].position = v3((-size.x / 2.0f) * pixelSize, (-size.y / 2.0f) * pixelSize, 0.0f);
		mesh->vertices[2].position = v3((pos.x + size) * tileSize, (pos.y + size) * tileSize, 0.0f);
		mesh->vertices[2].color = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[2].texCoords = v2(1.0f, 1.0f);

		//mesh->vertices[3].position = v3((0.0f), (0.0f + 1.0f), 0.0f);
		//mesh->vertices[3].position = v3((-size.x / 2.0f) * pixelSize, (size.y / 2.0f) * pixelSize, 0.0f);
		mesh->vertices[3].position = v3(pos.x * tileSize, (pos.y + size) * tileSize, 0.0f);
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

		// TODO(ck): Try this
		// https://stackoverflow.com/questions/58494179/how-to-create-a-grid-in-opengl-and-drawing-it-with-lines
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
		mesh->id = "BOUNDING_BOX";
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