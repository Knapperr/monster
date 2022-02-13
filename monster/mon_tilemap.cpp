#include "mon_tilemap.h"

namespace Mon
{
	int GetTileValueUnchecked(TileMap* map, int tileX, int tileY)
	{
		// Not using chunks right now
		//Assert(tileChunk);
		//Assert(tileX < tileMap->chunkDim);
		//Assert(tileY < tileMap->chunkDim);
		int tileValue = map->tiles[1]->id;
		return tileValue;
	}

	int GetTileValue(TileMap* map, int absTileX, int absTileY, int absTileZ)
	{
		//tile_chunk_position chunkPos = GetChunkPositionFor(tileMap, absTileX, absTileY, absTileZ);
		//tile_chunk* tileChunk = GetTileChunk(tileMap, chunkPos.tileChunkX, chunkPos.tileChunkY, chunkPos.tileChunkZ);
		//int tileChunkValue = GetTileValue(tileMap, tileChunk, chunkPos.relTileX, chunkPos.relTileY);

		return 1;
	}

	int GetTileValue(TileMap* tileMap, TileMapPosition pos)
	{
		//int tileChunkValue = GetTileValue(tileMap, pos.absTileX, pos.absTileY, pos.absTileZ);
		return 1;
	}

	void SetTile(Tile* tile, int tileId, int offsetX, int offsetY)
	{
		tile->id = tileId;
		tile->offsetX = offsetX;
		tile->offsetY = offsetY;
	}

	void SetTile(Tile* tile, Tile* newTile)
	{
		SetTile(tile, newTile->id, newTile->offsetX, newTile->offsetY);
	}


	void InitTileSheet(TileSheet* sheet, const char* fileName)
	{
		MonGL::LoadTextureFile(&sheet->texture, fileName, MonGL::TextureType::Diffuse, true, false, true);

		// TODO(ck): 
		// Parse the loaded texture and calculate the tileids
		// all we need is the width and height i.e 256x256 and the gridX and gridY
		// that will give us our ids for the sheet that we can then use for picking tiles
		// tileid 0 = 0,0 - 1 = 0,1 and so on this info can easily be put into the gui 
		// cause the map can just be used directly in the gui
		// need a way to save the map and all of that as well
		// possibly have a lighter weight tilemap for when the game is actually running 
		// that just reads off of a text file and instantly loads

		// TODO(ck):
		// SPLICE the tilesheet and figure out the offsets manually 
		sheet->tileCount = 18;
		sheet->tiles = new Tile[sheet->tileCount];

		sheet->tiles[0].id = 0; // top
		sheet->tiles[0].offsetX = 4;
		sheet->tiles[0].offsetY = 2;

		sheet->tiles[1].id = 1; // left
		sheet->tiles[1].offsetX = 0;
		sheet->tiles[1].offsetY = 4;

		sheet->tiles[2].id = 2; // right
		sheet->tiles[2].offsetX = 5;
		sheet->tiles[2].offsetY = 5;

		sheet->tiles[3].id = 3; // field
		sheet->tiles[3].offsetX = 1;
		sheet->tiles[3].offsetY = 0;

		sheet->tiles[4].id = 4; // bottom left
		sheet->tiles[4].offsetX = 0;
		sheet->tiles[4].offsetY = 7;

		sheet->tiles[5].id = 5; // bottom right
		sheet->tiles[5].offsetX = 5;
		sheet->tiles[5].offsetY = 7;

		sheet->tiles[6].id = 6; // top right
		sheet->tiles[6].offsetX = 5;
		sheet->tiles[6].offsetY = 2;

		sheet->tiles[7].id = 7; // top left
		sheet->tiles[7].offsetX = 0;
		sheet->tiles[7].offsetY = 2;

		sheet->tiles[8].id = 8; // bottom
		sheet->tiles[8].offsetX = 3;
		sheet->tiles[8].offsetY = 6;


		sheet->tiles[9].id = 9; // bush
		sheet->tiles[9].offsetX = 0;
		sheet->tiles[9].offsetY = 1;

		sheet->tiles[10].id = 10; // rock
		sheet->tiles[10].offsetX = 1;
		sheet->tiles[10].offsetY = 1;

		sheet->tiles[11].id = 11; // road top
		sheet->tiles[11].offsetX = 6;
		sheet->tiles[11].offsetY = 2;

		sheet->tiles[12].id = 12; // road mid
		sheet->tiles[12].offsetX = 6;
		sheet->tiles[12].offsetY = 3;

		sheet->tiles[13].id = 13; // road bot
		sheet->tiles[13].offsetX = 6;
		sheet->tiles[13].offsetY = 5;

		sheet->tiles[14].id = 14; // topleft bush
		sheet->tiles[14].offsetX = 7;
		sheet->tiles[14].offsetY = 2;

		sheet->tiles[15].id = 15; // topright bush
		sheet->tiles[15].offsetX = 8;
		sheet->tiles[15].offsetY = 2;

		sheet->tiles[16].id = 16; // bottomleft bush
		sheet->tiles[16].offsetX = 7;
		sheet->tiles[16].offsetY = 3;

		sheet->tiles[17].id = 17; // bottomright bush
		sheet->tiles[17].offsetX = 8;
		sheet->tiles[17].offsetY = 3;

	}

	// TODO(ck): MEMORY MANAGEMENT
	Tile* TileSheet::createTile(int tileId)
	{
		for (int i = 0; i < tileCount; ++i)
		{
			if (tiles[i].id == tileId)
			{

				// TODO(ck): NOTE(ck): a Tile and a TileSheet can not be the same thing... all of the pointers are pointing to the same x and y coordinates
				// this is because there is only ONE of that kind in the tile sheet so when you update the x and y its updating it to that which is fine for
				// the sheet because we are just using the x and y of the new tile we grabbed it just always sets that old tile to the last one...
				// this could have caused huge bugs later IMPORTANT(ck):
				// newTile.x = x, newTile.y = y
				Tile* newTile = &tiles[i];
				return newTile;
			}
		}
	}

	Tile* TileSheet::getTile(int tileId)
	{
		Tile* newTile = &tiles[tileId];
		return newTile;
	}

	void InitTileMap(TileMap* map, TileSheet* sheet)
	{
		assert(sheet->tileCount > 0);

		map->tileSideInMeters = 1.6f;
		map->tileSideInPixels = 32;
		map->metersToPixels = (float)map->tileSideInPixels / (float)map->tileSideInMeters;


		// NOTE(ck): Sheet must be created first
#define MAP_SIZE 40

	// TODO(ck): Load from file
		int testmap[MAP_SIZE][MAP_SIZE] =
		{
			{7, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
			{1, 2, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			 																														
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 15, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 17, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			 																														
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 11, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 12, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 10, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 13, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 10, 3, 3, 3, 3, 3, 3, 3, 2},
			 																														
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 10, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 10, 3, 10, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			 																														
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 10, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 11, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 13, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			 																														
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 14, 15, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 16, 17, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			 																														
			{1, 3, 3, 3, 3,  3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 10, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 2},
			 																														
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 2},
			{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 9, 3, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 9, 3, 10, 3, 3, 3, 3, 3, 2},
			{4, 8, 8, 8, 8,  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 5},
		};

		int width = 16;
		int height = 16;
		
		for (int y = 0; y < MAP_SIZE; ++y)
		{
			for (int x = 0; x < MAP_SIZE; ++x)
			{
				// this gets the x and y position for the vertice since its screenpos
				// absTileX = screenX * tilesPerWidth + tileX;
				// absTileY = screenY * tilesPerHeight + tileY;


				if (testmap[y][x] != 99)
				{
					// TODO(ck): NOTE(ck): a Tile and a TileSheet can not be the same thing... all of the pointers are pointing to the same x and y coordinates
					// this is because there is only ONE of that kind in the tile sheet so when you update the x and y its updating it to that which is fine for
					// the sheet because we are just using the x and y of the new tile we grabbed it just always sets that old tile to the last one...
					// this could have caused huge bugs later IMPORTANT(ck):
					Tile sheetTile = *sheet->createTile(testmap[y][x]);
					// TODO(ck): Copy constructor
					Tile* newTile = new Tile();
					newTile->id = sheetTile.id;
					newTile->width = sheetTile.width;
					newTile->height = sheetTile.height;
					newTile->offsetX = sheetTile.offsetX;
					newTile->offsetY = sheetTile.offsetY;
					newTile->x = x * width;
					newTile->y = y * width;
					map->tiles.push_back(newTile);
				}
			}
		}

		MonGL::InitTileMap(map->tiles.size());
	}

	void UpdateTile(TileMap* map, TileSheet* sheet, int tileIndex, int newTileId)
	{
		SetTile(map->tiles[tileIndex], sheet->getTile(newTileId));
	}

	void DrawTileMap(TileMap* map, MonGL::CommonProgram* shader, int textureID)
	{
		for (int i = 0; i < map->tiles.size(); ++i)
		{
			MonGL::FillBatch(map->tiles[i]->offsetX, map->tiles[i]->offsetY, map->tiles[i]->x, map->tiles[i]->y, 16);
		}

		MonGL::BindVertices();

		MonGL::DrawMap(shader, textureID);
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
	TileMapPosition RecanonicalizePosition(TileMap* tileMap, TileMapPosition pos)
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