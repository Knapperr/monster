#ifndef MON_GRID_H
#define MON_GRID_H

#include "mon_math.h"
#include "mon_gl_render.h"


struct CellPosition
{
	Mon::int32 absX;
	Mon::int32 absY;
	Mon::int32 absZ;

	Mon::v2 offset;
};

struct world_entity_block
{
	//uint32 entityCount;
	//uint32 lowEntityIndex[16];
	world_entity_block* next;
};

struct Chunk
{
	Mon::int32 chunkX;
	Mon::int32 chunkY;
	Mon::int32 chunkZ;

	world_entity_block firstBlock;

	Chunk* nextInHash;
};

struct Grid
{
	Chunk* chunks;

	float x;
	float z;
	int selectedTextureIndex;
	
	int meshIndex;
	int textureIndex;
};

struct Grid_
{
	Chunk* chunks;

	float x;
	float z;
};

struct MousePicker
{
	Mon::v3 currentTerrainPoint;
	Mon::v3 currentRay;
	Mon::v3 pos;
	Mon::mat4 projectionMatrix;
};

void InitGrid(Grid* grid);
void InitGrid(Grid_* grid);
void GetGridPosition(Mon::v3 pos);

void InitMousePicker(MousePicker* picker);
void UpdatePicker(MousePicker* picker, MonGL::Mesh* mesh, Mon::v2 mousePos, Mon::mat4 viewMatrix, Mon::mat4 projection, Mon::v3 cameraPos);
Mon::v3 CalculateWorldMouseRay(MousePicker* picker, Mon::v2 mousePos, Mon::mat4 viewMatrix);
Mon::v4 ToEyeCoords(MousePicker* picker, Mon::v4 clipCoords);
Mon::v3 ToWorldCoords(Mon::v4 eyeCoords, Mon::mat4 viewMatrix);
Mon::v2 GetNormalizedDeviceCoords(Mon::v2 mousePos);

Mon::v3 GetPointOnRay(Mon::v3 ray, float distance, Mon::v3 cameraPosition);
Mon::v3 BinarySearch(MonGL::Mesh* mesh, int count, float start, float finish, Mon::v3 ray, Mon::v3 cameraPosition);
bool IntersectionInRange(MonGL::Mesh* mesh, float start, float finish, Mon::v3 ray, Mon::v3 cameraPosition);
bool IsUnderGround(MonGL::Mesh* mesh, Mon::v3 testPoint);

float GetHeight(MonGL::Mesh* mesh, int x, int z);
float LookUpHeight(MonGL::Mesh* mesh, int x, int z);

#endif // MON_GRID_H