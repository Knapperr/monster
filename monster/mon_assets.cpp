#include "mon_assets.h"
#include "mon_gl_render.h"

#include <stb_image/stb_image.h>

namespace Mon
{
	// IMPORTANT(ck): Asset manager defined here
	// TODO(ck): Memory management
	Assets* g_Assets = new Assets();

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
				// TODO(ck): Remove name from .imp file shouldn't have a name
				// its just triangles that the entity is quickly loading
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

		int verticeCount = 4;
		// TODO(ck): Memory Allocation
		mesh->vertices = new MonGL::Vertex3D[verticeCount];
		mesh->vertices[0].position = v3(0.5f, 0.5f, 0.0f);
		mesh->vertices[0].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[0].texCoords = v2(1.0f, 1.0f);

		mesh->vertices[1].position = v3(0.5f, -0.5f, 0.0f);
		mesh->vertices[1].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[1].texCoords = v2(1.0f, 0.0f);

		mesh->vertices[2].position = v3(-0.5f, -0.5f, 0.0f);
		mesh->vertices[2].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[2].texCoords = v2(0.0f, 0.0f);

		mesh->vertices[3].position = v3(-0.5f, 0.5f, 0.0f);
		mesh->vertices[3].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[3].texCoords = v2(0.0f, 1.0f);

		// TODO(ck): split out
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
		int verticeCount = (xSize + 1) * (zSize + 1);
		mesh->vertices = new MonGL::Vertex3D[verticeCount];
		for (int index = 0, z = 0; z <= zSize; z++)
		{
			for (int x = 0; x <= xSize; x++, index++)
			{
				mesh->vertices[index] = {};
				mesh->vertices[index].position.x = x;
				mesh->vertices[index].position.y = -0.5f;
				mesh->vertices[index].position.z = z;

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