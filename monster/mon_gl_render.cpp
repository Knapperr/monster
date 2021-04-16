#include "mon_gl_render.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


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
}

// OpenGL
void gl_DrawObject(Shader* shader, Entity* obj)
{
	glUseProgram(shader->id);
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

void gl_DrawModel(Shader* shader)
{
	glUseProgram(shader->id);

	//glUniformMatrix4fv(GetLoc(modelShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	//glUniformMatrix4fv(GetLoc(modelShader, "view"), 1, GL_FALSE, glm::value_ptr(view));

	//glUniform1i(GetLoc(modelShader, "selected"), obj.selected);

	/*
	for (unsigned int i = 0; i < obj.model->meshes.size(); i++)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;

		for (unsigned int j = 0; j < obj.model->meshes[i].textures.size(); ++j)
		{
			glActiveTexture(GL_TEXTURE0 + j); // activate the proper texture unit before binding
			// retrieve texture number (the N in diffuse_TextureN)
			std::string number;
			std::string name = obj.model->meshes[i].textures[j].type;

			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);
			else if (name == "texture_normal")
				number = std::to_string(normalNr++);
			else if (name == "texture_height")
				number = std::to_string(heightNr++);

			// now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(modelShader->id, (name + number).c_str()), j);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, obj.model->meshes[i].textures[j].id);
		}

		// Set position, rotation and scale
		glm::mat4 matModel = glm::mat4(1.0f);

		glm::vec4 clipthing = glm::vec4(1.0f);

		glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
												glm::vec3(obj.pos.x, obj.pos.y, obj.pos.z));
		matModel = matModel * matTranslate;

		glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
										glm::radians(obj.orientation.z),
										glm::vec3(0.0f, 0.0f, 1.0f));
		matModel = matModel * rotateZ;

		glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
										glm::radians(obj.orientation.y),
										glm::vec3(0.0f, 1.0f, 0.0f));
		matModel = matModel * rotateY;

		glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
										glm::radians(obj.orientation.x),
										glm::vec3(1.0f, 0.0f, 0.0f));
		matModel = matModel * rotateX;

		glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
										glm::vec3(obj.scale, obj.scale, obj.scale));

		matModel = matModel * matScale;
		glUniformMatrix4fv(GetLoc(modelShader, "model"), 1, GL_FALSE, glm::value_ptr(matModel));

		/*
		// INVERSE WAS FROM GRAPHICS CLASS
			GLint matModel_loc = glGetUniformLocation(shaderProgID, "matModel");
			GLint matModelInvTran_loc = glGetUniformLocation(shaderProgID, "matModelInvTrans");


		if (obj.wireFrame)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Draw mesh
		glBindVertexArray(obj.model->meshes[i].VAO);
		glDrawElements(GL_TRIANGLES, obj.model->meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Always good practice to set everything back to defaults once configured
		// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

	}
	*/
}


void gl_DrawTile(Shader* shader, Tile* obj, Sprite* sprite)
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

void gl_DrawTile(Shader* shader, float minX, float minY, float maxX, float maxY)
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
void DrawCollider(Sprite *sprite, float x, float y, float z, float radius, int numberOfSides)
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