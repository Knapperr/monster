#include "mon_tilemap.h"

namespace Mon
{
#define MAP_SIZE 40

	// TODO(ck): Load from file
	int testmap[MAP_SIZE][MAP_SIZE] =
	{
		{7, 0, 0, 0, 99,  0, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
		{1, 2, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 99, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},

		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 18, 19, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 17, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 15, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},

		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 1, 3, 3, 3, 3, 3, 3, 3, 1, 3, 3, 3, 3, 3, 18, 19, 3,  3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 17, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 3, 3, 14, 15, 3,  3, 3, 3, 3, 3, 3, 1, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 1, 3, 3, 3, 3, 3, 3, 3, 2},

		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 9, 3, 3, 3, 18, 19, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 17, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 15, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 10, 3, 10, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},

		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 9, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 13, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 12, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 11, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},

		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 18, 19, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 16, 17, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 14, 15, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},

		{1, 3, 3, 3, 3,  3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 10, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 2},

		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 2},
		{1, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 10, 3, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 10, 3, 10, 3, 3, 3, 3, 3, 2},
		{4, 8, 8, 8, 8,  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 5},
	};


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
		tile->textureOffsetX = offsetX;
		tile->textureOffsetY = offsetY;
	}

	void SetTile(Tile* tile, Tile* newTile)
	{
		SetTile(tile, newTile->id, newTile->textureOffsetX, newTile->textureOffsetY);
	}


	void InitTileSheet(TileSheet* sheet, const char* fileName)
	{
		// TODO(ck): 
		// Parse the loaded texture and calculate the tileids
		// all we need is the width and height i.e 256x256 and the gridX and gridY
		// that will give us our ids for the sheet that we can then use for picking tiles
		// tileid 0 = 0,0 - 1 = 0,1 and so on this info can easily be put into the gui
		// cause the map can just be used directly in the gui
		// need a way to save the map and all of that as well
		// possibly have a lighter weight tilemap for when the game is actually running 
		// that just reads off of a text file and instantly loads

		// TODO(ck): MEMORY MANAGEMENT
		// TODO(ck): SPLICE the tilesheet and figure out the offsets through code
		// Just have tileCount and Tile not Tile* just have it array like entities?
		sheet->tileCount = 20;
		sheet->tiles = new Tile[sheet->tileCount];

		AssetTile* tile = GetTileAsset(g_Assets, 1, 36);
		sheet->tiles[0].id = 0; // top
		sheet->tiles[0].textureOffsetX = tile->x;
		sheet->tiles[0].textureOffsetY = tile->y;

		sheet->tiles[1].id = 1; // left
		sheet->tiles[1].textureOffsetX = 0;
		sheet->tiles[1].textureOffsetY = 4;

		sheet->tiles[2].id = 2; // right
		sheet->tiles[2].textureOffsetX = 5;
		sheet->tiles[2].textureOffsetY = 5;

		sheet->tiles[3].id = 3; // field
		sheet->tiles[3].textureOffsetX = 1;   // 1
		sheet->tiles[3].textureOffsetY = 11;  // 11

		sheet->tiles[4].id = 4; // bottom left
		sheet->tiles[4].textureOffsetX = 0;
		sheet->tiles[4].textureOffsetY = 7;

		sheet->tiles[5].id = 5; // bottom right
		sheet->tiles[5].textureOffsetX = 5;
		sheet->tiles[5].textureOffsetY = 7;

		sheet->tiles[6].id = 6; // top right
		sheet->tiles[6].textureOffsetX = 5;
		sheet->tiles[6].textureOffsetY = 2;

		sheet->tiles[7].id = 7; // top left
		sheet->tiles[7].textureOffsetX = 0;
		sheet->tiles[7].textureOffsetY = 2;

		sheet->tiles[8].id = 8; // bottom
		sheet->tiles[8].textureOffsetX = 3;
		sheet->tiles[8].textureOffsetY = 6;


		sheet->tiles[9].id = 9; // bush
		sheet->tiles[9].textureOffsetX = 3;
		sheet->tiles[9].textureOffsetY = 11;

		sheet->tiles[10].id = 10; // rock
		sheet->tiles[10].textureOffsetX = 1;
		sheet->tiles[10].textureOffsetY = 14;

		sheet->tiles[11].id = 11; // road top
		sheet->tiles[11].textureOffsetX = 6;
		sheet->tiles[11].textureOffsetY = 13;

		sheet->tiles[12].id = 12; // road mid
		sheet->tiles[12].textureOffsetX = 6;
		sheet->tiles[12].textureOffsetY = 12;

		sheet->tiles[13].id = 13; // road bot
		sheet->tiles[13].textureOffsetX = 6;
		sheet->tiles[13].textureOffsetY = 11;

		AssetTile* tltree = GetTileAsset(g_Assets, 1, 215);
		sheet->tiles[14].id = 14; // topleft tree
		sheet->tiles[14].textureOffsetX = tltree->x;
		sheet->tiles[14].textureOffsetY = tltree->y;

		sheet->tiles[15].id = 15; // topright tree
		sheet->tiles[15].textureOffsetX = 8;
		sheet->tiles[15].textureOffsetY = 13;

		sheet->tiles[16].id = 16; // midleft tree
		sheet->tiles[16].textureOffsetX = 7;
		sheet->tiles[16].textureOffsetY = 11;

		sheet->tiles[17].id = 17; // midright tree
		sheet->tiles[17].textureOffsetX = 8;
		sheet->tiles[17].textureOffsetY = 11;

		sheet->tiles[18].id = 18; // bottomleft tree
		sheet->tiles[18].textureOffsetX = 7;
		sheet->tiles[18].textureOffsetY = 11;

		sheet->tiles[19].id = 19; // bottomright tree
		sheet->tiles[19].textureOffsetX = 8;
		sheet->tiles[19].textureOffsetY = 11;
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

	void InitTileMap(TileMap* map)
	{		
		for (int y = 0; y < MAP_SIZE - 1; ++y)
		{
			for (int x = 0; x < MAP_SIZE - 1; ++x)
			{
				if (testmap[y][x] != 99)
				{

					Tile sheetTile = Tile();
					int tileIndex = testmap[y][x];
					sheetTile.id = testmap[y][x];
					
					if (testmap[y][x] == 1)
					{
						sheetTile.textureOffsetX = 3; // 3 - is grass
						sheetTile.textureOffsetY = 11; // field,grass,

					}
					else if (testmap[y][x] == 3)
					{
						sheetTile.textureOffsetX = 1; // 1 - is field
						sheetTile.textureOffsetY = 11; // field,grass,
					}
					else if (testmap[y][x] == 18) // top left tree
					{
						sheetTile.textureOffsetX = 7; 
						sheetTile.textureOffsetY = 11;
					}
					else if (testmap[y][x] == 19) // top right tree
					{
						sheetTile.textureOffsetX = 8; 
						sheetTile.textureOffsetY = 11;
					}
					else if (testmap[y][x] == 16) // mid left tree
					{
						sheetTile.textureOffsetX = 7; 
						sheetTile.textureOffsetY = 12; 
					}
					else if (testmap[y][x] == 17) // mid right tree
					{
						sheetTile.textureOffsetX = 8; 
						sheetTile.textureOffsetY = 12;
					}
					else if (testmap[y][x] == 14) // bot left tree
					{
						sheetTile.textureOffsetX = 7; 
						sheetTile.textureOffsetY = 13;
					}
					else if (testmap[y][x] == 15) // bot right tree
					{
						sheetTile.textureOffsetX = 8; 
						sheetTile.textureOffsetY = 13;
					}
					else
					{
						sheetTile.textureOffsetX = 1; // 1 - is field
						sheetTile.textureOffsetY = 11; // field,grass,
					}

					//Tile sheetTile = *sheet->createTile(testmap[y][x]);
					// TODO(ck): Copy constructor - MEMORY MANAGEMENT
					Tile* newTile = new Tile();
					newTile->id = sheetTile.id;
					newTile->width = sheetTile.width;
					newTile->height = sheetTile.height;
					newTile->textureOffsetX = sheetTile.textureOffsetX;
					newTile->textureOffsetY = sheetTile.textureOffsetY;
					newTile->x = (float)x;
					newTile->y = (float)y;
					map->tiles.push_back(newTile);
				}
			}
		}

		map->wireFrame = false;
	}

	void UpdateTile(TileMap* map, TileSheet* sheet, int tileIndex, int newTileId)
	{
		SetTile(map->tiles[tileIndex], sheet->getTile(newTileId));
	}


	///
	/// New tile map methods
	///

	void InitTilemap_(TileMap_* map)
	{
		// allocate the tilemap
		// allocate the tilemap chunks
		for (int y = 0; y < MAP_SIZE - 1; ++y)
		{
			for (int x = 0; x < MAP_SIZE - 1; ++x)
			{
				if (testmap[y][x] != 99)
				{
					// Init tile to 

					// Set Tile value 

					//Tile sheetTile = Tile();
					//sheetTile.id = 3;
					//sheetTile.textureOffsetX = 1;
					//sheetTile.textureOffsetY = 11;
					////Tile sheetTile = *sheet->createTile(testmap[y][x]);
					//// TODO(ck): Copy constructor - MEMORY MANAGEMENT
					//Tile* newTile = new Tile();
					//newTile->id = sheetTile.id;
					//newTile->width = sheetTile.width;
					//newTile->height = sheetTile.height;
					//newTile->textureOffsetX = sheetTile.textureOffsetX;
					//newTile->textureOffsetY = sheetTile.textureOffsetY;
					//newTile->x = (float)x;
					//newTile->y = (float)y;
					//map->tiles.push_back(newTile);
				}
			}
		}

		return;
	}

}