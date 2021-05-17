#include "mon_world.h"
#include "mon.h"
#include "mon_gl_render.h"

World::World()
{

	// TODO(ck): Efficient tile map creation
	map = new TileMap();
	map->sheet = {};
	const char* files[4];
	files[0] = "res/textures/test_tile.png";
	files[1] = "res/textures/test_tile_1.png";
	files[2] = "res/textures/test_tile_2.png";
	files[3] = "res/textures/test_tile_3.png";
	for (int i = 0; i < ArrayCount(files); ++i)
	{
		Sprite sprite = {};
		MonGL::gl_InitRenderData(&sprite);
		MonGL::LoadTextureFile(&sprite.texture, files[i], true);
		map->sheet.sprites.push_back(sprite);

	}

#define MAP_SIZE 20

	int testmap[MAP_SIZE][MAP_SIZE] =
	{
		{3, 0, 0, 0, 1,  2, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
						 
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
		{0, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},

		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
															 
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
		{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
		{0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3},
	};

	int width = 32;
	int height = 32;
	for (int y = 0; y < MAP_SIZE; ++y)
	{
		for (int x = 0; x < MAP_SIZE; ++x)
		{
			// this gets the x and y position 
			// tileX 
			// x = screenX * tilesPerWidth + tileX;
			// y = screenY * tilesPerHeight + tileY;
			Tile tile = Tile(testmap[y][x], x * width, y * height);
			map->tiles.push_back(tile);
		}
	}

	player = new Entity("res/textures/p1.png", true,
						glm::vec2(5, 5), glm::vec2(32, 32), glm::vec3(1.0f, 1.0f, 1.0f),
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
