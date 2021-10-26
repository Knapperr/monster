#ifndef MON_TILEMAP_H
#define MON_TILEMAP_H

#include "mon_math.h"
#include "mon_gl_render.h"

#include <vector>

namespace Mon
{
#define TILE_SIZE 16

	struct TileMapPosition
	{
		uint32_t absTileX;
		uint32_t absTileY;
		// NOTE(casey): These are the offsets from the tile center
		v2 offset;
	};

	struct Tile
	{
		float x, y;
		int height, width;
		int offsetX, offsetY;


		// TODO(ck): TILE_PART_ONE - this is linked to mon_world.cpp 
		//file there is a render call using the old sprite method 
		// we will use the tile id in the renderer to decide which 
		// tile from the tile sheet to use
		// The tile should actully just have offsets to draw the right tile from the 
		// texture atlas or tile texture atlas
		int tileId; // this links to the sprite

		Tile()
		{
			x = 0;
			y = 0;
			height = TILE_SIZE;
			width = TILE_SIZE;
			tileId = 0;
			offsetX = 0;
			offsetY = 0;
		}

		Tile(int id, float x, float y) : x(x), y(y)
		{
			height = TILE_SIZE;
			width = TILE_SIZE;
			tileId = id;
			offsetX = 0;
			offsetY = 0;
		}
	};

	struct TileSheet
	{
		Tile* tiles;
		int tileCount;
		MonGL::Texture texture;
		int gridX = TILE_SIZE;
		int gridY = TILE_SIZE;

		Tile* createTile(int tileId, int x, int y);

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

	void initTileSheet(TileSheet* sheet, const char* fileName);

	struct TileMap
	{
		//MonGL::BatchData* batch;
		std::vector<Tile> tiles;
		float tileSideInMeters;
		float metersToPixels;
		int tileSideInPixels;
	};

	void initTileMap(TileMap* map, TileSheet* sheet);
	TileMapPosition RecanonicalizePosition(TileMap* tileMap, TileMapPosition pos);
}

#endif
