#include "mon_gl_render.h"

#include <glad/glad.h>
#include <glm/gtx/transform.hpp>

#include <vector>

namespace MonGL
{

	glm::vec3 GetSize(ColliderSize* size)
	{
		return glm::vec3(size->max.x - size->min.x, size->max.y - size->min.y, size->max.z - size->min.z);
	}

	glm::vec3 GetCenter(ColliderSize* size)
	{
		return glm::vec3((size->min.x + size->max.x) / 2, (size->min.y + size->max.y) / 2, (size->min.z + size->max.z) / 2);
	}

	glm::mat4 GetTransform(ColliderSize* size)
	{
		return glm::translate(glm::mat4(1), GetCenter(size)) * glm::scale(glm::mat4(1), GetSize(size));
	}

	void initCharacter(RenderData* data, int shaderID, int TESTCHOOSE)
	{
		data->vertices.push_back({ glm::vec3(0.5, 0.5, 0.0), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f) });
		data->vertices.push_back({ glm::vec3(0.5, -0.5, 0.0), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f) });
		data->vertices.push_back({ glm::vec3(-0.5,-0.5, 0.0), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) });
		data->vertices.push_back({ glm::vec3(-0.5, 0.5, 0.0), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f) });

		unsigned int indices[] = {
			0, 1, 3,
			1, 2, 3
		};

		unsigned int EBO;
		glGenVertexArrays(1, &data->VAO);
		glGenBuffers(1, &data->VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(data->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
		glBufferData(GL_ARRAY_BUFFER, data->vertices.size() * sizeof(Vertex3D), &data->vertices[0], GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);


		if (TESTCHOOSE == 1)
		{
			std::string textPath = "res/textures/p1.png";
			std::string textDir = textPath.substr(0, textPath.find_last_of('/'));
			data->texture = {};
			MonTexture::LoadTextureFile(&data->texture, textPath.c_str(), false, true, true);

			glUniform1i(glGetUniformLocation(shaderID, "texture_diffuse1"), 0);
		}
		if (TESTCHOOSE == 2)
		{
			std::string textPath = "res/textures/tree.png";
			std::string textDir = textPath.substr(0, textPath.find_last_of('/'));
			data->texture = {};
			MonTexture::LoadTextureFile(&data->texture, textPath.c_str(), false, true, true);

			glUniform1i(glGetUniformLocation(shaderID, "texture_diffuse1"), 0);
		}

	}

	void initBoundingBox(RenderData* data)
	{
		data->lineWidth = 2;
		data->color = glm::vec3(0.2, 0.7, 0.4);

		data->vertices.push_back({ glm::vec3(-0.5, -0.5, -0.5), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) });
		data->vertices.push_back({ glm::vec3(0.5, -0.5, -0.5), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) });
		data->vertices.push_back({ glm::vec3(0.5, 0.5, -0.5), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) });
		data->vertices.push_back({ glm::vec3(-0.5, 0.5, -0.5), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) });
		
		data->vertices.push_back({ glm::vec3(-0.5, -0.5, 0.5), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) });
		data->vertices.push_back({ glm::vec3(0.5, -0.5, 0.5), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) });
		data->vertices.push_back({ glm::vec3(0.5, 0.5, 0.5), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) });
		data->vertices.push_back({ glm::vec3(-0.5, 0.5, 0.5), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) });

		glGenVertexArrays(1, &data->VAO);
		glGenBuffers(1, &data->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
#if 0
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
#endif
		glBufferData(GL_ARRAY_BUFFER, data->vertices.size() * sizeof(Vertex3D), &data->vertices[0], GL_STATIC_DRAW);
		glBindVertexArray(data->VAO);
		
		GLushort elements[] = {
			0, 1, 2, 3, // front 
			4, 5, 6, 7, // back
			0, 4, 1, 5, 2, 6, 3, 7 // back
		};
		data->elementLength = sizeof(elements) / sizeof(elements[0]);

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

		data->size = {};
		data->size.min = glm::vec3(0.0f, 0.0f, 0.0f);
		data->size.max = glm::vec3(2.0f, 2.0f, 2.0f);
		// Set world matrix to be the same size as the bounding box
		//data->worldMatrix = GetTransform(&data->size);

		// TODO(ck): Add back for cubes
		// Normals
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		//glEnableVertexAttribArray(1);

		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		//glEnableVertexAttribArray(2);
	}

	void drawCharacter(RenderData* data,
						 glm::vec3 playerPos, glm::vec3 scale, glm::vec3 camPos,
						 glm::mat4 projection, glm::mat4 view,
						 unsigned int shaderID)
	{

		// ==============================================================================
		//glUniform3fv(glGetUniformLocation(shaderID, "material.ambient"), 1, &data->mat.ambient[0]);
		//glUniform3fv(glGetUniformLocation(shaderID, "material.diffuse"), 1, &data->mat.diffuse[0]);
		//glUniform3fv(glGetUniformLocation(shaderID, "material.specular"), 1, &data->mat.specular[0]);
		//glUniform1f(glGetUniformLocation(shaderID, "material.shininess"), data->mat.shininess);

		glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, &camPos[0]);

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glUniform1i(glGetUniformLocation(shaderID, "useTexture"), true);
		glUniform1i(glGetUniformLocation(shaderID, "pixelTexture"), true);
		// bind textures on corresponding texture units
		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, data->texture.id);

		glUniform3fv(glGetUniformLocation(shaderID, "colliderColor"), 1, &data->color[0]);
		glUniform1i(glGetUniformLocation(shaderID, "collider"), true);
		// ==============================================================================

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, playerPos);
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, scale);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(data->VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}


	// TODO(ck): gl_DrawBoundingBox(size) 
	// the collider will have a size
	void drawBoundingBox(RenderData* data, 
					 glm::vec3 playerPos, glm::vec3 camPos,
					 glm::mat4 projection, glm::mat4 view, 
					 unsigned int shaderID)
	{

		// ==============================================================================
		glUniform3fv(glGetUniformLocation(shaderID, "material.ambient"), 1, &data->mat.ambient[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "material.diffuse"), 1, &data->mat.diffuse[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "material.specular"), 1, &data->mat.specular[0]);
		glUniform1f(glGetUniformLocation(shaderID, "material.shininess"), data->mat.shininess);

		glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, &camPos[0]);

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glUniform1i(glGetUniformLocation(shaderID, "useTexture"), false);

		glUniform3fv(glGetUniformLocation(shaderID, "colliderColor"), 1, &data->color[0]);
		glUniform1i(glGetUniformLocation(shaderID, "collider"), true);
		// ==============================================================================

		glm::mat4 model = data->worldMatrix * GetTransform(&data->size);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(data->VAO);

		glEnable(GL_LINE_SMOOTH);
		glLineWidth(data->lineWidth);
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
		glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
	}


	void generateTerrain(RenderData* data)
	{
		data->VAO = 0;
		data->VBO = 0;

		const int SIZE = 64;
		const int VERTEX_COUNT = 16;
		
		data->vertices.resize(VERTEX_COUNT * VERTEX_COUNT);

		// TODO(ck): Memory Allocation
		int* indices = new int[6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1)];
		int verticesLength = VERTEX_COUNT * VERTEX_COUNT;
		data->elementLength = 6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1);

		int index = 0;
		for (int i = 0; i < VERTEX_COUNT; ++i)
		{
			for (int j = 0; j < VERTEX_COUNT; ++j)
			{
				data->vertices[index].position.x = (float)j / ((float)VERTEX_COUNT - 1) * SIZE;

				data->vertices[index].position.y = -0.3f;

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
		glBufferData(GL_ARRAY_BUFFER, data->vertices.size() *sizeof(Vertex3D), &data->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->elementLength * sizeof(int), indices, GL_STATIC_DRAW);
		
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
		delete[] indices;

		data->mat = {};
		data->mat.ambient = glm::vec3(1.0f, 0.5f, 0.6f);
		data->mat.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
		data->mat.specular = glm::vec3(0.5f, 0.5f, 0.5f);
		data->mat.shininess = 32.0f;

		//std::string textPath = "res/textures/terrain/1024multi.png";
		std::string textPath = "res/textures/terrain/pix_grass.png";
		std::string textDir = textPath.substr(0, textPath.find_last_of('/'));
		/*
		textureIds[0] = MonTexture::LoadTextureFile("1024multi.png", textDir, false);
		std::string filename = std::string(path);
		filename = directory + '/' + filename;
		*/
		data->texture = {};
		MonTexture::LoadTextureFile(&data->texture, textPath.c_str(), false, true);


	}

	void drawTerrain(unsigned int shaderID, RenderData* data, Light* light, glm::mat4 projection, glm::mat4 view,
						glm::vec3 camPos)
	{
		glUseProgram(shaderID);

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glUniform3fv(glGetUniformLocation(shaderID, "light.pos"), 1, &light->pos[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "light.ambient"), 1, &light->ambient[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "light.diffuse"), 1, &light->diffuse[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "light.specular"), 1, &light->specular[0]);

		glUniform3fv(glGetUniformLocation(shaderID, "material.ambient"), 1, &data->mat.ambient[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "material.diffuse"), 1, &data->mat.diffuse[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "material.specular"), 1, &data->mat.specular[0]);
		glUniform1f(glGetUniformLocation(shaderID, "material.shininess"), data->mat.shininess);

		glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, &camPos[0]);


		glm::mat4 matModel = glm::mat4(1.0f);

		//glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
		//										glm::vec3(terrain->x, 0.0f, terrain->z));
		glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
												glm::vec3(0.0f, 0.0f, 0.0f));
		matModel = matModel * matTranslate;

		glUniform1i(glGetUniformLocation(shaderID, "useTexture"), true);

		//glBindTexture(GL_TEXTURE_2D, terrain->selectedTextureId);
		glBindTexture(GL_TEXTURE_2D, data->texture.id);

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(matModel));
		glBindVertexArray(data->VAO);

		//terrain->wireFrame ?
		//	glDrawElements(GL_LINES, terrain->indicesLength, GL_UNSIGNED_INT, 0)
		//	: glDrawElements(GL_TRIANGLES, terrain->indicesLength, GL_UNSIGNED_INT, 0);
		glDrawElements(GL_TRIANGLES, data->elementLength, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		// Always good practice to set everything back to defaults once configured
		// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

	}

	// ==================================================================



	///
	/// 2D
	///
	 
	BatchData* batch;
	std::vector<Vertex> tileVertices;
	int usedIndices = 0;

	void initRenderData(Sprite* sprite)
	{

#if 1
		GLfloat vertices[] =
		{
			// Pos          Tex
			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,

			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f
		};
#endif
		glGenVertexArrays(1, &sprite->VAO);
		glGenBuffers(1, &sprite->VBO);

		glBindBuffer(GL_ARRAY_BUFFER, sprite->VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindVertexArray(sprite->VAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}
	
	void initTileMap(int tileAmount)
	{
		// These will be figured out after looping our tilemap and pushing quads
		// TODO(ck): Need to be able to choose amount of vertices and indices
		int usedVertices = 1;
		const int quadCount = 1000;
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
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(tileIndices), tileIndices, GL_STATIC_DRAW);

		delete[] tileIndices;
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	};

	// This isn't draw this is fill
	void fillBatch(int tileOffsetX, int tileOffsetY, float tileXPos, float tileYPos)
	{		
		float sheetSize = 256.0f;
		int spriteSize = 32;

		// Texture coords
		float topRightX		= ((tileOffsetX + 1) * spriteSize) / sheetSize;
		float topRightY		= ((tileOffsetY + 1) * spriteSize) / sheetSize;
		float topLeftX		= (tileOffsetX * spriteSize) / sheetSize;
		float topLeftY		= ((tileOffsetY + 1) * spriteSize) / sheetSize;
		float bottomRightX	= (tileOffsetX * spriteSize) / sheetSize;
		float bottomRightY  = (tileOffsetY * spriteSize) / sheetSize;
		float bottomLeftX	= ((tileOffsetX + 1) * spriteSize) / sheetSize;
		float bottomLeftY	= (tileOffsetY * spriteSize) / sheetSize;

		// TODO(ck): pushQuad(pos, color, texcoords)		
		float x = tileXPos;
		float y = tileYPos;		
		int size = 32;
		Vertex v0 = {
			glm::vec3(x, y, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec2(bottomRightX, bottomRightY)	
		};

		Vertex v1 = {
			glm::vec3(x + size, y, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec2(bottomLeftX, bottomLeftY)
		};

		Vertex v2 = {
			glm::vec3(x + size, y + size, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec2(topRightX, topRightY)
		};

		Vertex v3 = {
			glm::vec3(x, y + size, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec2(topLeftX, topLeftY)
		};

		usedIndices += 6;
		tileVertices.push_back(v0);
		tileVertices.push_back(v1);
		tileVertices.push_back(v2);
		tileVertices.push_back(v3);
	}

	void bindVertices()
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
		glBufferSubData(GL_ARRAY_BUFFER, 0, tileVertices.size() * sizeof(Vertex), &tileVertices[0]);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//_uNumUsedVertices += vVertices.size() + uNumExtraVertices;
		//_lastVertex = vVertices[vVertices.size() - 1];
	}

	void drawMap(MonShader::Shader* shader, unsigned int textureID)
	{
		glUseProgram(shader->id);

		//glActiveTexture(GL_TEXTURE0);
		// IMPORTANT(ck):
		// NOTE(ck): the reason why you set it to 0 is because thats the base texture slot
		// its not expecting the textureID thats only for binding
		glUniform1i(glGetUniformLocation(shader->id, "image"), 0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glBindVertexArray(batch->VAO);
		glDrawElements(GL_TRIANGLES, usedIndices, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		//reset buffer
		//_uNumUsedVertices = 0;
		//_config.iPriority = 0;
	}

	void drawObject(MonShader::Shader* shader, RenderData2D* data)
	{
		glUseProgram(shader->id);

		// TODO(ck): We don't calculate the matrix here
		// we calc it in the game and send the matrix to the
		// renderer 
		// CONVERT World matrix to drawing position
		glm::mat4 model = glm::mat4(1.0f);
		model * -data->pos;

		//glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//glUniform3f(glGetUniformLocation(shader->id, "spriteColor"), obj->color.r, obj->color.g, obj->color.b);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, obj->sprite.texture.id);

		glBindVertexArray(data->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	void drawObject(MonShader::Shader* shader, Entity* obj)
	{
		glUseProgram(shader->id);

		// TODO(ck): We don't calculate the matrix here
		// we calc it in the game and send the matrix to the
		// renderer 
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(obj->pos, 0.0f));

		model = glm::translate(model, glm::vec3(0.5f * obj->size.x, 0.0f * obj->size.y, 0.0f));
		model = glm::rotate(model, obj->rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.5f * obj->size.x, -0.5f * obj->size.y, 0.0f));

		model = glm::scale(model, glm::vec3(obj->size, 1.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(glGetUniformLocation(shader->id, "spriteColor"), obj->color.r, obj->color.g, obj->color.b);

		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, obj->sprite.texture.id);

		glBindVertexArray(obj->sprite.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	void cleanUp(Sprite* sprite)
	{
		glDeleteVertexArrays(1, &sprite->VAO);
		glDeleteBuffers(1, &sprite->VBO);
		//glDeleteBuffers(1, &EBO);
		//glDeleteProgram(shaderProgram);
	}
}