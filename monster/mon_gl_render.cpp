#include "mon_gl_render.h"

#include <glad/glad.h>

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
	
	void initTile(Sprite* sprite, int x, int y)
	{
		float sheetWidth = 256.0f;
		float sheetHeight = 256.0f;
		int spriteWidth = 32;
		int spriteHeight = 32;

		float topRightX = (x * spriteWidth) / sheetWidth;
		float topRightY = (y * spriteHeight) / sheetHeight;
		float topLeftX = (x * spriteWidth) / sheetWidth;
		float topLeftY = ((y + 1) * spriteHeight) / sheetHeight;
		float bottomRightX = ((x + 1) * spriteWidth) / sheetWidth;
		float bottomRightY = (y * spriteHeight) / sheetHeight;
		float bottomLeftX = ((x + 1) * spriteWidth) / sheetWidth;
		float bottomLeftY = ((y + 1) * spriteHeight) / sheetHeight;

		float vertices[] = {
			// positions          // colors           // texture coords
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   bottomLeftX, bottomLeftY, // top right
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   bottomRightX, bottomRightY, // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   topRightX, topRightY, // bottom left
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   topLeftX, topLeftY // top left 
		};

		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
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

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
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
		int maxVertices = 100000;
		int usedVertices = 1;
		int indicesLength = 1;
		 
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
		unsigned int offset = 0;
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		offset += sizeof(glm::vec2);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		glEnableVertexAttribArray(1);
		offset += sizeof(glm::vec4);

		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		// Fill vertices here

	};

	// OpenGL
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


	void drawTile(MonShader::Shader* shader)
	{

		// fill batch


	//float vertices[] = {
	//	// positions          // colors           // texture coords
	//	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   bottomLeftX, bottomLeftY, // top right
	//	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   bottomRightX, bottomRightY, // bottom right
	//	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   topRightX, topRightY, // bottom left
	//	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   topLeftX, topLeftY // top left 
	//};




		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(tile->x, tile->y, 0.0f));
		//model = glm::scale(model, glm::vec3(glm::vec2(tile->width, tile->height), 1.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//glUniform3f(glGetUniformLocation(shader->id, "spriteColor"), obj->color.r, obj->color.g, obj->color.b);



		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader->id, "image"), 0);
		//glBindTexture(GL_TEXTURE_2D, tile->tileId);

		// TEST
		BatchData* batch = new BatchData();

		glBindVertexArray(batch->VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}


	// This isn't draw this is fill
	void fillBatch(MonShader::Shader* shader, int tileOffsetX, int tileOffsetY)
	{
		glUseProgram(shader->id);


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

		float textureCoords[] = {
			 bottomLeftX, bottomLeftY, // top right
			 bottomRightX, bottomRightY, // bottom right
			 topRightX, topRightY, // bottom left
			 topLeftX, topLeftY // top left 
		};

		
		// TODO(ck): Structure

		// Filling the batch the VAO, VBO or EBO do not get touched here
		// this is all CPU that is just filling it up getting all of the data 
		// ready checking if anything has changed to get rid of cached data
		
		// We fill the indices every frame too the reason why he does INSERT_BATCH
		// is because if the m_batches has indices in it already they need to get dumped
		// so that when push_quad gets called i think??
		// this method is a little confusing so i might have to do something less oopy 
		// for myself not that it is super oop but i might just 

		// then make a quad for our vertices so we push a quad for that tile 
		// we aren't like pushing a quad of opengl its literally the coords and stuff
		// then we move onto actual rendering for gpu

		


		// All of our 
		// Rendering at the end of the game->draw() function
		// this is where the renderer actually gets called and checks to see
		// we still update the VAO and VBO here 

		/*
		* // VERTICES
			m_vertex_count = count;
			gl.BindVertexArray(m_id);
			{
				// Create Buffer if it doesn't exist yet
				if (m_vertex_buffer == 0)
					gl.GenBuffers(1, &(m_vertex_buffer));

				// TODO:
				// Cache this
				m_vertex_size = gl_mesh_assign_attributes(m_vertex_buffer, GL_ARRAY_BUFFER, format, 0);

				// Upload Buffer
				gl.BindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
				gl.BufferData(GL_ARRAY_BUFFER, m_vertex_size * count, vertices, GL_DYNAMIC_DRAW);
			}
			gl.BindVertexArray(0);
		
		
		// INDICES
			m_instance_count = count;
			gl.BindVertexArray(m_id);
			{
				// Create Buffer if it doesn't exist yet
				if (m_instance_buffer == 0)
					gl.GenBuffers(1, &(m_instance_buffer));

				// TODO:
				// Cache this
				m_instance_size = gl_mesh_assign_attributes(m_instance_buffer, GL_ARRAY_BUFFER, format, 1);

				// Upload Buffer
				gl.BindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
				gl.BufferData(GL_ARRAY_BUFFER, m_instance_size * count, instances, GL_DYNAMIC_DRAW);
			}
			gl.BindVertexArray(0);
		
		
		
		*/

		
	}




	void drawTile(MonShader::Shader* shader, BatchData* batch, int tileOffsetX, int tileOffsetY)
	{
		glUseProgram(shader->id);

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

		float textureCoords[] = {
			 bottomLeftX, bottomLeftY, // top right
			 bottomRightX, bottomRightY, // bottom right
			 topRightX, topRightY, // bottom left
			 topLeftX, topLeftY // top left 
		};


		// fill batch


		//float vertices[] = {
		//	// positions          // colors           // texture coords
		//	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   bottomLeftX, bottomLeftY, // top right
		//	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   bottomRightX, bottomRightY, // bottom right
		//	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   topRightX, topRightY, // bottom left
		//	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   topLeftX, topLeftY // top left 
		//};




		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(tile->x, tile->y, 0.0f));
		//model = glm::scale(model, glm::vec3(glm::vec2(tile->width, tile->height), 1.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//glUniform3f(glGetUniformLocation(shader->id, "spriteColor"), obj->color.r, obj->color.g, obj->color.b);



		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader->id, "image"), 0);
		//glBindTexture(GL_TEXTURE_2D, tile->tileId);




		glBindVertexArray(batch->VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void drawTile(MonShader::Shader* shader, Tile* tile, Sprite* sprite)
	{
		glUseProgram(shader->id);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(tile->x, tile->y, 0.0f));
		model = glm::scale(model, glm::vec3(glm::vec2(tile->width, tile->height), 1.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//glUniform3f(glGetUniformLocation(shader->id, "spriteColor"), obj->color.r, obj->color.g, obj->color.b);

		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sprite->texture.id);

		glBindVertexArray(sprite->VAO);
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	
	//void gl_DrawTile(MonShader::Shader* shader, Tile* tile)
	//{
	//	glUseProgram(shader->id);
	//	glm::mat4 model = glm::mat4(1.0f);
	//	model = glm::translate(model, glm::vec3(tile->x, tile->y, 0.0f));
	//	model = glm::scale(model, glm::vec3(glm::vec2(tile->width, tile->height), 1.0f));

	//	glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_FALSE, glm::value_ptr(model));
	//	//glUniform3f(glGetUniformLocation(shader->id, "spriteColor"), obj->color.r, obj->color.g, obj->color.b);

	//	glActiveTexture(GL_TEXTURE0);
	//	glUniform1i(glGetUniformLocation(shader->id, "image"), 0);
	//	glBindTexture(GL_TEXTURE_2D, tile->tileId);

	//	glBindVertexArray(tile->sprite.VAO);
	//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//	glBindVertexArray(0);
	//}

	void drawTile(MonShader::Shader* shader, float minX, float minY, float maxX, float maxY)
	{
		glUseProgram(shader->id);
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(obj->pos, 0.0f));

		glm::vec2 size = glm::vec2(32.0f);
		model = glm::translate(model, glm::vec3(0.5f * size.x, 0.0f * size.y, 0.0f));
		//model = glm::rotate(model, obj->rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

		model = glm::scale(model, glm::vec3(size, 1.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//glUniform3f(glGetUniformLocation(shader->id, "spriteColor"), obj->color.r, obj->color.g, obj->color.b);

		////glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, obj->sprite.texture.id);

		//glBindVertexArray(obj->sprite.VAO);

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