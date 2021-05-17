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
		data->size.X(1.0f, 2.0f);
		data->size.Y(1.0f, 2.0f);
		data->size.Z(1.0f, 2.0f);

		// Set world matrix to be the same size as the bounding box
		//data->worldMatrix = GetTransform(&data->size);

		/*
		* 
		* TODO this is how we get the size of it 
		  GLfloat
		min_x, max_x,
		min_y, max_y,
		min_z, max_z;
	  min_x = max_x = mesh->vertices[0].x;
	  min_y = max_y = mesh->vertices[0].y;
	  min_z = max_z = mesh->vertices[0].z;
	  for (int i = 0; i < mesh->vertices.size(); i++) {
		if (mesh->vertices[i].x < min_x) min_x = mesh->vertices[i].x;
		if (mesh->vertices[i].x > max_x) max_x = mesh->vertices[i].x;
		if (mesh->vertices[i].y < min_y) min_y = mesh->vertices[i].y;
		if (mesh->vertices[i].y > max_y) max_y = mesh->vertices[i].y;
		if (mesh->vertices[i].z < min_z) min_z = mesh->vertices[i].z;
		if (mesh->vertices[i].z > max_z) max_z = mesh->vertices[i].z;
	  }
	  glm::vec3 size = glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z);
	  glm::vec3 center = glm::vec3((min_x+max_x)/2, (min_y+max_y)/2, (min_z+max_z)/2);
	  glm::mat4 transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);
		
		
		i see here you are multiplying above with the models model matrix so that
		you can that might be a vec3? but i dont think so?

		/* Apply object's transformation matrix this is the world transform 
		* thats why its called object2world i guess cause its what gets called when drawing happens??
		* same as position?

		
		*/

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






	void gl_InitRenderData(Sprite* sprite)
	{
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

	// OpenGL
	void gl_DrawObject(MonShader::Shader* shader, Entity* obj)
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

	void gl_DrawTile(MonShader::Shader* shader, Tile* obj, Sprite* sprite)
	{
		glUseProgram(shader->id);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(obj->x, obj->y, 0.0f));
		model = glm::scale(model, glm::vec3(glm::vec2(obj->width, obj->height), 1.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//glUniform3f(glGetUniformLocation(shader->id, "spriteColor"), obj->color.r, obj->color.g, obj->color.b);

		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sprite->texture.id);

		glBindVertexArray(sprite->VAO);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	void gl_DrawTile(MonShader::Shader* shader, float minX, float minY, float maxX, float maxY)
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

#define PI 3.14159265358f
	void DrawCollider(Sprite* sprite, float x, float y, float z, float radius, int numberOfSides)
	{
		// TODO(There has to be a way to just put vertices inside of the collider and then 
		// reference those vertices and draw with our current shader the same way we 
		// do with an actual sprite
		float doublePi = 2.0f * PI;
		int numberOfVertices = numberOfSides + 2;

		// for now probably have to calculate this by collider size?
		float* circleVerticesX = new float[numberOfVertices];
		float* circleVerticesY = new float[numberOfVertices];
		float* circleVerticesZ = new float[numberOfVertices];

		circleVerticesX[0] = x;
		circleVerticesY[0] = y;
		circleVerticesZ[0] = z;

		for (int i = 1; i < numberOfVertices; i++)
		{
			circleVerticesX[i] = x + (radius * cos(i * doublePi / numberOfSides));
			circleVerticesY[i] = y + (radius * sin(i * doublePi / numberOfSides));
			circleVerticesZ[i] = z;
		}

		float* allCircleVertices = new float[(numberOfVertices) * 3];

		for (int i = 0; i < numberOfVertices; i++)
		{
			allCircleVertices[i * 3] = circleVerticesX[i];
			allCircleVertices[(i * 3) + 1] = circleVerticesY[i];
			allCircleVertices[(i * 3) + 2] = circleVerticesZ[i];
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, allCircleVertices);
		glDrawArrays(GL_TRIANGLE_FAN, 0, numberOfVertices);
		glDisableClientState(GL_VERTEX_ARRAY);

		// Cant see this being any good
		delete[]circleVerticesX;
		delete[]circleVerticesY;
		delete[]circleVerticesZ;
		delete[]allCircleVertices;
	}


	void gl_CleanUp(Sprite* sprite)
	{
		glDeleteVertexArrays(1, &sprite->VAO);
		glDeleteBuffers(1, &sprite->VBO);
		//glDeleteBuffers(1, &EBO);
		//glDeleteProgram(shaderProgram);
	}

}