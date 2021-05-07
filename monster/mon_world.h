#ifndef MON_WORLD_H
#define MON_WORLD_H

#include "mon_entity.h"
#include <vector>

struct Tile
{
	float x, y;
	int height, width;
	
	// TODO(ck): TILE_PART_ONE - this is linked to mon_world.cpp 
	//file there is a render call using the old sprite method 
	// we will use the tile id in the renderer to decide which 
	// tile from the tile sheet to use
	int tileId; // this links to the sprite
	//Sprite sprite;

	Tile(int id, float x, float y) : x(x), y(y)
	{
		height = 32;
		width = 32;
		tileId = id;
		//sprite = {};
	}
};

// TODO(ck): tileId needs to map to the tilesheet
struct TileSheet
{
	std::vector<Sprite> sprites;
};

struct TileMap
{
	std::vector<Tile> tiles;
	TileSheet sheet;
};


class World
{
public:
	//GameObject* objects[MAX_OBJECTS];

	// Just put the player with the entites and have a getPlayer for it
	Entity* player; // player needs its own class --- it needs to have an array of spells and stuff
	std::vector<Entity*> entities;
	TileMap* map;

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