#include "mon_world.h"
#include "mon.h"


void initTileSheet(TileSheet* sheet, const char* fileName)
{
	MonTexture::LoadTextureFile(&sheet->texture, fileName, true);

	// TODO(ck): Parse the loaded texture and calculate the tileids
	// all we need is the width and height i.e 256x256 and the gridX and gridY
	// that will give us our ids for the sheet that we can then use for picking tiles
	// tileid 0 = 0,0 - 1 = 0,1 and so on this info can easily be put into the gui 
	// cause the map can just be used directly in the gui
	// need a way to save the map and all of that as well
	// possibly have a lighter weight tilemap for when the game is actually running 
	// that just reads off of a text file and instantly loads
	sheet->tileCount = 4;
	sheet->tiles = new Tile[sheet->tileCount];

	sheet->tiles[0].tileId = 0;
	sheet->tiles[0].offsetX = 6;
	sheet->tiles[0].offsetY = 2;

	sheet->tiles[1].tileId = 1;
	sheet->tiles[1].offsetX = 7;
	sheet->tiles[1].offsetY = 2;

	sheet->tiles[2].tileId = 2;
	sheet->tiles[2].offsetX = 6;
	sheet->tiles[2].offsetY = 2;

	sheet->tiles[3].tileId = 3;
	sheet->tiles[3].offsetX = 7;
	sheet->tiles[3].offsetY = 2;
}

// TODO(ck): MEMORY MANAGEMENT
Tile* TileSheet::createTile(int tileId, int x, int y)
{
	for (int i = 0; i < tileCount; ++i)
	{
		if (tiles[i].tileId == tileId)
		{
			Tile* newTile = &tiles[i];
			newTile->x = x;
			newTile->y = y;
			return newTile;
		}
	}
}


void initTileMap(TileMap* map)
{
	// NOTE(ck): Sheet must be created first
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
			Tile tile = *map->sheet.createTile(testmap[y][x], x * width, y * height);
			map->tiles.push_back(tile);
		}
	}
}

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
		MonGL::fillBatch(i, map->tiles[i].offsetX, map->tiles[i].offsetY, map->tiles[i].x, map->tiles[i].y);
	}



	MonGL::bindVertices();
	
	//const char* files[4];
	//files[0] = "res/textures/test_tile.png";
	//files[1] = "res/textures/test_tile_1.png";
	//files[2] = "res/textures/test_tile_2.png";
	//files[3] = "res/textures/test_tile_3.png";
	//for (int i = 0; i < ArrayCount(files); ++i)
	//{
	//	Sprite sprite = {};
	//	MonGL::gl_InitRenderData(&sprite);
	//	MonGL::LoadTextureFile(&sprite.texture, files[i], true);
	//	map->sheet.sprites.push_back(sprite);

	//}

	//Sprite sheet = {};
	//MonGL::gl_InitRenderData(&sheet);
	//MonGL::LoadTextureFile(&sheet.texture, "res/textures/basic_tiles.png", true);
	//map->sheet.sprites.push_back(sheet);

	//testTile = {};
	//MonGL::gl_InitTile(&testTile, 6, 2);
	//MonGL::LoadTextureFile(&testTile.texture, "res/textures/basic_tiles.png", true);



	/*
	for (int y = 0; y < MAP_SIZE; ++y)
	{
		for (int x = 0; x < MAP_SIZE; ++x)
		{
			// this gets the x and y position 
			// tileX 
			// x = screenX * tilesPerWidth + tileX;
			// y = screenY * tilesPerHeight + tileY;
			Tile tile = {0, 0, 0};
			

			if (testmap[y][x] == 3)
			{
				tile = Tile(testmap[y][x], x * width, y * height);
				MonGL::gl_InitTile(&tile, 6, 2);
			}
			if (testmap[y][x] == 1)
			{
				tile = Tile(testmap[y][x], x * width, y * height);
				MonGL::gl_InitTile(&tile.sprite, 7, 2);
			}
			if (testmap[y][x] == 2)
			{
				tile = Tile(testmap[y][x], x * width, y * height);
				MonGL::gl_InitTile(&tile.sprite, 7, 2);
			}
			if (testmap[y][x] == 0)
			{
				tile = Tile(testmap[y][x], x * width, y * height);
				MonGL::gl_InitTile(&tile.sprite, 7, 2);
			}

			tile.tileId = testTile.texture.id;
			map->tiles.push_back(tile);
		}
	}
	*/
	// TODO(ck): Memory management
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
