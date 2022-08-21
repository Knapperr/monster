#include "mon_grid.h"
#include <string>


#define SIZE 64
#define VERTEX_COUNT 64

void InitGrid(Grid* grid)
{
	grid->x = (float)0 * SIZE;
	grid->z = (float)0 * SIZE;

	// TODO(ck): Memory allocation 
	// Loading chunks and cells 
	int tileChunkCountXYZ = 1;
	int chunkDim = 8;
	int tileCount = chunkDim * chunkDim;
	grid->chunks = new Chunk[tileChunkCountXYZ];
	grid->chunks[0].cells = new Mon::int32[tileCount];
	for (int i = 0; i < tileCount; ++i)
	{
		grid->chunks->cells[i] = 1;
	}


	// NOTE(ck): Power of 2 needs to be used for SIZE so that
	//			texture coords to be proper on the cells
	grid->data = {};
	grid->data.meshIndex = 4; // data.meshIndex = GetMesh("grid");
	grid->data.textureIndex = 12;
	grid->data.programData.texCoordScale = 8.0f;
}


void GetGridPosition(Mon::v3 pos)
{
	Mon::v3 gridPos;
	gridPos.x = (int)(pos.x / 1.0f);
	gridPos.y = 0.0f;
	gridPos.z = (int)(pos.z / 1.0f);
}

float GetHeight(MonGL::Mesh* mesh, int x, int z)
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
		result = BarryCentric(Mon::v3(0, LookUpHeight(mesh, gridX, gridZ), 0),
							  Mon::v3(1, LookUpHeight(mesh, gridX + 1, gridZ), 0),
							  Mon::v3(0, LookUpHeight(mesh, gridX, gridZ + 1), 1),
							  Mon::v2(xCoord, zCoord));
	}
	else
	{
		result = BarryCentric(Mon::v3(1, LookUpHeight(mesh, gridX + 1, gridZ), 0),
							  Mon::v3(1, LookUpHeight(mesh, gridX + 1, gridZ + 1), 1),
							  Mon::v3(0, LookUpHeight(mesh, gridX, gridZ + 1), 1),
							  Mon::v2(xCoord, zCoord));
	}

	return result;
}

// NOTE(ck): https://github.com/munro98/LandscapeWorld/blob/master/src/Terrain.cpp
float BarryCentric(Mon::v3 p1, Mon::v3 p2, Mon::v3 p3, Mon::v2 pos)
{
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

float LookUpHeight(MonGL::Mesh* mesh, int x, int z)
{
	// NOTE(ck): Mine should be different than this
	// int i = (x + 1) + ((z + 1) * (VERTEX_COUNT + 3));
	int i = (x + 1) + ((z + 1) * (VERTEX_COUNT));
	return mesh->vertices[i].position.y;
}

void InitMousePicker(MousePicker* picker)
{
	picker->projectionMatrix = Mon::mat4(1.0f);
	picker->currentRay = Mon::v3(0.0f);
	picker->currentTerrainPoint = Mon::v3(0.0f);
}

#define RECURSION_COUNT 200.0f
#define RAY_RANGE 100.0f
void UpdatePicker(MousePicker* picker, MonGL::Mesh* mesh, Mon::v2 mousePos, Mon::mat4 viewMatrix, Mon::mat4 projection, Mon::v3 cameraPos)
{
	// TODO(ck): Do this for now? the java implementation points to it 
	// ours cant cause of glm
	picker->projectionMatrix = projection;
	picker->currentRay = CalculateMouseRay(picker, mousePos, viewMatrix);
	picker->currentTerrainPoint = BinarySearch(mesh, 0, 0, RAY_RANGE, picker->currentRay, cameraPos);
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

Mon::v3 BinarySearch(MonGL::Mesh* mesh, int count, float start, float finish, Mon::v3 ray, Mon::v3 cameraPosition)
{
	float half = start + ((finish - start) / 2.0f);
	if (count >= RECURSION_COUNT)
	{
		return GetPointOnRay(ray, half, cameraPosition);
	}

	if (IntersectionInRange(mesh, start, half, ray, cameraPosition))
	{
		return BinarySearch(mesh, count + 1, start, half, ray, cameraPosition);
	}
	else
	{
		return BinarySearch(mesh, count + 1, half, finish, ray, cameraPosition);
	}
}

Mon::v3 GetPointOnRay(Mon::v3 ray, float distance, Mon::v3 cameraPosition)
{
	Mon::v3 scaledRay = Mon::v3(ray.x * distance, ray.y * distance, ray.z * distance);
	return cameraPosition + scaledRay;
}

bool IntersectionInRange(MonGL::Mesh* mesh, float start, float finish, Mon::v3 ray, Mon::v3 cameraPosition)
{
	Mon::v3 startPoint = GetPointOnRay(ray, start, cameraPosition);
	Mon::v3 endPoint = GetPointOnRay(ray, finish, cameraPosition);

	if (!IsUnderGround(mesh, startPoint) && IsUnderGround(mesh, endPoint))
		return true;
	else
		return false;
}

bool IsUnderGround(MonGL::Mesh* mesh, Mon::v3 testPoint)
{
	// TODO(ck): completely pull out
	float height = GetHeight(mesh, (int)testPoint.x, (int)testPoint.z);
	if (testPoint.y < height)
		return true;
	else
		return false;
}
