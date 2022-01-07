#include "mon_gl_render.h"

#include <glad/glad.h>

namespace MonGL
{
	// TODO(ck): TODO(ck): Somehow put into game so we can call from gui
	// can have a getter method that retrieves the globalDrawCalls from here
	int globalDrawCalls = 0;

	void LoadTexture(RenderData *data, int index, TextureType type, int shaderID, std::string path, bool pixelTexture)
	{
		data->texturePath = path;
		Texture text = {};
		LoadTextureFile(&text, path.c_str(), type, false, true, pixelTexture);
		data->textures[index] = text;
		data->selectedTexture = 0;
		glUniform1i(glGetUniformLocation(shaderID, "texture_diffuse1"), 0);
	}

	/// 
	/// Init RenderData
	/// 

	void InitQuad(RenderData* data, bool tangents)
	{		
		data->verticeCount = 4;
		// TODO(ck): Memory Allocation
		data->vertices = new Vertex3D[data->verticeCount];
		data->vertices[0].position = v3(0.5f, 0.5f, 0.0f);
		data->vertices[0].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[0].texCoords = v2(1.0f, 1.0f);

		data->vertices[1].position = v3(0.5f, -0.5f, 0.0f);
		data->vertices[1].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[1].texCoords = v2(1.0f, 0.0f);

		data->vertices[2].position = v3(-0.5f, -0.5f, 0.0f); 
		data->vertices[2].normal = v3(1.0f, 1.0f, 1.0f); 
		data->vertices[2].texCoords = v2(0.0f, 0.0f);

		data->vertices[3].position = v3(-0.5f, 0.5f, 0.0f);
		data->vertices[3].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[3].texCoords = v2(0.0f, 1.0f);

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

			v3 tangent1, bitangent1;
			v3 tangent2, bitangent2;


			f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		}

		unsigned int indices[] = {
			0, 1, 3,
			1, 2, 3
		};
		data->indiceCount = 6;

		unsigned int EBO;
		glGenVertexArrays(1, &data->VAO);
		glGenBuffers(1, &data->VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(data->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
		//glBufferData(GL_ARRAY_BUFFER, data->vertices.size() * sizeof(Vertex3D), &data->vertices[0], GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, data->verticeCount * sizeof(Vertex3D), data->vertices, GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)0);
		
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));
		
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, texCoords));

#if 0
		if (tangents)
		{
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, tangent));

			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, bitangent));
		}
#endif

		// unbind
		glBindVertexArray(0);

		data->visible = true;
		data->scale = v3(1.0f);
	}

	void InitBoundingBox(RenderData* data)
	{
		data->lineWidth = 2;
		data->color = v3(0.7f, 0.15f, 0.4f);

		data->verticeCount = 8;
		// TODO(ck): Memory Allocation
		data->vertices = new Vertex3D[data->verticeCount];
		data->vertices[0].position = v3(-0.5, -0.5, -0.5);
		data->vertices[0].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[0].texCoords = v2(0.0f, 0.0f);
		data->vertices[1].position  = v3(0.5, -0.5, -0.5);
		data->vertices[1].normal    = v3(1.0f, 1.0f, 1.0f);
		data->vertices[1].texCoords = v2(0.0f, 0.0f);
		data->vertices[2].position = v3(0.5, 0.5, -0.5);
		data->vertices[2].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[2].texCoords = v2(0.0f, 0.0f);
		data->vertices[3].position = v3(-0.5, 0.5, -0.5);
		data->vertices[3].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[3].texCoords = v2(0.0f, 0.0f);

		data->vertices[4].position = v3(-0.5, -0.5, 0.5);
		data->vertices[4].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[4].texCoords = v2(0.0f, 0.0f);
		data->vertices[5].position = v3(0.5, -0.5, 0.5);
		data->vertices[5].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[5].texCoords = v2(0.0f, 0.0f);
		data->vertices[6].position = v3(0.5, 0.5, 0.5);
		data->vertices[6].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[6].texCoords = v2(0.0f, 0.0f);
		data->vertices[7].position = v3(-0.5, 0.5, 0.5);
		data->vertices[7].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[7].texCoords = v2(0.0f, 0.0f);
	
		glGenVertexArrays(1, &data->VAO);
		glGenBuffers(1, &data->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
#if 0
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, sizeof(data->vertices), data->vertices, GL_STATIC_DRAW);
#endif
		glBufferData(GL_ARRAY_BUFFER, data->verticeCount * sizeof(Vertex3D), data->vertices, GL_STATIC_DRAW);
		glBindVertexArray(data->VAO);
		
		GLushort elements[] = {
			0, 1, 2, 3, // front 
			4, 5, 6, 7, // back
			0, 4, 1, 5, 2, 6, 3, 7 // back
		};
		data->indiceCount = sizeof(elements) / sizeof(elements[0]);

		glGenBuffers(1, &data->IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);  

		// Position
		glEnableVertexAttribArray(0);
#if 0
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
#endif
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		// Set world matrix to be the same size as the bounding box
		//data->worldMatrix = GetTransform(&data->size);

		// TODO(ck): Add back for cubes
		// Normals
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		//glEnableVertexAttribArray(1);

		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		//glEnableVertexAttribArray(2);

		data->visible = true;
	}


	void InitCube(RenderData* data)
	{
		// Load from .vt file (need to do efficient as possible)
		// maybe dont need to do this but?? tilemap does a quad and its a huge
		// cubes can just be created with a macro PUSH_CUBE PUSH_QUAD x4?
		data->verticeCount = 36;
		// TODO(ck): Memory Allocation
		data->vertices = new Vertex3D[data->verticeCount];

		data->vertices[0].position = v3(-0.5f, -0.5f, -0.5f);
		data->vertices[0].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[0].texCoords = v2(0.0f, 0.0f);
		data->vertices[1].position = v3(0.5f, -0.5f, -0.5f);
		data->vertices[1].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[1].texCoords = v2(1.0f, 0.0f);
		data->vertices[2].position = v3(0.5f, 0.5f, -0.5f);
		data->vertices[2].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[2].texCoords = v2(1.0f, 1.0f);
		data->vertices[3].position = v3(0.5f, 0.5f, -0.5f);
		data->vertices[3].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[3].texCoords = v2(1.0f, 1.0f);
		data->vertices[4].position = v3(-0.5f, 0.5f, -0.5f);
		data->vertices[4].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[4].texCoords = v2(0.0f, 1.0f);
		data->vertices[5].position = v3(-0.5f, -0.5f, -0.5f);
		data->vertices[5].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[5].texCoords = v2(0.0f, 0.0f);

		data->vertices[6].position = v3(-0.5f, -0.5f, 0.5f);
		data->vertices[6].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[6].texCoords = v2(0.0f, 0.0f);
		data->vertices[7].position = v3(0.5f, -0.5f, 0.5f);
		data->vertices[7].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[7].texCoords = v2(1.0f, 0.0f);
		data->vertices[8].position = v3(0.5f, 0.5f, 0.5f);
		data->vertices[8].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[8].texCoords = v2(1.0f, 1.0f);
		data->vertices[9].position = v3(0.5f, 0.5f, 0.5f);
		data->vertices[9].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[9].texCoords = v2(1.0f, 1.0f);
		data->vertices[10].position = v3(-0.5f, 0.5f, 0.5f);
		data->vertices[10].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[10].texCoords = v2(0.0f, 1.0f);
		data->vertices[11].position = v3(-0.5f, -0.5f, 0.5f);
		data->vertices[11].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[11].texCoords = v2(0.0f, 0.0f);

		data->vertices[12].position = v3(-0.5f, 0.5f, 0.5f);
		data->vertices[12].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[12].texCoords = v2(1.0f, 0.0f);
		data->vertices[13].position = v3(-0.5f, 0.5f, -0.5f);
		data->vertices[13].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[13].texCoords = v2(1.0f, 1.0f);
		data->vertices[14].position = v3(-0.5f, -0.5f, -0.5f);
		data->vertices[14].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[14].texCoords = v2(0.0f, 1.0f);
		data->vertices[15].position = v3(-0.5f, -0.5f, -0.5f);
		data->vertices[15].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[15].texCoords = v2(0.0f, 1.0f);
		data->vertices[16].position = v3(-0.5f, -0.5f, 0.5f);
		data->vertices[16].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[16].texCoords = v2(0.0f, 0.0f);
		data->vertices[17].position = v3(-0.5f, 0.5f, 0.5f);
		data->vertices[17].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[17].texCoords = v2(1.0f, 0.0f);

		data->vertices[18].position = v3(0.5f, 0.5f, 0.5f);
		data->vertices[18].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[18].texCoords = v2(1.0f, 0.0f);
		data->vertices[19].position = v3(0.5f, 0.5f, -0.5f);
		data->vertices[19].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[19].texCoords = v2(1.0f, 1.0f);
		data->vertices[20].position = v3(0.5f, -0.5f, -0.5f);
		data->vertices[20].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[20].texCoords = v2(0.0f, 1.0f);
		data->vertices[21].position = v3(0.5f, -0.5f, -0.5f);
		data->vertices[21].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[21].texCoords = v2(0.0f, 1.0f);
		data->vertices[22].position = v3(0.5f, -0.5f, 0.5f);
		data->vertices[22].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[22].texCoords = v2(0.0f, 0.0f);
		data->vertices[23].position = v3(0.5f, 0.5f, 0.5f);
		data->vertices[23].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[23].texCoords = v2(1.0f, 0.0f);

		data->vertices[24].position = v3(-0.5f, -0.5f, -0.5f);
		data->vertices[24].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[24].texCoords = v2(0.0f, 1.0f);
		data->vertices[25].position = v3(0.5f, -0.5f, -0.5f);
		data->vertices[25].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[25].texCoords = v2(1.0f, 1.0f);
		data->vertices[26].position = v3(0.5f, -0.5f, 0.5f);
		data->vertices[26].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[26].texCoords = v2(1.0f, 0.0f);
		data->vertices[27].position = v3(0.5f, -0.5f, 0.5f);
		data->vertices[27].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[27].texCoords = v2(1.0f, 0.0f);
		data->vertices[28].position = v3(-0.5f, -0.5f, 0.5f);
		data->vertices[28].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[28].texCoords = v2(0.0f, 0.0f);
		data->vertices[29].position = v3(-0.5f, -0.5f, -0.5f);
		data->vertices[29].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[29].texCoords = v2(0.0f, 1.0f);

		data->vertices[30].position = v3(-0.5f, 0.5f, -0.5f);
		data->vertices[30].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[30].texCoords = v2(0.0f, 1.0f);
		data->vertices[31].position = v3(0.5f, 0.5f, -0.5f);
		data->vertices[31].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[31].texCoords = v2(1.0f, 1.0f);
		data->vertices[32].position = v3(0.5f, 0.5f, 0.5f);
		data->vertices[32].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[32].texCoords = v2(1.0f, 0.0f);
		data->vertices[33].position = v3(0.5f, 0.5f, 0.5f);
		data->vertices[33].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[33].texCoords = v2(1.0f, 0.0f);
		data->vertices[34].position = v3(-0.5f, 0.5f, 0.5f);
		data->vertices[34].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[34].texCoords = v2(0.0f, 0.0f);
		data->vertices[35].position = v3(-0.5f, 0.5f, -0.5f);
		data->vertices[35].normal = v3(1.0f, 1.0f, 1.0f);
		data->vertices[35].texCoords = v2(0.0f, 1.0f);

		glGenVertexArrays(1, &data->VAO);
		glGenBuffers(1, &data->VBO);
		glBindVertexArray(data->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
		glBufferData(GL_ARRAY_BUFFER, data->verticeCount * sizeof(Vertex3D), data->vertices, GL_STATIC_DRAW);

		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, texCoords));

		// TODO(ck): Check for no texture?
		// load a texture onto the cube
		data->indiceCount = 0;

		// unbind
		glBindVertexArray(0);

		data->visible = true;
		data->scale = v3(1.0f);
	}

	// NOTE(ck): Assumes the vertices and indices have been loaded
	void InitModel(RenderData* data)
	{
		glGenVertexArrays(1, &data->VAO);
		glGenBuffers(1, &data->VBO);
		glGenBuffers(1, &data->IBO);

		glBindVertexArray(data->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
		glBufferData(GL_ARRAY_BUFFER, data->verticeCount * sizeof(Vertex3D), data->vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->indiceCount * sizeof(unsigned int), data->indices, GL_STATIC_DRAW);

		// Set the vertex attribute poiinters
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, texCoords));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, tangent));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, bitangent));

		glBindVertexArray(0);

		data->visible = true;
		data->scale = v3(1.0f);
	}

	void InitInstancedData(InstancedData* data, int amount)
	{
		data->amount = amount;

		InitModel(&data->renderData);
		data->matrices = new mat4[data->amount];
	}
	
	void GenerateTerrain(RenderData* data, float* heightMap)
	{
		data->VAO = 0;
		data->VBO = 0;

		const int SIZE = 128;
		const int VERTEX_COUNT = 32;
		data->verticeCount = VERTEX_COUNT*VERTEX_COUNT;
		// TODO(ck): Memory Allocation
		data->vertices = new Vertex3D[data->verticeCount];

		// TODO(ck): Memory Allocation
		int* indices = new int[6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1)];
		int verticesLength = data->verticeCount;
		data->indiceCount = 6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1);

		int index = 0;
		for (int i = 0; i < VERTEX_COUNT; ++i)
		{
			for (int j = 0; j < VERTEX_COUNT; ++j)
			{
				data->vertices[index] = {};
				data->vertices[index].position.x = (float)j / ((float)VERTEX_COUNT - 1) * SIZE;

				data->vertices[index].position.y = -0.3f;
				heightMap[index] = -0.3f;

				data->vertices[index].position.z = (float)i / ((float)VERTEX_COUNT - 1) * SIZE;

				data->vertices[index].normal.x = 0;
				data->vertices[index].normal.y = 1;
				data->vertices[index].normal.z = 0;

				data->vertices[index].texCoords.x = (float)j / ((float)VERTEX_COUNT - 1);
				data->vertices[index].texCoords.y = (float)i / ((float)VERTEX_COUNT - 1);
				++index;
			}
		}

		index = 0;
		for (int gz = 0; gz < VERTEX_COUNT - 1; ++gz)
		{
			for (int gx = 0; gx < VERTEX_COUNT - 1; ++gx)
			{
				int topLeft = (gz * VERTEX_COUNT) + gx;
				int topRight = topLeft + 1;
				int bottomLeft = ((gz + 1) * VERTEX_COUNT) + gx;
				int bottomRight = bottomLeft + 1;

				indices[index++] = topLeft;
				indices[index++] = bottomLeft;
				indices[index++] = topRight;
				indices[index++] = topRight;
				indices[index++] = bottomLeft;
				indices[index++] = bottomRight;
			}
		}

		unsigned int ebo;
		glGenVertexArrays(1, &data->VAO);
		glGenBuffers(1, &data->VBO);
		glGenBuffers(1, &ebo);

		glBindVertexArray(data->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
		//glBufferData(GL_ARRAY_BUFFER, data->vertices.size() * sizeof(Vertex3D), &data->vertices[0], GL_STATIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, verticesLength * sizeof(Vertex3D), data->vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->indiceCount * sizeof(int), indices, GL_STATIC_DRAW);

		// pos
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)0);
		// normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));
		// tex coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, texCoords));

		// unbind
		glBindVertexArray(0);
		// TODO(ck): dont need to delete these yet?
		//delete[] vertices;
		delete[] indices;

		data->mat = {};
		data->mat.ambient = v3(1.0f, 0.5f, 0.6f);
		data->mat.diffuse = v3(1.0f, 0.5f, 0.31f);
		data->mat.specular = v3(0.5f, 0.5f, 0.5f);
		data->mat.shininess = 32.0f;

		std::string textPath = "res/textures/terrain/1024multi.png";
		std::string textDir = textPath.substr(0, textPath.find_last_of('/'));

		/*
		textureIds[0] = MonTexture::LoadTextureFile("1024multi.png", textDir, false);
		std::string filename = std::string(path);
		filename = directory + '/' + filename;
		*/
		Texture text = {};
		data->textures[0] = text;
		LoadTextureFile(&data->textures[0], textPath.c_str(), TextureType::Diffuse, false, false, false);

		textPath = "res/textures/terrain/grass.jpg";
		Texture text1 = {};
		data->textures[1] = text1;
		LoadTextureFile(&data->textures[1], textPath.c_str(), TextureType::Diffuse, false, false, false);

		textPath = "res/textures/terrain/pix_grass.png";
		Texture text2 = {};
		data->textures[2] = text2;
		LoadTextureFile(&data->textures[2], textPath.c_str(), TextureType::Diffuse, false, false, false);

		textPath = "res/textures/terrain/snow.jpg";
		Texture text3 = {};
		data->textures[3] = text3;
		LoadTextureFile(&data->textures[3], textPath.c_str(), TextureType::Diffuse, false, false, false);
	}


	void InitDistortedWater(RenderData* renderData, RenderSetup* setup)
	{
		/*
		TODO(ck): Can I have some kind of uniform structure.
			I dont think I need a uniform abstract. It looks like in handmade hero in
			handmade_renderer_opengl.cpp / .h
			that you use a program.. I'll have to research this because it looks like they
			hold all of the uniforms and data its just a matching struct to the shader im assuming
			casey doesn't use any inheritance he just has struct opengl_program_common
		*/

		// Create a quad first
		InitQuad(renderData, true);

		setup->tiling = 5.0f;
		setup->speed = 0.3f;
		setup->flowStrength = 0.05f;
		setup->flowOffset = -0.23f;
		setup->heightScale = 0.1f;
		setup->heightScaleModulated = 8.0f;

		// CREATE A BASIC SHAPE LOADER replace ASSIMP
		// WE WILL HAVE TO PUSH THIS TEXTURE TO THE TEXUTES OF THE QUAD 
		// IN model.h
		// data->texturePath = texturePath;

		// type = "texture_diffuse"
		std::string path = "res/textures/water/water.png";
		Texture uv = {};
		LoadTextureFile(&uv, path.c_str(), TextureType::Diffuse, false, true);
		

		renderData->textures[0] = uv;

		// type = "texture_normal"
		path = "res/textures/water/flow-speed-noise.png";
		Texture flow = {};
		LoadTextureFile(&flow, path.c_str(), TextureType::Normal, false, true);
		
		renderData->textures[1] = flow;

		// type = "texture_normal"
		path = "res/textures/water/water-derivative-height.png";
		Texture normal = {};
		LoadTextureFile(&normal, path.c_str(), TextureType::Normal, false, true);
		renderData->textures[2] = normal;
		
	}

	void BeginRender(Config* config, mat4 projection, mat4 view, int shaderID)
	{
		MonGL::ViewPort(&config->viewPort);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		globalDrawCalls = 0;
	}

	///
	/// Draw RenderData  
	///

	void Draw(Config* config, RenderData* data, v3 pos, Camera* camera,
			  unsigned int shaderID, int selectedTexture)
	{
	
		// NOTE(ck): Lighting information
		//glUniform3fv(glGetUniformLocation(shaderID, "lightPos"), 1, &light->pos[0]);
		//glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, &camPos[0]);

		bool useTexture = (ArrayCount(data->textures) > 0);
		glUniform1i(glGetUniformLocation(shaderID, "useTexture"), useTexture);
		glUniform1i(glGetUniformLocation(shaderID, "pixelTexture"), useTexture);
		glUniform1i(glGetUniformLocation(shaderID, "useTexture"), true);
		glUniform1i(glGetUniformLocation(shaderID, "pixelTexture"), true);
		// bind textures on corresponding texture units
		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, data->textures[selectedTexture].id);

		glUniform3fv(glGetUniformLocation(shaderID, "colliderColor"), 1, &data->color[0]);
		glUniform1i(glGetUniformLocation(shaderID, "collider"), false);

		// TODO(ck): Does this happen at the end of update. the data gets its mat4 updated 
		// and then we can just call glUniformMatrix on this
		data->worldMatrix = mat4(1.0f);
		data->worldMatrix = glm::translate(data->worldMatrix, pos);
		if (data->indiceCount > 0)
		{
			data->worldMatrix = glm::rotate(data->worldMatrix, glm::radians(config->angleDegrees), v3{ 1.0f, 0.0f, 0.0f });
		}
		data->worldMatrix = glm::scale(data->worldMatrix, data->scale);
		
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(data->worldMatrix));

		glBindVertexArray(data->VAO);
		if (data->indiceCount > 0)
		{
			glDrawElements(GL_TRIANGLES, data->indiceCount, GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		globalDrawCalls++;
	}

	void DrawBoundingBox(RenderData* data, Camera* camera, unsigned int shaderID)
	{
		if (false == data->visible)
			return;

		
		// ==============================================================================
		glUniform3fv(glGetUniformLocation(shaderID, "material.ambient"), 1, &data->mat.ambient[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "material.diffuse"), 1, &data->mat.diffuse[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "material.specular"), 1, &data->mat.specular[0]);
		glUniform1f(glGetUniformLocation(shaderID, "material.shininess"), data->mat.shininess);

		glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, &camera->pos[0]);

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(Projection(camera)));
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(ViewMatrix(camera)));

		glUniform1i(glGetUniformLocation(shaderID, "useTexture"), false);

		glUniform3fv(glGetUniformLocation(shaderID, "colliderColor"), 1, &data->color[0]);
		glUniform1i(glGetUniformLocation(shaderID, "collider"), true);
		// ==============================================================================

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(data->worldMatrix));
		glBindVertexArray(data->VAO);

		glEnable(GL_LINE_SMOOTH);
		glLineWidth(data->lineWidth);
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
		glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));

		globalDrawCalls++;
	}

	void DrawWater(RenderData* data, RenderSetup* setup, WaterDataProgram* waterData, Light* light, v3 pos, v3 scale, v3 camPos, unsigned int shaderID)
	{

		// ==============================================================================
		//glUniform3fv(glGetUniformLocation(shaderID, "material.ambient"), 1, &data->mat.ambient[0]);
		//glUniform3fv(glGetUniformLocation(shaderID, "material.diffuse"), 1, &data->mat.diffuse[0]);
		//glUniform3fv(glGetUniformLocation(shaderID, "material.specular"), 1, &data->mat.specular[0]);
		//glUniform1f(glGetUniformLocation(shaderID, "material.shininess"), data->mat.shininess);

		// TODO(ck): Move to begin render. MonGL::beginRender(&cam);

		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;

		// Textures
		// ==================================================================================
		for (unsigned int i = 0; i < ArrayCount(data->textures); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i); // activate the proper texture unit before binding
		//	// retrieve texture number (the N in diffuse_TextureN)
			std::string number;
			std::string name = "texture_";
			switch (data->textures[i].type)
			{
			case TextureType::Diffuse:
				number = std::to_string(diffuseNr++);
				name += "diffuse";
				break;
			case TextureType::Specular:
				number = std::to_string(specularNr++);
				name += "specular";
				break;
			case TextureType::Normal:
				number = std::to_string(normalNr++);
				name += "normal";
				break;
			case TextureType::Height:
				number = std::to_string(heightNr++);
				name += "height";
			default:
				number = std::to_string(diffuseNr++);
				name += "diffuse";
				break;
			}

			// now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shaderID, (name + number).c_str()), i);
			// and finally bind the texture		
			glBindTexture(GL_TEXTURE_2D, data->textures[i].id);
		}

		// ==================================================================================

		//glUniform1f(GetLoc(waterShader, "time"), (float)glfwGetTime());
		glUniform3fv(glGetUniformLocation(shaderID, "lightPos"), 1, &light->pos[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, &camPos[0]);

		glUniform1f(glGetUniformLocation(shaderID, "uJump"), 0.25f);	// water->uJump
		glUniform1f(glGetUniformLocation(shaderID, "vJump"), 0.25f);	// water->vJump
		glUniform1f(glGetUniformLocation(shaderID, "tiling"), waterData->tiling);	// water->tiling
		glUniform1f(glGetUniformLocation(shaderID, "speed"), waterData->speed);	// water->speed 
		glUniform1f(glGetUniformLocation(shaderID, "flowStrength"), waterData->flowStrength); // water->flowStrength
		glUniform1f(glGetUniformLocation(shaderID, "flowOffset"), waterData->flowOffset);	// water->flowOffset
		glUniform1f(glGetUniformLocation(shaderID, "heightScale"), waterData->heightScale);	// water->heightScale
		glUniform1f(glGetUniformLocation(shaderID, "heightScaleModulated"), waterData->heightScaleModulated); // water->heightScaleModulated
		//glUniform1f(glGetUniformLocation(shaderID, "waveLength"), waterData->waveLength);	//  water->waveLength

		// ==============================================================================

		// TODO(ck): Does this happen at the end of update. the data gets its mat4 updated 
		// and then we can just call glUniformMatrix on this
		mat4 model = mat4(1.0f);
		model = glm::translate(model, pos);
		//model = glm::rotate(model, glm::radians(config->angleDegrees), v3{ 1.0f, 0.0f, 0.0f });
		//model = glm::scale(model, scale);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(data->VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Always good practice to set everything back to defaults once configured
		// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void DrawTerrain(unsigned int shaderID, RenderData* data, Light* light, Camera* camera, bool wireFrame)
	{
		glUseProgram(shaderID);

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(Projection(camera)));
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(ViewMatrix(camera)));

		glUniform3fv(glGetUniformLocation(shaderID, "light.pos"), 1, &light->pos[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "light.ambient"), 1, &light->ambient[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "light.diffuse"), 1, &light->diffuse[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "light.specular"), 1, &light->specular[0]);

		glUniform3fv(glGetUniformLocation(shaderID, "material.ambient"), 1, &data->mat.ambient[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "material.diffuse"), 1, &data->mat.diffuse[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "material.specular"), 1, &data->mat.specular[0]);
		glUniform1f(glGetUniformLocation(shaderID, "material.shininess"), data->mat.shininess);

		glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, &camera->pos[0]);


		mat4 matModel = mat4(1.0f);

		//mat4 matTranslate = glm::translate(mat4(1.0f),
		//										v3(terrain->x, 0.0f, terrain->z));
		mat4 matTranslate = glm::translate(mat4(1.0f),
												v3(0.0f, 0.0f, 0.0f));
		matModel = matModel * matTranslate;

		glUniform1i(glGetUniformLocation(shaderID, "useTexture"), true);
		glUniform1i(glGetUniformLocation(shaderID, "pixelTexture"), true);


		//glBindTexture(GL_TEXTURE_2D, terrain->selectedTextureId);
		glBindTexture(GL_TEXTURE_2D, data->textures[data->selectedTexture].id);

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(matModel));
		glBindVertexArray(data->VAO);

		wireFrame ?
			glDrawElements(GL_LINES, data->indiceCount, GL_UNSIGNED_INT, 0)
			: glDrawElements(GL_TRIANGLES, data->indiceCount, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		// Always good practice to set everything back to defaults once configured
		// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);


		globalDrawCalls++;
	}

	void EndRender()
	{
		assert(globalDrawCalls > 0);
	}

	// ==================================================================

	void ViewPort(Rect* port)
	{		
		glViewport(port->x, port->y, port->w, port->h);
		return;
	}

	//void cleanUp(Sprite* sprite)
	//{
	//	glDeleteVertexArrays(1, &sprite->VAO);
	//	glDeleteBuffers(1, &sprite->VBO);
	//	//glDeleteBuffers(1, &EBO);
	//	//glDeleteProgram(shaderProgram);
	//}

			// TODO(ck): clean up render data 
	//glDeleteVertexArrays(1, &player.data.VAO);
	//glDeleteBuffers(1, &player.data.VBO);

	//MonGL::DeleteShader(&shader);



	///
	/// 2D
	///
	 
	BatchData* batch;
	std::vector<Vertex> tileVertices;
	int usedIndices = 0;


	void InitRenderData2D(RenderData2D* sprite)
	{
		// TODO(ck): Switch to fill batch for this... can not rely on model matrix anymore if using a batch
		// need to update the positions and the texture coordinates manually each frame.
		int size = 32;

		int x = 0;
		int y = 0;

		Vertex vertices[4];
		vertices[0] = {};
		vertices[0].position = v3(x, y, 0.0f);
		vertices[0].color = v3(1.0f, 0.0f, 0.0f);
		vertices[0].texCoords = v2(0.0f, 0.0f);

		vertices[1] = {};
		vertices[1].position = v3(x + size, y, 0.0f);
		vertices[1].color = v3(1.0f, 0.0, 0.0f);
		vertices[1].texCoords = v2(1.0f, 0.0f);

		vertices[2] = {};
		vertices[2].position = v3(x + size, y + size, 0.0f);
		vertices[2].color = v3(1.0f, 1.0f, 1.0f);
		vertices[2].texCoords = v2(1.0f, 1.0f);

		vertices[3] = {};
		vertices[3].position = v3(x, y + size, 0.0f);
		vertices[3].color = v3(1.0f, 1.0f, 1.0f);
		vertices[3].texCoords = v2(0.0f, 1.0f);

		unsigned int indices[] = {
			0, 1, 3,
			1, 2, 3
		};

		unsigned int EBO;
		glGenVertexArrays(1, &sprite->VAO);
		glGenBuffers(1, &sprite->VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(sprite->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, sprite->VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), (GLvoid*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		sprite->color = v3(1.0f);
		sprite->pos = {};
	}

	
	void InitTileMap(int tileAmount)
	{
		// These will be figured out after looping our tilemap and pushing quads
		// TODO(ck): Need to be able to choose amount of vertices and indices
		int usedVertices = 1;
		const int quadCount = 2000;
		const int maxVertices = quadCount * 4;
		const int indicesLength = quadCount * 6;
		 
		// TODO(ck): MEMORY - TEST 
		batch = new BatchData();
		
		glGenVertexArrays(1, &batch->VAO);
		glBindVertexArray(batch->VAO);

		glGenBuffers(1, &batch->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, batch->VBO);
		glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		// position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		// color attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		// texture coord attribute
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		// TODO(ck): Memory management
		uint32_t* tileIndices = new uint32_t[indicesLength];
		int offset = 0;
		for (int i = 0; i < indicesLength; i += 6)
		{
			tileIndices[i + 0] = 0 + offset;
			tileIndices[i + 1] = 1 + offset;
			tileIndices[i + 2] = 2 + offset;
			
			tileIndices[i + 3] = 2 + offset;
			tileIndices[i + 4] = 3 + offset;
			tileIndices[i + 5] = 0 + offset;

			offset += 4;
		}

		unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(tileIndices), tileIndices, GL_DYNAMIC_DRAW);

		delete[] tileIndices;
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	};

	// This isn't draw this is fill
	void FillBatch(int tileOffsetX, int tileOffsetY, float tileXPos, float tileYPos, int tileSize)
	{		
		float sheetSize = 256.0f;
		int spriteSize = 16;

		// Texture coords
		float topRightX		= ((tileOffsetX + 1) * spriteSize) / sheetSize;
		float topRightY		= ((tileOffsetY + 1) * spriteSize) / sheetSize;
		float topLeftX		= (tileOffsetX * spriteSize) / sheetSize;
		float topLeftY		= ((tileOffsetY + 1) * spriteSize) / sheetSize;
		float bottomLeftX	= (tileOffsetX * spriteSize) / sheetSize;
		float bottomLeftY  = (tileOffsetY * spriteSize) / sheetSize;
		float bottomRightX	= ((tileOffsetX + 1) * spriteSize) / sheetSize;
		float bottomRightY	= (tileOffsetY * spriteSize) / sheetSize;

		// TODO(ck): pushQuad(pos, color, texcoords)		
		float x = tileXPos;
		float y = tileYPos;		
		int size = tileSize;
		Vertex vec0 = {
			v3(x, y, 0.0f),
			v3(1.0f, 0.0f, 0.0f),
			v2(bottomLeftX, bottomLeftY)	
		};

		Vertex vec1 = {
			v3(x + size, y, 0.0f),
			v3(0.0f, 1.0f, 0.0f),
			v2(bottomRightX, bottomRightY)
		};

		Vertex vec2 = {
			v3(x + size, y + size, 0.0f),
			v3(0.0f, 0.0f, 1.0f),
			v2(topRightX, topRightY)
		};

		Vertex vec3 = {
			v3(x, y + size, 0.0f),
			v3(1.0f, 1.0f, 0.0f),
			v2(topLeftX, topLeftY)
		};

		usedIndices += 6;
		tileVertices.push_back(vec0);
		tileVertices.push_back(vec1);
		tileVertices.push_back(vec2);
		tileVertices.push_back(vec3);
	}

	void BindVertices()
	{
		// 2 extra vertices are needed for degenerate triangles between each strip 
		//unsigned uNumExtraVertices = ( GL_TRIANGLE_STRIP == _config.uRenderType && _uNumUsedVertices > 0 ? 2 : 0 ); 

		glBindVertexArray(batch->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, batch->VBO);
		//if (uNumExtraVertices > 0)
		//{
		//	//need to add 2 vertex copies to create degenerate triangles between this strip 
		//	//and the last strip that was stored in the batch 
		//	glBufferSubData(GL_ARRAY_BUFFER, _uNumUsedVertices * sizeof(GuiVertex), sizeof(GuiVertex), &_lastVertex);
		//	glBufferSubData(GL_ARRAY_BUFFER, (_uNumUsedVertices + 1) * sizeof(GuiVertex), sizeof(GuiVertex), &vVertices[0]);
		//}

		// Use glMapBuffer instead, if moving large chunks of data > 1MB 
		//int _uNumUsedVertices = 400;
		//int uNumExtraVertices = 2;
		//glBufferSubData(GL_ARRAY_BUFFER, (_uNumUsedVertices + uNumExtraVertices) * sizeof(Vertex), newTileVertices.size() * sizeof(Vertex), &newTileVertices[0]);
		
		// IMPORTANT(ck):
		// STUDY(ck): The second param (offset) in this was set to verticesLength * sizeof(Vertex). This was causing the vertices the show up as stretched 
		// and elongated triangles
		
		//glBufferSubData(GL_ARRAY_BUFFER, 0, tileVertices.size() * sizeof(Vertex), &tileVertices[0]);
		glBufferData(GL_ARRAY_BUFFER, tileVertices.size() * sizeof(Vertex), &tileVertices[0], GL_DYNAMIC_DRAW);
		/*
			// Upload Buffer
			gl.BindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
			gl.BufferData(GL_ARRAY_BUFFER, m_vertex_size * count, vertices, GL_DYNAMIC_DRAW);
		
		*/


		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//_uNumUsedVertices += vVertices.size() + uNumExtraVertices;
		//_lastVertex = vVertices[vVertices.size() - 1];
	}

	void DrawMap(CommonProgram* shader, unsigned int textureID)
	{
		// TODO(ck): If we want the ability to change the map at runtime we need to constantly
		// be filling and binding the batch (if things have changed)
		// Fill batch 
		// bind vertices


		//glUseProgram(shader->id);

		//glActiveTexture(GL_TEXTURE0);
		// IMPORTANT(ck):
		// NOTE(ck): the reason why you set it to 0 is because thats the base texture slot
		// its not expecting the textureID thats only for binding
		glUniform1i(glGetUniformLocation(shader->handle, "image"), 0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glBindVertexArray(batch->VAO);
		// (void*)(index_size * pass.index_start)
		glDrawElements(GL_TRIANGLES, usedIndices, GL_UNSIGNED_INT, (void*)(2 * 0));
		glBindVertexArray(0);

		//reset buffer
		//_uNumUsedVertices = 0;
		//_config.iPriority = 0;
		usedIndices = 0;
		tileVertices.clear();
	}

	void DrawObject(CommonProgram* shader, RenderData2D* data)
	{
		//glUseProgram(shader->id);

		// TODO(ck): We don't calculate the matrix here
		// we calc it in the game and send the matrix to the
		// renderer 
		mat4 model = mat4(1.0f);
		v3 tilePosition = {};
		tilePosition.x = data->pos.x;
		tilePosition.y = data->pos.y;
		model = glm::translate(model, tilePosition);

		//model = glm::translate(model, v3(0.5f * obj->size.x, 0.0f * obj->size.y, 0.0f));
		//model = glm::rotate(model, obj->rotation, v3(0.0f, 0.0f, 1.0f));
		//model = glm::translate(model, v3(-0.5f * obj->size.x, -0.5f * obj->size.y, 0.0f));

		//model = glm::scale(model, v3(data->size, 1.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader->handle, "model"), 1, GL_FALSE, glm::value_ptr(model));
		
		//glUniform3f(glGetUniformLocation(shader->handle, "spriteColor"), data->color.r, data->color.g, data->color.b);

		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, data->texture.id);

		glBindVertexArray(data->VAO);

		//if (data->wireFrame)
		//	glDrawArrays(GL_LINES, 0, 6);
		//else 
		//	glDrawArrays(GL_TRIANGLES, 0, 6);


		//glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, (void*)(2 * 0));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


	}



	void DrawMap(CommonProgram* shader, unsigned int textureID, int batchThing)
	{
		//
		// Fill batch 
		//
		
		
		//
		// bind vertices
		// 
		BindVertices();

		// 2 extra vertices are needed for degenerate triangles between each strip 
		//unsigned uNumExtraVertices = ( GL_TRIANGLE_STRIP == _config.uRenderType && _uNumUsedVertices > 0 ? 2 : 0 ); 

		glBindVertexArray(batch->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, batch->VBO);
		//if (uNumExtraVertices > 0)
		//{
		//	//need to add 2 vertex copies to create degenerate triangles between this strip 
		//	//and the last strip that was stored in the batch 
		//	glBufferSubData(GL_ARRAY_BUFFER, _uNumUsedVertices * sizeof(GuiVertex), sizeof(GuiVertex), &_lastVertex);
		//	glBufferSubData(GL_ARRAY_BUFFER, (_uNumUsedVertices + 1) * sizeof(GuiVertex), sizeof(GuiVertex), &vVertices[0]);
		//}

		// Use glMapBuffer instead, if moving large chunks of data > 1MB 
		//int _uNumUsedVertices = 400;
		//int uNumExtraVertices = 2;
		//glBufferSubData(GL_ARRAY_BUFFER, (_uNumUsedVertices + uNumExtraVertices) * sizeof(Vertex), newTileVertices.size() * sizeof(Vertex), &newTileVertices[0]);

		// IMPORTANT(ck):
		// STUDY(ck): The second param (offset) in this was set to verticesLength * sizeof(Vertex). This was causing the vertices the show up as stretched 
		// and elongated triangles

		//glBufferSubData(GL_ARRAY_BUFFER, 0, tileVertices.size() * sizeof(Vertex), &tileVertices[0]);
		glBufferData(GL_ARRAY_BUFFER, tileVertices.size() * sizeof(Vertex), &tileVertices[0], GL_DYNAMIC_DRAW);
		/*
			// Upload Buffer
			gl.BindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
			gl.BufferData(GL_ARRAY_BUFFER, m_vertex_size * count, vertices, GL_DYNAMIC_DRAW);

		*/


		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//_uNumUsedVertices += vVertices.size() + uNumExtraVertices;
		//_lastVertex = vVertices[vVertices.size() - 1];



		//
		// Render batch
		//

		//glUseProgram(shader->id);

		//glActiveTexture(GL_TEXTURE0);
		// IMPORTANT(ck):
		// NOTE(ck): the reason why you set it to 0 is because thats the base texture slot
		// its not expecting the textureID thats only for binding
		glUniform1i(glGetUniformLocation(shader->handle, "image"), 0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glBindVertexArray(batch->VAO);
		// (void*)(index_size * pass.index_start)
		glDrawElements(GL_TRIANGLES, usedIndices, GL_UNSIGNED_INT, (void*)(2 * 0));
		glBindVertexArray(0);

		//reset buffer
		//_uNumUsedVertices = 0;
		//_config.iPriority = 0;




		// clear batch
	}

}