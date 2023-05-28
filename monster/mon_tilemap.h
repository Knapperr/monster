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
	struct TileMapDifference
	{
		v2 dXY;
		float dZ;
	};

	struct TileMapPosition
	{
		int32 absTileX;
		int32 absTileY;
		int32 absTileZ;
		// NOTE(casey): These are the offsets from the tile center
		v2 offset;
	};
	
	struct TileChunkPosition
	{
		int32 tileChunkX;
		int32 tileChunkY;
		int32 tileChunkZ;
		
		int32 relTileX;
		int32 relTileY;
	};

	struct TileChunk
	{
		uint32* tiles;
	};

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
		// this is the tile and the ones that get created for
		// everytthing else the tile sheet creates tiles for the
		// map that way we can also manipulate the tile sheet
		// 	in the gui for our loaded texture and info about the sheet like 
		// 	   lengths of ids and 
		// 
		//Tile[] tiles;

		//texture
		//info
		//	this is calculated by parsing the loaded texture
		//	we get an id for each tile 
		//	ids (1, 2, 3, 4 ,5 )
	};

	void InitTileSheet(TileSheet* sheet, const char* fileName);


	struct TileMap
	{
		//MonGL::BatchData* batch;
		std::vector<Tile*> tiles;
		MonGL::RenderData2D sprite;
		float tileSideInMeters;
		float metersToPixels;
		int tileSideInPixels;

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

		TileChunk* chunks;
	};

	TileChunk* GetTileChunk(TileMap_* tileMap, int32 tileChunkX, int32 tileChunkY, int32 tileChunkZ);
	uint32 GetTileValueUnchecked(TileMap_ *tileMap, TileChunk *tileChunk, int32 tileX, int32 tileY);
	void SetTileValueUnchecked(TileMap_ *tileMap, TileChunk *tileChunk, int32 tileX, int32 tileY);
	
	void SetTileValue(TileMap_* tileMap, TileChunk* tileChunk, int32 testTileX, int32 testTileY, int32 tileValue);
	void GetChunkPositionFor(TileMap_ *tileMap, int32 absTileX, int32 absTileY, int32 absTileZ);
	uint32 GetTileValue(TileMap_ *tileMap, TileChunk *tileChunk, int32 testTileX, int32 testTileY);
	uint32 GetTileValue(TileMap_ *tileMap, int32 absTileX, int32 absTileY, int32 absTileZ);
	uint32 GetTileValue(TileMap_* tileMap, TileMapPosition pos);
	bool IsTileValueEmpty(int32 tileValue);
	bool IsTileMapPointEmpty(TileMap_* tileMap, TileMapPosition);
	void SetTileValue(TileMap *tileMap,
				int32 absTileX, int32 absTileY, int32 absTileZ,
				int32 tileValue);
	//void SetTileValue(MemoryArena *arena, TileMap *tileMap,
	//				uint32 absTileX, uint32 absTileY, uint32 absTileZ,
	//				uint32 tileValue);

	void InitTileMap(TileMap* map, TileSheet* sheet);
	TileMapPosition RecanonicalizePosition(TileMap* tileMap, TileMapPosition pos);
	void UpdateTile(TileMap* map, TileSheet* sheet, int tileIndex, int newTileId);
}

#endif
