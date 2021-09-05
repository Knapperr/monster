#include "mon_tilemap.h"

namespace Mon
{
	void initTileSheet(TileSheet* sheet, const char* fileName)
	{
		MonTexture::LoadTextureFile(&sheet->texture, fileName, true, false, true);

		// TODO(ck): 
		// Parse the loaded texture and calculate the tileids
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
		sheet->tiles[0].offsetY = 1;

		sheet->tiles[1].tileId = 1;
		sheet->tiles[1].offsetX = 6;
		sheet->tiles[1].offsetY = 3;

		sheet->tiles[2].tileId = 2;
		sheet->tiles[2].offsetX = 6;
		sheet->tiles[2].offsetY = 2;

		sheet->tiles[3].tileId = 3;
		sheet->tiles[3].offsetX = 6;
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
#define MAP_SIZE 40

	// TODO(ck): Load from file
		int testmap[MAP_SIZE][MAP_SIZE] =
		{
			{0, 0, 1, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{1, 2, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 2, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3},

			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 7, 7, 7, 7, 7, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 7, 7, 7, 7, 7, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},

			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 4, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 4, 0, 3, 3, 3, 3, 3, 3, 3, 3},

			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 3, 3, 3, 3, 3},
			{1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

			{0, 0, 1, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{1, 2, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 2, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3},

			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 7, 7, 7, 7, 7, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 7, 7, 7, 7, 7, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},

			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 4, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 4, 0, 3, 3, 3, 3, 3, 3, 3, 3},

			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 3, 3, 3, 3, 3},
			{1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		};

		int width = 16;
		int height = 16;
		for (int y = 0; y < MAP_SIZE; ++y)
		{
			for (int x = 0; x < MAP_SIZE; ++x)
			{
				// this gets the x and y position for the vertice since its screenpos
				// x = screenX * tilesPerWidth + tileX;
				// y = screenY * tilesPerHeight + tileY;


				if (testmap[y][x] != 4 && testmap[y][x] != 7)
				{
					Tile tile = *map->sheet.createTile(testmap[y][x], x * width, y * height);
					map->tiles.push_back(tile);
				}
			}
		}
	}

}