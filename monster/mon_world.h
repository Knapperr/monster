#ifndef MON_WORLD_H
#define MON_WORLD_H

#include "mon_entity.h"


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
		height = 32;
		width = 32;
		tileId = 0;
		offsetX = 0;
		offsetY = 0;
	}

	Tile(int id, float x, float y) : x(x), y(y)
	{
		height = 32;
		width = 32;
		tileId = id;
		offsetX = 0;
		offsetY = 0;
	}
};

struct TileSheet
{
	Tile* tiles;
	int tileCount;
	MonTexture::Texture texture;
	int gridX = 32;
	int gridY = 32;

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
	TileSheet sheet;
};

void initTileMap(TileMap* map);


class World
{
public:
	//GameObject* objects[MAX_OBJECTS];

	// Just put the player with the entites and have a getPlayer for it
	Entity* player; // player needs its own class --- it needs to have an array of spells and stuff
	std::vector<Entity*> entities;
	TileMap* map;
	Sprite testTile;

	World();
	~World();

	/*
	dont mind doing something like this
	int player = obj.getplayer();
	if (player > -1)
	{
		obj.entities[player].xp += ss_toi(words[1]);
		obj.entities[player].yp += ss_toi(words[2]);
	
	int entityclass::getplayer()
	{
	//Returns the index of the first player entity
	for (size_t i = 0; i < entities.size(); i++)
	{
		if(entities[i].type==0)
		{
			return i;
		}
	}

	return -1;
}

	*/
};

#endif