#include "mon_terrain.h"
#include <string>

#include "mon_gl_render.h"
#include <glad/glad.h>


#define SIZE 32
#define VERTEX_COUNT 16
#define TEXTURE_ID_COUNT 4
Terrain::Terrain(int gridX, int gridZ)
{
	x = (float)gridX * SIZE;
	z = (float)gridZ * SIZE;

	VAO = 0;
	VBO = 0;
	indicesLength = 0;
	wireFrame = false;
	drawTexture = true;

	// TODO(ck): memory allocation
	vertices = new MonVertex[VERTEX_COUNT * VERTEX_COUNT];
	// HOW TO DO ARENA WITH FLOAT
	heightMap = new float[VERTEX_COUNT * VERTEX_COUNT];

	textureIds = new int[TEXTURE_ID_COUNT];
	std::string textPath = "res/textures/terrain/";
	std::string textDir = textPath.substr(0, textPath.find_last_of('/'));
	textureIds[0] = MonGL::LoadTextureFile("1024multi.png", textDir, false);
	textureIds[1] = MonGL::LoadTextureFile("rock.png", textDir, false);
	textureIds[2] = MonGL::LoadTextureFile("grass.jpg", textDir, false);
	textureIds[3] = MonGL::LoadTextureFile("snow.jpg", textDir, false);

	selectedTextureId = textureIds[2];

	// TODO(ck): just use the model shader?
	// base shader?
	//shader = {};
	//LoadShader(&shader, "res/shaders/vert_colors.glsl", "res/shaders/frag_colors.glsl", NULL);
	material = {};
	material.ambient = glm::vec3(1.0f, 0.5f, 0.6f);
	material.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
	material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
	material.shininess = 32.0f;
	// grass shader
}

Terrain::~Terrain()
{
	// delete[] grass.matrices;
	delete[] vertices;
	delete[] textureIds;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
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
float Terrain::barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos)
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
	return heightMap[i];
}

void Terrain::generate()
{
	// TODO(ck): Memory Allocation
	int* indices = new int[6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1)];
	int verticesLength = VERTEX_COUNT * VERTEX_COUNT;
	indicesLength = 6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1);

	int index = 0;
	for (int i = 0; i < VERTEX_COUNT; ++i)
	{
		for (int j = 0; j < VERTEX_COUNT; ++j)
		{
			vertices[index].position.x = (float)j / ((float)VERTEX_COUNT - 1) * SIZE;

			vertices[index].position.y = 0.0f;
			heightMap[index] = vertices[index].position.y;

			vertices[index].position.z = (float)i / ((float)VERTEX_COUNT - 1) * SIZE;

			vertices[index].normal.x = 0;
			vertices[index].normal.y = 1;
			vertices[index].normal.z = 0;

			vertices[index].texCoords.x = (float)j / ((float)VERTEX_COUNT - 1);
			vertices[index].texCoords.y = (float)i / ((float)VERTEX_COUNT - 1);

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
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &ebo);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesLength * sizeof(MonVertex), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(int), indices, GL_STATIC_DRAW);

	// pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MonVertex), (void*)0);
	// normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MonVertex), (void*)offsetof(MonVertex, normal));
	// tex coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MonVertex), (void*)offsetof(MonVertex, texCoords));

	// TODO(CK) & STUDY(CK): I forgot 3 * sizeof(float) in the last parameter 
	// I need to make sure that I study glBufferData and glVertexAttribPointer I think because 
	// it was missing the stride it wasn't formating the data correctly which is why it looked
	// like a mountain
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	// STUDY(CK): For some reason at the very end of this (void*)(6 * sizeof(float))); 
	// was causing the texcoords to distort.. this is probably why you are meant to have
	// ONE BUFFER bound instead 
	// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	// unbind
	glBindVertexArray(0);
	delete[] indices;
	delete[] vertices;
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
		model = glm::translate(model, glm::vec3(x, y, z));

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
