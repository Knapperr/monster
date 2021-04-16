#include "mon_world.h"
#include "mon.h"
#include "mon_gl_render.h"

World::World()
{

	// TODO(ck): Efficient tile map creation
	map = new TileMap();
	gl_InitRenderData(&map->sheet.sprite);
	LoadTextureFile(&map->sheet.sprite.texture, "res/textures/test_tile.png", true);
	int width = 32;
	int height = 32;
	for (int y = 0; y < 20; ++y)
	{
		for (int x = 0; x < 20; ++x)
		{
			// this gets the x and y position 
			// tileX 
			// x = screenX * tilesPerWidth + tileX;
			// y = screenY * tilesPerHeight + tileY;

			Tile tile = Tile(x * width, y * height);
			map->tiles.push_back(tile);
		}
	}

	player = new Entity("res/textures/p1.png", true,
								glm::vec2(5, 5), glm::vec2(32, 42), glm::vec3(1.0f, 1.0f, 1.0f),
								200.0f, 0.0f, false);

}

World::~World()
{
	for (size_t i = 0; i < entities.size(); ++i)
	{
		delete entities[i];
	}
	delete player;
}
