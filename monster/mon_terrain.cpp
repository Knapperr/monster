#include "mon_terrain.h"
#include <string>

#include <glad/glad.h>

// TODO(ck): Send info to glRender
#define SIZE 32
#define VERTEX_COUNT 16
#define TEXTURE_ID_COUNT 4
Terrain::Terrain(int gridX, int gridZ)
{
	x = (float)gridX * SIZE;
	z = (float)gridZ * SIZE;

	mesh = {};
	wireFrame = false;
	drawTexture = true;
}

Terrain::~Terrain()
{
	// TODO(ck): Move
	// delete[] grass.matrices;
	delete[] textureIds;
	// TODO(ck): delete index buffer too?
}

float Terrain::getHeight(int x, int z)
{
	float gridSquareSize = (float)SIZE / ((float)VERTEX_COUNT);
	int gridX = (int)std::floor(x / gridSquareSize);
	int gridZ = (int)std::floor(z / gridSquareSize);

	if (gridX >= VERTEX_COUNT || gridZ >= VERTEX_COUNT || gridX < 0 || gridZ < 0)
		return 0;

	float xCoord = ((int)x % (int)gridSquareSize) / gridSquareSize;
	float zCoord = ((int)z % (int)gridSquareSize) / gridSquareSize;
	float result;

	if (xCoord <= (1 - zCoord))
	{
		result = barryCentric(glm::vec3(0, lookUpHeight(gridX, gridZ), 0),
							  glm::vec3(1, lookUpHeight(gridX + 1, gridZ), 0),
							  glm::vec3(0, lookUpHeight(gridX, gridZ + 1), 1),
							  glm::vec2(xCoord, zCoord));
	}
	else
	{
		result = barryCentric(glm::vec3(1, lookUpHeight(gridX + 1, gridZ), 0),
							  glm::vec3(1, lookUpHeight(gridX + 1, gridZ + 1), 1),
							  glm::vec3(0, lookUpHeight(gridX, gridZ + 1), 1),
							  glm::vec2(xCoord, zCoord));
	}

	return result;
}

// NOTE(ck): https://github.com/munro98/LandscapeWorld/blob/master/src/Terrain.cpp
float Terrain::barryCentric(Mon::v3 p1, Mon::v3 p2, Mon::v3 p3, Mon::v2 pos)
{
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

float Terrain::lookUpHeight(int x, int z)
{
	// NOTE(ck): Mine should be different than this
	// int i = (x + 1) + ((z + 1) * (VERTEX_COUNT + 3));
	int i = (x + 1) + ((z + 1) * (VERTEX_COUNT));
	return 0.0f;
	//return mesh.vertices[i].position.y;
}

/*
void Terrain::GenerateGrass()
{

	if (this->grass.matrices == NULL)
	{
		this->grass = {};
		this->grass.model = Model("content/objects/grass_star/Grass.obj");
		// TODO(CK): Need a getter for texture id
		this->grass.textureId = this->grass.model.textures_loaded[0].id;
		grass.matrices = new glm::mat4[this->grass.amount];
	}
	else
	{
		delete[] grass.matrices;
		grass.matrices = new glm::mat4[this->grass.amount];
	}

	srand(time(NULL));
	// TODO(CK): Shouldn't increment by 3 this is buggy
	for (unsigned int i = 0; i < this->grass.amount; i += 3)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]

		// TODO(ck): Use blue noise for this
		float x = rand() % 380 + 10;
		float y = 0;
		float z = rand() % 380 + 10;
		model = glm::translate(model, v3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = 1.0f;
		model = glm::scale(model, glm::vec3(scale));

		// 4. now add to list of matrices
		this->grass.matrices[i] = model;

		if (i < (this->grass.amount - 3))
		{
			float rotAngle = 40.0f;
			model = glm::rotate(model, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
			assert((i + 1) < this->grass.amount);
			this->grass.matrices[i + 1] = model;

			rotAngle = 135.0f;
			model = glm::rotate(model, rotAngle, glm::vec3(0.0f, -1.0f, 0.0f));
			assert((i + 2) < this->grass.amount);
			this->grass.matrices[i + 2] = model;
		}
	}

	// configure instanced array
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, this->grass.amount * sizeof(glm::mat4), &this->grass.matrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < this->grass.model.meshes.size(); i++)
	{
		unsigned int VAO = grass.model.meshes[i].VAO;
		glBindVertexArray(VAO);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
}
*/
