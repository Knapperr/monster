#ifndef MON_TERRAIN_H
#define MON_TERRAIN_H

#include "mon_shader.h"
#include "mon_entity.h"

struct TerrainPosition
{
	uint32_t absTileX;
	uint32_t absTileY;
	// NOTE(casey): These are the offsets from the tile center
	Mon::v2 offset;
};

class Terrain
{
public:
	float x;
	float z;

	int selectedTextureId;
	bool wireFrame;
	bool drawTexture;

	MonGL::RenderData mesh;
	Mon::Collider collider;
	
	float* heightMap;

	Terrain(int gridX, int gridZ);
	~Terrain();

	float barryCentric(Mon::v3 p1, Mon::v3 p2, Mon::v3 p3, Mon::v2 pos);
	float getHeight(int x, int z);
	float lookUpHeight(int x, int z);
};

struct MousePicker
{
	Mon::v3 currentTerrainPoint;
	Mon::v3 currentRay;
	Mon::v3 pos;
	Mon::mat4 projectionMatrix;
};

void InitMousePicker(MousePicker* picker);
void UpdatePicker(MousePicker* picker, Terrain* terrain, Mon::v2 mousePos, Mon::mat4 viewMatrix, Mon::mat4 projection, Mon::v3 cameraPos);
Mon::v3 CalculateMouseRay(MousePicker* picker, Mon::v2 mousePos, Mon::mat4 viewMatrix);
Mon::v4 ToEyeCoords(MousePicker* picker, Mon::v4 clipCoords);
Mon::v3 ToWorldCoords(Mon::v4 eyeCoords, Mon::mat4 viewMatrix);
Mon::v2 GetNormalizedDeviceCoords(Mon::v2 mousePos);

Mon::v3 binarySearch(Terrain* terrain, int count, float start, float finish, Mon::v3 ray, Mon::v3 cameraPosition);
Mon::v3 getPointOnRay(Mon::v3 ray, float distance, Mon::v3 cameraPosition);
bool intersectionInRange(Terrain* terrain, float start, float finish, Mon::v3 ray, Mon::v3 cameraPosition);
bool isUnderGround(Terrain* terrain, Mon::v3 testPoint);


#endif