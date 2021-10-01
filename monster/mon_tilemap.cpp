#include "mon_tilemap.h"

namespace Mon
{
	void initTileSheet(TileSheet* sheet, const char* fileName)
	{
		MonGL::LoadTextureFile(&sheet->texture, fileName, true, false, true);

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
		sheet->tiles[0].offsetX = 4;
		sheet->tiles[0].offsetY = 1;

		sheet->tiles[1].tileId = 1;
		sheet->tiles[1].offsetX = 4;
		sheet->tiles[1].offsetY = 3;

		sheet->tiles[2].tileId = 2;
		sheet->tiles[2].offsetX = 4;
		sheet->tiles[2].offsetY = 2;

		sheet->tiles[3].tileId = 3;
		sheet->tiles[3].offsetX = 7;
		sheet->tiles[3].offsetY = 3;
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

	void initTileMap(TileMap* map, TileSheet* sheet)
	{
		map->tileSideInMeters = 1.6f;

		// NOTE(ck): Sheet must be created first
#define MAP_SIZE 40

	// TODO(ck): Load from file
		int testmap[MAP_SIZE][MAP_SIZE] =
		{
			{0, 0, 1, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{1, 2, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3},

			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 7, 7, 7, 7, 7, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},

			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 4, 0, 3, 3, 3, 3, 3, 3, 3, 3},

			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},

			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 2, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
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
			{3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
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
					Tile tile = *sheet->createTile(testmap[y][x], x * width, y * height);
					map->tiles.push_back(tile);
				}
			}
		}
	}

	void RecanonicalizeCoord(TileMap* tileMap, uint32_t* tile, float* tileRel)
	{
		// TODO(casey): Need to do something that doesn't use the divide/multiply method
		// for recanonicalizing because this can end up rounding back on to the tile 
		// you just came from.

		// NOTE(casey): tile_map is assumed to be torodial topology, if you step off one end you
		// come back on the other!
		int offset = roundReal32ToInt32(*tileRel / tileMap->tileSideInMeters);
		*tile += offset;
		*tileRel -= offset * tileMap->tileSideInMeters;

		// TODO(casey): Fix floating point math so this can be <
		//Assert(*tileRel >= -0.5f * tileMap->tileSideInMeters);
		//Assert(*tileRel <= 0.5f * tileMap->tileSideInMeters);
	}

	// NOTE(CK):
	// Turn something that has been mutated (had its relative x and y messed with) turn it canonical again
	inline TileMapPosition RecanonicalizePosition(TileMap* tileMap, TileMapPosition pos)
	{
		/* NOTE(CK):
			Because we store the player relative to a tile (in world position)
			we no longer have to worry about changing the player from pixel location
			to world location. We can do this for free now its making the code cleaner
			and way easier to work with.
		*/
		TileMapPosition result = pos;

		RecanonicalizeCoord(tileMap, &result.absTileX, &result.offset.x);
		RecanonicalizeCoord(tileMap, &result.absTileY, &result.offset.y);

		return result;
	}


}