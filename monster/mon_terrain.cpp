#include "mon_terrain.h"
#include <string>


#define SIZE 64
#define VERTEX_COUNT 64

Terrain::Terrain(int gridX, int gridZ)
{
	x = (float)gridX * SIZE;
	z = (float)gridZ * SIZE;

	wireFrame = false;
	drawTexture = true;

	mesh = {};
	heightMap = new float[(SIZE + 1) * (SIZE + 1)];
	// NOTE(ck): Power of 2 needs to be used for SIZE so that
	//			texture coords to be proper on the cells
	/*I think the proper way to do this is have a map structure with the positions as int32 and then 
	have the mesh that just gets the vertices from the positions just like a tilemap this way the 
	grid is its own thing and then the 
	*/
	// data.meshIndex = GetMesh("grid");
	mesh.meshIndex = 4;
	MonGL::InitGrid(&mesh, SIZE, SIZE, heightMap);

	Mon::InitBoxCollider(&collider);
	collider.data.color = Mon::v3(0.1f, 0.4f, 0.95f);
	collider.worldPos = Mon::v3(-2.0f, -6.40f, -1.10f);
	collider.max = Mon::v3(68.00f, 6.10f, 64.0f);
	Mon::SetBoxTransform(&collider, collider.worldPos, Mon::v3(1.0f));
}

Terrain::~Terrain()
{
	delete[] heightMap;
}

float GetHeight(int x, int z)
{
	return 0.0f;
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
		result = barryCentric(Mon::v3(0, lookUpHeight(gridX, gridZ), 0),
							  Mon::v3(1, lookUpHeight(gridX + 1, gridZ), 0),
							  Mon::v3(0, lookUpHeight(gridX, gridZ + 1), 1),
							  Mon::v2(xCoord, zCoord));
	}
	else
	{
		result = barryCentric(Mon::v3(1, lookUpHeight(gridX + 1, gridZ), 0),
							  Mon::v3(1, lookUpHeight(gridX + 1, gridZ + 1), 1),
							  Mon::v3(0, lookUpHeight(gridX, gridZ + 1), 1),
							  Mon::v2(xCoord, zCoord));
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
	return heightMap[i];
	
	// pass a height map to the renderer to fill with positions?
	// return terrain->heightMap[i];
	//return mesh.vertices[i].position.y;
}

void InitMousePicker(MousePicker* picker)
{
	picker->projectionMatrix = Mon::mat4(1.0f);
	picker->currentRay = Mon::v3(0.0f);
	picker->currentTerrainPoint = Mon::v3(0.0f);
}

#define RECURSION_COUNT 200.0f
#define RAY_RANGE 100.0f
void UpdatePicker(MousePicker* picker, Terrain* terrain, Mon::v2 mousePos, Mon::mat4 viewMatrix, Mon::mat4 projection, Mon::v3 cameraPos)
{
	// TODO(ck): Do this for now? the java implementation points to it 
	// ours cant cause of glm
	picker->projectionMatrix = projection;
	picker->currentRay = CalculateMouseRay(picker, mousePos, viewMatrix);
	picker->currentTerrainPoint = binarySearch(terrain, 0, 0, RAY_RANGE, picker->currentRay, cameraPos);
}

Mon::v3 CalculateMouseRay(MousePicker* picker, Mon::v2 mousePos, Mon::mat4 viewMatrix)
{
	Mon::v2 normalizedCoords = GetNormalizedDeviceCoords(mousePos);
	Mon::v4 clipCoords = Mon::v4(normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f);
	Mon::v4 eyeCoords = ToEyeCoords(picker, clipCoords);
	Mon::v3 worldRay = ToWorldCoords(eyeCoords, viewMatrix);

	return worldRay;
}

Mon::v2 GetNormalizedDeviceCoords(Mon::v2 mousePos)
{
	// TODO(ck): Get from settings
	Mon::v2 window = { 1440.0f, 900.0f };
	Mon::v2 port = { 960.0f, 540.0f };
	Mon::v2 offset = (window - port); // port.x & y = 5

	// TODO(ck): Figure out offset. Working for y but not for x 
	// STUDY(ck): 
	// x only needs to account for the offset x position of the port but
	// the y position needs to subtract the offset of the window and port as well
	// but x doesn't seem to need this..?
	float x = (2.0f * (mousePos.x)) / port.x - 1.0f;
	float y = (2.0f * (mousePos.y - offset.y)) / port.y - 1.0f;

	// NOTE(ck): Make sure to reverse the direction of y!
	return Mon::v2(x, -y);
}

Mon::v4 ToEyeCoords(MousePicker* picker, Mon::v4 clipCoords)
{
	Mon::v4 eyeCoords = glm::inverse(picker->projectionMatrix) * clipCoords;
	return Mon::v4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
}

Mon::v3 ToWorldCoords(Mon::v4 eyeCoords, Mon::mat4 viewMatrix)
{
	Mon::v4 rayWorld = glm::inverse(viewMatrix) * eyeCoords;
	Mon::v3 mouseRay = Mon::v3(rayWorld.x, rayWorld.y, rayWorld.z);
	mouseRay = glm::normalize(mouseRay);

	return mouseRay;
}


Mon::v3 binarySearch(Terrain* terrain, int count, float start, float finish, Mon::v3 ray, Mon::v3 cameraPosition)
{
	float half = start + ((finish - start) / 2.0f);
	if (count >= RECURSION_COUNT)
	{
		return getPointOnRay(ray, half, cameraPosition);
	}

	if (intersectionInRange(terrain, start, half, ray, cameraPosition))
	{
		return binarySearch(terrain, count + 1, start, half, ray, cameraPosition);
	}
	else
	{
		return binarySearch(terrain, count + 1, half, finish, ray, cameraPosition);
	}
}

Mon::v3 getPointOnRay(Mon::v3 ray, float distance, Mon::v3 cameraPosition)
{
	Mon::v3 scaledRay = Mon::v3(ray.x * distance, ray.y * distance, ray.z * distance);
	return cameraPosition + scaledRay;
}

bool intersectionInRange(Terrain* terrain, float start, float finish, Mon::v3 ray, Mon::v3 cameraPosition)
{
	Mon::v3 startPoint = getPointOnRay(ray, start, cameraPosition);
	Mon::v3 endPoint = getPointOnRay(ray, finish, cameraPosition);

	if (!isUnderGround(terrain, startPoint) && isUnderGround(terrain, endPoint))
		return true;
	else
		return false;
}

bool isUnderGround(Terrain* terrain, Mon::v3 testPoint)
{
	float height = terrain->getHeight(testPoint.x, testPoint.z);
	if (testPoint.y < height)
		return true;
	else
		return false;
}
