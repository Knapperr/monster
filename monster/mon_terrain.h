#ifndef MON_TERRAIN_H
#define MON_TERRAIN_H

#include "mon_shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Material
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

struct MonVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Terrain
{
public:
	float x;
	float z;
	unsigned int VBO;
	unsigned int VAO;
	unsigned int indicesLength;

	int selectedTextureId;
	bool wireFrame;
	bool drawTexture;

	MonVertex* vertices;
	//Grass grass;
	Shader shader;
	Material material;
	// Shader* grassShader;
	int* textureIds;
	float* heightMap;

	Terrain(int gridX, int gridZ);
	~Terrain();

	float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos);
	float getHeight(int x, int z);
	float lookUpHeight(int x, int z);
	void generate();
	//void GenerateGrass();
};


#endif