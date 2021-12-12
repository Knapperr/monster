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

	MonGL::InitBoundingBox(&collider.data);
	collider.data.color = Mon::v3(0.1f, 0.4f, 0.95f);
	collider.worldPos = Mon::v3(-2.0f, -6.40f, -1.10f);
	collider.size.max = Mon::v3(133.00f, 6.10f, 131.0f);
	Mon::SetTransform(&collider, collider.worldPos, Mon::v3(1.0f));
}

Terrain::~Terrain()
{
	// 
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