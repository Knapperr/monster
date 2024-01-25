#include "mon_assets.h"
#include "mon_gl_render.h"

#include <stb_image/stb_image.h>

namespace Mon
{
	// IMPORTANT(ck): Asset manager defined here
	Assets* g_Assets = nullptr;

// TODO(ck): Move to platform layer
// Need to open files from the platform layer not in here
// or get a file handle from the platform layer or directory iterator???

#include <fstream>
#include <string>
#include <windows.h>
	void InitAssets(Assets* assets)
	{
		Mon::Log::print("Init assets...");
		// Go directly into the folder and load them
		//empty mesh #0
		int index = AddMesh(assets);

		// This is for the terrain
		index = AddMesh(assets);
		Mesh* gridMesh = GetMesh(assets, index);
		InitGridMesh(gridMesh, 64, 64);
		
		WIN32_FIND_DATA findFileData;
		// D:/Programming/$$GameProjects/monster/monster/res/models
		// res/models
		// You need to add the * so it knows you want to look at the files?
		HANDLE hFind = FindFirstFileW(L"res/models/*", &findFileData);
		bool loadModels = true;
		if (hFind == INVALID_HANDLE_VALUE)
		{
			Mon::Log::warn("unable to load model path");
			loadModels = false;
		}

		if (loadModels)
		{
			do
			{
				if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					Mon::Log::print("inside directory");
					//if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0) 
					//{
					//	//std::wcout << L"Folder: " << findFileData.cFileName << std::endl;
					//	
					//}
				}
				else
				{
					Mon::Log::print("inside file");
					int index = AddMesh(assets);
					Mesh* mesh = GetMesh(assets, index);
					// TODO(ck): Fix this absolute dog's breakfast of a mess
					std::wstring wideFileName = findFileData.cFileName;
					std::string fileName = std::string(wideFileName.begin(), wideFileName.end());
					std::string meshPath = "res/models/" + fileName;
					InitModelMesh(mesh, meshPath.c_str());

				}

			} while (FindNextFile(hFind, &findFileData) != 0);

		}
		FindClose(hFind);

		// empty #0 for image
		AddImage(assets);
		// empty #0 for textureAssets
		AddTextureAsset(assets);
		// empty #0 for textureAtlas
		AddTextureAtlas(assets);

		// TODO(ck): Move file reading to platform layer do not want this in the game
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
			// IMPORTANT(ck):
			// NOTE(ck): Already parsed the category line above because i do file >> line after InitImage
			int textureAssetCount = 0;
			file >> textureAssetCount;

			std::string textureName;
			int type = 0;
			bool isPixelArt = true;
			int imageIndex = 0;

			for (int i = 1; i <= textureAssetCount; ++i)
			{
				file >> textureName;
				file >> type;
				file >> isPixelArt;
				file >> imageIndex;

				AddTextureAsset(assets);
				TextureAsset* asset = GetTextureAsset(assets, i);
			

				InitTextureAsset(asset, textureName, (MonGL::TextureType)type, isPixelArt, imageIndex);
			}

			int atlasCount = 0;
			int atlasSize = 0;
			int tileSize = 0;
			int textureAssetIndex = 0;

			file >> line; // category
			file >> atlasCount;
			for (int i = 1; i <= atlasCount; ++i)
			{
				file >> textureAssetIndex;
				file >> atlasSize;
				file >> tileSize;

				AddTextureAtlas(assets);
				TextureAtlas* atlas = GetTextureAtlas(assets, i);
				InitTextureAtlas(atlas, textureAssetIndex, atlasSize, tileSize);
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

	void InitAssets2D(Assets* assets, const char* fileName)
	{
		return;
	}


	void LoadImpFile(Mesh* mesh, const char* filename)
	{
		// ensure file can be loaded properly for the engine
		// this keeps the test code out of the engine
		std::ifstream file(filename, std::ios::in | std::ios::binary);
		file.read(reinterpret_cast<char*>(&mesh->verticeCount), sizeof(int));

		mesh->vertices = new MonGL::Vertex3D[mesh->verticeCount];
		file.read(reinterpret_cast<char*>(mesh->vertices), sizeof(MonGL::Vertex3D) * mesh->verticeCount);


		file.read(reinterpret_cast<char*>(&mesh->indiceCount), sizeof(int));
		mesh->indices = new unsigned short int[mesh->indiceCount];
		file.read(reinterpret_cast<char*>(mesh->indices), sizeof(unsigned short int) * mesh->indiceCount);

		file.close();


		float minX = 0.0f;
		float maxX = 0.0f;
		float minY = 0.0f;
		float maxY = 0.0f;
		float minZ = 0.0f;
		float maxZ = 0.0f;

		for (int vertIndex = 0; vertIndex < mesh->verticeCount; ++vertIndex)
		{
			if (mesh->vertices[vertIndex].position.x > maxX) maxX = mesh->vertices[vertIndex].position.x;
			if (mesh->vertices[vertIndex].position.x < minX) minX = mesh->vertices[vertIndex].position.x;

			if (mesh->vertices[vertIndex].position.y > maxY) maxY = mesh->vertices[vertIndex].position.y;
			if (mesh->vertices[vertIndex].position.y < minY) minY = mesh->vertices[vertIndex].position.y;

			if (mesh->vertices[vertIndex].position.z > maxZ) maxZ = mesh->vertices[vertIndex].position.z;
			if (mesh->vertices[vertIndex].position.z < minZ) minZ = mesh->vertices[vertIndex].position.z;
		}

		mesh->bbox.min = glm::vec3(minX, minY, minZ);
		mesh->bbox.max = glm::vec3(maxX, maxY, maxZ);
	}

	void InitImage(Image* image, const char* file, bool flip)
	{
		MonGL::Log::print("loading image", file);

		stbi_set_flip_vertically_on_load(flip);
		image->data = stbi_load(file, &image->width, &image->height, &image->nrChannels, 0);
	}

	// If a texture is not using an image should it be freed?
	void FreeImage(Image* image)
	{
		stbi_image_free(image->data);
	}

	void InitTextureAsset(TextureAsset* asset, std::string &name, MonGL::TextureType type, bool pixelArt, int imageIndex)
	{

		asset->name = std::string(name);
		asset->type = type;
		asset->isPixelArt = pixelArt;
		asset->imageIndex = imageIndex;
	}

	void InitQuadMesh(Mesh* mesh, bool tangents)
	{
		Mon::Log::print("Loading quad mesh");

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
		mesh->indices = new unsigned short int[indiceCount];
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
		Mon::Log::print("Loading 2D quad mesh");
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
		Mon::Log::print("Loading cube mesh");

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
		std::string msg = "Loading mesh: " + std::string(fileName);
		Mon::Log::print(msg.c_str());

		mesh->id = fileName;
		LoadImpFile(mesh, fileName);
		mesh->type = RenderType::Model;

		// separate this out we don't need to do this in the asset layer
		//MonGL::UploadOpenGLMesh(mesh);
		MonGL::UploadGLMesh(mesh);
	}

	void InitGridMesh(Mesh* mesh, int xSize, int zSize)
	{
		Mon::Log::print("Loading grid mesh");

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
				mesh->vertices[index].position.y = 0.0f;
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
		// TODO(ck): Memory allocation
		mesh->indices = new unsigned short int[indiceCount];
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
		Mon::Log::print("Loading cube map");

		mesh->id = "CUBEMAP";
		mesh->type = RenderType::CubeMap;

		// TODO(ck): Memory Allocation
		mesh->verticeCount = 36;
		mesh->vertices = new MonGL::Vertex3D[mesh->verticeCount];
	}

	void InitBoundingBoxMesh(Mesh* mesh)
	{
		Mon::Log::print("Loading bounding box mesh");

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
		mesh->vertices[7].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[7].position = v3(-0.5, 0.5, 0.5);
		mesh->vertices[7].texCoords = v2(0.0f, 0.0f);

		unsigned short elements[] = {
			0, 1, 2, 3, // front 
			4, 5, 6, 7, // back
			0, 4, 1, 5, 2, 6, 3, 7 // back
		};
		int indiceCount = sizeof(elements) / sizeof(elements[0]);
		mesh->indices = (unsigned short int*)elements;

		// Set world matrix to be the same size as the bounding box
		//data->worldMatrix = GetTransform(&data->size);

		mesh->verticeCount = verticeCount;
		mesh->indiceCount = indiceCount;
		MonGL::UploadOpenGLMesh(mesh);
	}
	
	void InitTextureAtlas(TextureAtlas* atlas, int assetIndex, int atlasSize, int tileSize)
	{
		TextureAsset* t = GetTextureAsset(g_Assets, assetIndex);
		Mon::Log::print("Loading texture atlas", t->name.c_str());

		atlas->textureAssetIndex = assetIndex;

		atlas->size = atlasSize;
		atlas->tileSize = tileSize;
		atlas->rows = atlasSize / tileSize;
		atlas->cols = atlasSize / tileSize;


		// TODO(ck): Temp for now
		int index = 0;
		for (int row = 0; row < atlas->rows; ++row)
		{
			for (int col = 0; col < atlas->cols; ++col)
			{
				AssetTile tile = {};
				tile.x = col;
				tile.y = row;
				tile.id = index;
				atlas->tiles[index] = tile;
				
				if (tile.x == 4 && tile.y == 2) {
					// top 4,2
					int i = index; // 36
				}
				if (tile.x == 0 && tile.y == 4) {
					// left 0,4
					int i = index; // 64
				}
				if (tile.x == 5 && tile.y == 5) {
					// right 5,5
					int i = index; // 85
				}
				if (tile.x == 1 && tile.y == 11) {
					// field 1,11
					int i = index; // 177
				}
				if (tile.x == 0 && tile.y == 7) {
					// bottom left 0,7
					int i = index; // 112
				}
				if (tile.x == 5 && tile.y == 7) {
					// bottom right 5,7
					int i = index; // 117
				}
				if (tile.x == 5 && tile.y == 2) {
					// top right 5,2
					int i = index; // 37
				}
				if (tile.x == 0 && tile.y == 2) {
					// top left 0,2
					int i = index; // 32
				}
				if (tile.x == 3 && tile.y == 6) {
					// bottom 3,6
					int i = index; // 99
				}
				if (tile.x == 3 && tile.y == 11) {
					// bush 3,11
					int i = index; // 179
				}
				if (tile.x == 1 && tile.y == 14) {
					// rock 1,14
					int i = index; // 225
				}
				if (tile.x == 6 && tile.y == 13) {
					// road top 6,13
					int i = index; // 214
				}
				if (tile.x == 6 && tile.y == 12) {
					// road mid 6,12
					int i = index; // 198
				}
				if (tile.x == 6 && tile.y == 11) {
					// road bottom 6,11
					int i = index; // 182
				}
				if (tile.x == 7 && tile.y == 13) {
					// topleft tree 7,13
					int i = index; // 215
				}
				if (tile.x == 8 && tile.y == 13) {
					// topright tree 8,13
					int i = index; // 216
				}
				if (tile.x == 7 && tile.y == 12) {
					// midleft tree 7,12
					int i = index; // 199
				}
				if (tile.x == 8 && tile.y == 12) {
					// midright tree 8,12
					int i = index; // 200
				}
				if (tile.x == 7 && tile.y == 11) {
					// bottomleft tree 7,11
					int i = index; // 183
				}
				if (tile.x == 8 && tile.y == 11) {
					// bottom right tree 8,11
					int i = index; // 184
				}

				++index;

				
			}
		}
	}

};