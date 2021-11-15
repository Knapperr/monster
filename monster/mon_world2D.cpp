#include "mon_world2D.h"

namespace Mon
{
	World2D::World2D()
	{

		// TODO(ck): MEMORY MANAGEMENT - Efficient tile map creation
		// TODO(ck): Move all of this into the initTileMap
		map = new TileMap();
		sheet = {};
		initTileSheet(&sheet, "res/textures/basic_16.png");
		initTileMap(map, &sheet);

		// ------------------------------------------------------------------

		// TODO(ck): Memory management
		player = new Entity2D();
		initEntity(player, "res/textures/sPlayerRPG.png", true, v2(0, 0));
		Entity2D* collider = new Entity2D();
		initEntity(collider, "res/textures/p1.png", true, v2(50, 50));
		collider->sprite.wireFrame = true;
		//entities.push_back(collider);

		Entity2D* ball = new Entity2D();
		initEntity(ball, "res/textures/sflow.png", true, v2(10, 10));
		entities.push_back(ball);

		for (int i = 0; i < 5; ++i)
		{
			Entity2D* e = new Entity2D();
			initEntity(e, "res/textures/p1_.png", true,v2(30 * i, 5 * i));
			//entities.push_back(e);
		}

		Entity2D* house = new Entity2D();
		initEntity(house, "res/textures/acorn_house.png", true, v2(100.0f, 100.0f));
		entities.push_back(house);

		Entity2D* partner = new Entity2D();
		initEntity(partner, "res/textures/sRansome.png", true, v2(20.0f, 20.0f));
		entities.push_back(partner);

	}

	World2D::~World2D()
	{
		for (size_t i = 0; i < entities.size(); ++i)
		{
			delete entities[i];
		}
		delete player;
	}
}