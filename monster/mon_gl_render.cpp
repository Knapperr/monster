#include "mon_gl_render.h"

#include <glad/glad.h>

#include <vector>
#include <stack>

namespace MonGL
{

	glm::vec3 GetSize(ColliderSize* size)
	{
		return glm::vec3(size->maxX() - size->minX(), size->maxY() - size->minY(), size->maxZ() - size->minZ());
	}

	glm::vec3 GetCenter(ColliderSize* size)
	{
		return glm::vec3((size->minX() + size->maxX()) / 2, (size->minY() + size->maxY()) / 2, (size->minZ() + size->maxZ()) / 2);
	}

	glm::mat4 GetTransform(ColliderSize* size)
	{
		return glm::translate(glm::mat4(1), GetCenter(size)) * glm::scale(glm::mat4(1), GetSize(size));
	}

	// TODO(ck): gl_InitBoundingBox
	void gl_InitBoundingBox(RenderData* data)
	{
		data->lineWidth = 2;
		data->color = glm::vec3(0.2, 0.7, 0.4);

		float vertices[] = {
			-0.5, -0.5, -0.5, 1.0,
			 0.5, -0.5, -0.5, 1.0,
			 0.5,  0.5, -0.5, 1.0,
			-0.5,  0.5, -0.5, 1.0,
			-0.5, -0.5,  0.5, 1.0,
			 0.5, -0.5,  0.5, 1.0,
			 0.5,  0.5,  0.5, 1.0,
			-0.5,  0.5,  0.5, 1.0,
		};

		glGenVertexArrays(1, &data->VAO);
		glGenBuffers(1, &data->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		data->size = {};
		data->size.X(0.0f, 2.0f);
		data->size.Y(0.0f, 2.0f);
		data->size.Z(0.0f, 2.0f);

		// Set world matrix to be the same size as the bounding box
		//data->worldMatrix = GetTransform(&data->size);

		// TODO(ck): Add back for cubes
		// Normals
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		//glEnableVertexAttribArray(1);

		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		//glEnableVertexAttribArray(2);
	}

	// TODO(ck): gl_DrawBoundingBox(size) 
	// the collider will have a size
	void gl_DrawBoundingBox(RenderData* data, 
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

	// ======================




	/// 
	/// 2D
	/// 
	 
	BatchData* batch;
	//float* tileVertices;
	std::vector<float*> tileVertices;
	std::vector<Vertex> newTileVertices;

	// TODO(ck): I don't need this yet a matrix stack allows me to 
	// create the position of each for my vertices. Not that necessary for
	// static tiles
	std::stack<glm::vec3> matrixStack;
	int* tileIndices;

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


		// For this 
	}
	
	void initTileMap(int tileAmount)
	{
		/*
		* 
		* IMPORTANT(ck): I dont think we fill it here we just init
		* 
			Need to get all of the vertices and indices of every tile
			i guess I pass in the tiles or can i get tileAmount and Rect TileSize
			then if its 32,32 i just loop through and do that many 
			or just say it doesnt even need that i just need the vertices each tile
			takes up it doesnt matter what size it is
		


			for ()
			{
				PushQuad()
				fills array of tileVertices

				
			}
		*/

	


		// These will be figured out after looping our tilemap and pushing quads
		const int maxVertices = 10000;
		
		//tileVertices = new float[maxVertices];
		int usedVertices = 1;
		int indicesLength = 8000;
		 
		// TEST
		batch = new BatchData();
		tileIndices = new int[indicesLength];


		glGenVertexArrays(1, &batch->VAO);
		glBindVertexArray(batch->VAO);

		// TODO(ck): Consider GL_DYNAMIC_DRAW OVER GL_STREAM_DRAW
		glGenBuffers(1, &batch->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, batch->VBO);
		glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex), nullptr, GL_STREAM_DRAW);

		unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(int), tileIndices, GL_STREAM_DRAW);


		// these sizes wont work
		// position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// color attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		// texture coord attribute
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	};

	// This isn't draw this is fill
	void fillBatch(int tileIndex, int tileOffsetX, int tileOffsetY, float tileXPos, float tileYPos)
	{
		// TODO(ck): Vertices amount
		assert(tileIndex < 8000);
		
		float sheetWidth = 256.0f;
		float sheetHeight = 256.0f;
		int spriteWidth = 32;
		int spriteHeight = 32;

		float topRightX = (tileOffsetX * spriteWidth) / sheetWidth;
		float topRightY = (tileOffsetY * spriteHeight) / sheetHeight;
		float topLeftX = (tileOffsetX * spriteWidth) / sheetWidth;
		float topLeftY = ((tileOffsetY + 1) * spriteHeight) / sheetHeight;
		float bottomRightX = ((tileOffsetX + 1) * spriteWidth) / sheetWidth;
		float bottomRightY = (tileOffsetY * spriteHeight) / sheetHeight;
		float bottomLeftX = ((tileOffsetX + 1) * spriteWidth) / sheetWidth;
		float bottomLeftY = ((tileOffsetY + 1) * spriteHeight) / sheetHeight;

		//float textureCoords[] = {
		//	 bottomLeftX, bottomLeftY, // top right
		//	 bottomRightX, bottomRightY, // bottom right
		//	 topRightX, topRightY, // bottom left
		//	 topLeftX, topLeftY // top left 
		//};

		float vertices[] = {
			// positions          // colors           // texture coords
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   bottomLeftX, bottomLeftY, // top right
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   bottomRightX, bottomRightY, // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   topRightX, topRightY, // bottom left
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   topLeftX, topLeftY // top left 
		};


		// TODO(ck): pushQuad(pos, color, texcoords)
#if 1
		float x = -1.0f;
		float y = -0.5f;
		x = tileXPos;
		y = tileYPos;
#else

		int screenX = 200;
		int screenY = 200;
		int width = 960;
		int height = 540;
		float x = (((2.0 * screenX) - (2.0 * tileXPos)) / width) - 1.0;
		float y = (((2.0 * screenY) - (2.0 * tileYPos)) / height) - 1.0;
#endif
		float size = 32.0f;
		printf("tile index: %d - x:%f , y:%f\n", tileIndex, x, y);

		/*
		Mat3x2(1, 0, 0, 1, x, y);
		m11 = 1
		m12 = 0
		m21 = 0
		m22 = 1
		m31 = x
		m32 = y

		PUSH_QUAD(
			pos.x + sub.draw_coords[0].x, pos.y + sub.draw_coords[0].y,
			pos.x + sub.draw_coords[1].x, pos.y + sub.draw_coords[1].y,
			pos.x + sub.draw_coords[2].x, pos.y + sub.draw_coords[2].y,
			pos.x + sub.draw_coords[3].x, pos.y + sub.draw_coords[3].y,
			sub.tex_coords[0].x, sub.tex_coords[0].y,
			sub.tex_coords[1].x, sub.tex_coords[1].y,
			sub.tex_coords[2].x, sub.tex_coords[2].y,
			sub.tex_coords[3].x, sub.tex_coords[3].y,
			color, color, color, color,
			m_tex_mult, m_tex_wash, 0
		);

		#define PUSH_QUAD(px0, py0, px1, py1, px2, py2, px3, py3, tx0, ty0, tx1, ty1, tx2, ty2, tx3, ty3, col0, col1, col2, col3, mult, fill, wash) \
		{ \
			m_batch.elements += 2; \
			auto _i = m_indices.expand(6); \
			*_i++ = (u32)m_vertices.size() + 0; \
			*_i++ = (u32)m_vertices.size() + 1; \
			*_i++ = (u32)m_vertices.size() + 2; \
			*_i++ = (u32)m_vertices.size() + 0; \
			*_i++ = (u32)m_vertices.size() + 2; \
			*_i++ = (u32)m_vertices.size() + 3; \
			Vertex* _v = m_vertices.expand(4); \
			MAKE_VERTEX(_v, m_matrix, px0, py0, tx0, ty0, col0, mult, fill, wash); _v++; \
			MAKE_VERTEX(_v, m_matrix, px1, py1, tx1, ty1, col1, mult, fill, wash); _v++; \
			MAKE_VERTEX(_v, m_matrix, px2, py2, tx2, ty2, col2, mult, fill, wash); _v++; \
			MAKE_VERTEX(_v, m_matrix, px3, py3, tx3, ty3, col3, mult, fill, wash); \
		}

		#define MAKE_VERTEX(vert, mat, px, py, tx, ty, c, m, w, f) \
			(vert)->pos.x = ((px) * mat.m11) + ((py) * mat.m21) + mat.m31; \
			(vert)->pos.y = ((px) * mat.m12) + ((py) * mat.m22) + mat.m32; \
			(vert)->tex.x = tx;  \
			if (m_batch.flip_vertically) \
				(vert)->tex.y = 1.0f - ty; \
			else \
				(vert)->tex.y = ty; \
			(vert)->col = c; \
			(vert)->mult = m; \
			(vert)->wash = w; \
			(vert)->fill = f;
		*/

		Vertex v0 = {
			glm::vec3(x, y, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec2(bottomLeftX, bottomLeftY)
		};

		Vertex v1 = {
			glm::vec3(x + size, y, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec2(bottomRightX, bottomRightY)
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

#if 0

		if (tileIndex == 12)
		{
			//glm::mat4 model = glm::mat4(1.0f);
			//vert.position  += glm::vec3(tilePos.x, tilePos.y, 0.0f);
			//vert2.position += glm::vec3(tilePos.x, tilePos.y, 0.0f);
			//vert3.position += glm::vec3(tilePos.x, tilePos.y, 0.0f);
			//vert4.position += glm::vec3(tilePos.x, tilePos.y, 0.0f);
			
			//x = 50.5f;
			//y = -0.5f;
			//int screenX = 50;
			//int screenY = 50;
			//int width = 960;
			//int height = 540;
			screenX = 600;
			screenY = 600;
			x = (((2.0 * screenX) - (2.0 * tileXPos)) / width) - 1.0;
			y = (((2.0 * screenY) - (2.0 * tileYPos)) / height) - 1.0;
			v0 = {
				glm::vec3(x, y, 0.0f),
				glm::vec3(1.0f, 0.0f, 0.0f),
				glm::vec2(bottomLeftX, bottomLeftY)
			};

			v1 = {
				glm::vec3(x + size, y, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec2(bottomRightX, bottomRightY)
			};

			v2 = {
				glm::vec3(x + size, y + size, 0.0f),
				glm::vec3(0.0f, 0.0f, 1.0f),
				glm::vec2(topRightX, topRightY)
			};

			v3 = {
				glm::vec3(x, y + size, 0.0f),
				glm::vec3(1.0f, 1.0f, 0.0f),
				glm::vec2(topLeftX, topLeftY)
			};
		}
#endif

		newTileVertices.push_back(v0);
		newTileVertices.push_back(v1);
		newTileVertices.push_back(v2);
		newTileVertices.push_back(v3);


		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};

		tileVertices.push_back(vertices);

		//tileVertices[tileIndex] = *vertices;
		tileIndices[tileIndex] = *indices;

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
		int _uNumUsedVertices = 400;
		int uNumExtraVertices = 2;
		//glBufferSubData(GL_ARRAY_BUFFER, (_uNumUsedVertices + uNumExtraVertices) * sizeof(Vertex), newTileVertices.size() * sizeof(Vertex), &newTileVertices[0]);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex), newTileVertices.size() * sizeof(Vertex), &newTileVertices[0]);

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
		// GL_TRIANGLES should be render type
		// 400 = num used vertices
		glDrawArrays(GL_TRIANGLES, 0, newTileVertices.size());
		glBindVertexArray(0);

		//reset buffer
		//_uNumUsedVertices = 0;
		//_config.iPriority = 0;
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