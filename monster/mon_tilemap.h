#ifndef MON_TILEMAP_H
#define MON_TILEMAP_H

/*
@file:		mon_tilemap.h
@author:	Cole Fidler-Knapp
@date:		July 27 2022
@desc:		Tile map system is based on Casey Muratori's Handmade Hero series
*/

#include "mon_math.h"
#include "mon_gl_render.h"

#include <vector>


namespace Mon
{
#define TILE_SIZE 16
	
	struct Tile
	{
		float x, y;
		int height, width;
		int textureOffsetX, textureOffsetY;

		// TODO(ck): TILE_PART_ONE - this is linked to mon_world.cpp 
		//file there is a render call using the old sprite method 
		// we will use the tile id in the renderer to decide which 
		// tile from the tile sheet to use
		// The tile should actully just have offsets to draw the right tile from the 
		// texture atlas or tile texture atlas
		int id; // this links to the sprite

		Tile()
		{
			x = 0;
			y = 0;
			height = TILE_SIZE;
			width = TILE_SIZE;
			id = 0;
			textureOffsetX = 0;
			textureOffsetY = 0;
		}

		Tile(int tileId, float x, float y) : x(x), y(y)
		{
			height = TILE_SIZE;
			width = TILE_SIZE;
			id = tileId;
			textureOffsetX = 0;
			textureOffsetY = 0;
		}
	};

	void SetTile(Tile* tile, int tileId, int offsetX, int offsetY, float x, float y, int width, int height);
	void SetTile(Tile* tile);

	struct TileSheet
	{
		Tile* tiles;
		int tileCount;
		MonGL::Texture texture;
		int gridX = TILE_SIZE;
		int gridY = TILE_SIZE;

		Tile* createTile(int tileId);
		Tile* getTile(int tileId);
	};

	void InitTileSheet(TileSheet* sheet, const char* fileName);


	struct TileMap
	{
		std::vector<Tile*> tiles;
		bool wireFrame;
	};

	struct TileMap_
	{
		uint32 chunkShift;
		uint32 chunkMask;
		uint32 chunkDim;

		float tileSideInMeters;

		// size of our world in width and height
		uint32 tileChunkCountX;
		uint32 tileChunkCountY;
		uint32 tileChunkCountZ;

	};

	// New tilemap methods
	void InitTilemap_(TileMap_* map);

	void InitTileMap(TileMap* map);
	void UpdateTile(TileMap* map, TileSheet* sheet, int tileIndex, int newTileId);
}

#endif
