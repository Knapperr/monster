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

	heightMap = new float[(SIZE + 1) * (SIZE + 1)];
	// NOTE(ck): Power of 2 needs to be used for SIZE so that
	//			texture coords to be proper on the cells
	/*I think the proper way to do this is have a map structure with the positions as int32 and then 
	have the mesh that just gets the vertices from the positions just like a tilemap this way the 
	grid is its own thing and then the 
	*/
	// data.meshIndex = GetMesh("grid");
	data.meshIndex = 4;
	
	Mon::InitBoxCollider(&collider);
	collider.data.color = Mon::v3(0.1f, 0.4f, 0.95f);
	collider.worldPos = Mon::v3(-2.0f, -6.40f, -1.10f);
	collider.max = Mon::v3(68.00f, 6.10f, 64.0f);
	Mon::SetBoxTransform(&collider, collider.worldPos, Mon::v3(1.0f));

	std::string textPath = "res/textures/terrain/1024multi.png";
	MonGL::Texture text = {};
	data.textures[0] = text;
	MonGL::LoadTextureFile(&data.textures[0], textPath.c_str(), MonGL::TextureType::Diffuse, false, false, true, false);

	textPath = "res/textures/terrain/grass.jpg";
	MonGL::Texture text1 = {};
	data.textures[1] = text1;
	MonGL::LoadTextureFile(&data.textures[1], textPath.c_str(), MonGL::TextureType::Diffuse, false, false, true, false);

	textPath = "res/textures/terrain/pix_grass.png";
	MonGL::Texture text2 = {};
	data.textures[2] = text2;
	MonGL::LoadTextureFile(&data.textures[2], textPath.c_str(), MonGL::TextureType::Diffuse, false, false, true, true);

	textPath = "res/textures/terrain/snow.jpg";
	MonGL::Texture text3 = {};
	data.textures[3] = text3;
	MonGL::LoadTextureFile(&data.textures[3], textPath.c_str(), MonGL::TextureType::Diffuse, false, false, true, false);
}

Terrain::~Terrain()
{
	delete[] heightMap;
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
	float height = GetHeight(mesh, testPoint.x, testPoint.z);
	if (testPoint.y < height)
		return true;
	else
		return false;
}
