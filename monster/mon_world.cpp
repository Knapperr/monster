#include "mon_world.h"

namespace Mon
{
	World::World()
	{

		// TODO(ck): MEMORY MANAGEMENT - Efficient tile map creation
		map = new TileMap();
		map->sheet = {};
		initTileSheet(&map->sheet, "res/textures/basic_tiles.png");

		initTileMap(map);
		MonGL::initTileMap(map->tiles.size());

		for (int i = 0; i < map->tiles.size(); ++i)
		{
			MonGL::fillBatch(map->tiles[i].offsetX, map->tiles[i].offsetY, map->tiles[i].x, map->tiles[i].y, 16);
		}

		MonGL::bindVertices();


		// TODO(ck): Memory management
		player = new Mon::Entity2D("res/textures/p1.png", true,
								   glm::vec2(0, 0));

		Entity2D* ball = new Entity2D("res/textures/awesomeface.png", true,
									  glm::vec2(200, 200));
		entities.push_back(ball);
		for (int i = 0; i < 5; ++i)
		{
			Entity2D* e = new Entity2D("res/textures/awesomeface.png", true,
									   glm::vec2(30 * i, 5 * i));
			entities.push_back(e);
		}
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