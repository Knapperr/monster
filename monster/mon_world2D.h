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
	};

	void InitWorld(World2D_* world);


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