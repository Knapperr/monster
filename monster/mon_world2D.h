#ifndef MON_WORLD2D_H
#define MON_WORLD2D_H

#include "mon_entity2D.h"

namespace Mon
{
	struct World2D
	{
		Entity2D entities[256];
		MonGL::RenderData2D renderData[256];
		TileMap* map;
		TileSheet sheet;

		int selectedEntityIndex;
		int entityCount;
	};

	bool InitWorld(World2D* world);

	static unsigned int AddEntity2D(World2D* world)
	{
		unsigned int entityIndex = world->entityCount++;

		Entity2D* entity = &world->entities[entityIndex];
		entity = {};

		return entityIndex;
	}

	static Entity2D* GetEntity2D(World2D* world, unsigned int index)
	{
		Entity2D* entity = 0;
		if ((index > 0) && (index < ArrayCount(world->entities)))
		{
			entity = &world->entities[index];
		}
		return entity;
	}

	static Entity2D* GetPlayer(World2D* world)
	{
		Entity2D* entity = 0;
		entity = &world->entities[1];
		return entity;
	}

}
#endif