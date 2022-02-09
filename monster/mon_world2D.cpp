#include "mon_world2D.h"

namespace Mon
{
	World2D::World2D()
	{

		// TODO(ck): MEMORY MANAGEMENT - Efficient tile map creation
		// TODO(ck): Move all of this into the initTileMap
		map = new TileMap();
		sheet = {};
		InitTileSheet(&sheet, "res/textures/basic_16.png");
		InitTileMap(map, &sheet);

		// ------------------------------------------------------------------

		// TODO(ck): Memory management
		player = new Entity2D();
		initEntity(player, "res/textures/ch_witch.png", true, v2(0, 0), 32);
		Entity2D* collider = new Entity2D();
		initEntity(collider, "res/textures/p1.png", true, v2(50, 50), 32);
		collider->sprite.wireFrame = true;
		//entities.push_back(collider);

		Entity2D* ball = new Entity2D();
		initEntity(ball, "res/textures/test_house.png", true, v2(75, 84), 96);
		entities.push_back(ball);

		Entity2D* ent = new Entity2D();
		initEntity(ent, "res/textures/ch_minion.png", true, v2(60, 20), 32);
		entities.push_back(ent);

		for (int i = 0; i < 5; ++i)
		{
			Entity2D* e = new Entity2D();
			initEntity(e, "res/textures/p1_.png", true,v2(30 * i, 5 * i), 32);
			//entities.push_back(e);
		}

		Entity2D* partner = new Entity2D();
		initEntity(partner, "res/textures/sRansome.png", true, v2(20.0f, 20.0f), 32);
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