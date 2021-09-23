#include "mon_world.h"

namespace Mon
{
	World::World()
	{

		// TODO(ck): MEMORY MANAGEMENT - Efficient tile map creation
		map = new TileMap();
		sheet = {};
		initTileSheet(&sheet, "res/textures/basic_16.png");

		initTileMap(map, &sheet);
		MonGL::initTileMap(map->tiles.size());

		for (int i = 0; i < map->tiles.size(); ++i)
		{
			MonGL::fillBatch(map->tiles[i].offsetX, map->tiles[i].offsetY, map->tiles[i].x, map->tiles[i].y, 16);
		}

		MonGL::bindVertices();

		// TODO(ck): Memory management
		player = new Entity2D();
		initEntity(player, "res/textures/p1.png", true, v2(0, 0));
		Entity2D* collider = new Entity2D();
		initEntity(collider, "res/textures/p1.png", true, v2(50, 50));
		collider->sprite.wireFrame = true;
		entities.push_back(collider);

		Entity2D* ball = new Entity2D();
		initEntity(ball, "res/textures/p1_.png", true, v2(30, 30));
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
	}

	World::~World()
	{
		for (size_t i = 0; i < entities.size(); ++i)
		{
			delete entities[i];
		}
		delete player;
	}
}