#ifndef MON_WORLD2D_H
#define MON_WORLD2D_H

#include "mon_entity2D.h"

namespace Mon
{
	struct World2D_
	{
		Entity2D entities[256];
		MonGL::RenderData2D renderData[256];
		TileMap* map;
		TileSheet sheet;

		int entityCount;
	};

	bool InitWorld(World2D_* world);

	static unsigned int AddEntity2D(World2D_* world)
	{
		unsigned int entityIndex = world->entityCount++;

		Entity2D* entity = &world->entities[entityIndex];
		entity = {};

		return entityIndex;
	}

	static Entity2D* GetEntity2D(World2D_* world, unsigned int index)
	{
		Entity2D* entity = 0;
		if ((index > 0) && (index < ArrayCount(world->entities)))
		{
			entity = &world->entities[index];
		}
		return entity;
	}

	static Entity2D* GetPlayer(World2D_* world)
	{
		Entity2D* entity = 0;
		entity = &world->entities[1];
		return entity;
	}

	class World2D
	{
	public:
		// Just put the player with the entites and have a getPlayer for it
		Entity2D* player;
		//GameObject* objects[MAX_OBJECTS];
		std::vector<Entity2D*> entities;
		std::vector<MonGL::RenderData2D> drawData;
		TileMap* map;
		TileSheet sheet;

		World2D();
		~World2D();

		/*
		dont mind doing something like this
		int player = obj.getplayer();
		if (player > -1)
		{
			obj.entities[player].xp += ss_toi(words[1]);
			obj.entities[player].yp += ss_toi(words[2]);

		int entityclass::getplayer()
		{
		//Returns the index of the first player entity
		for (size_t i = 0; i < entities.size(); i++)
		{
			if(entities[i].type==0)
			{
				return i;
			}
		}

		return -1;
	}

		*/
	};

}
#endif