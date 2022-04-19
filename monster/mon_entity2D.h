#ifndef MON_ENTITY2D_H
#define MON_ENTITY2D_H

#include "mon_tilemap.h"


namespace Mon
{
	// TODO(ck): Component? give entities a Movement struct to move
	struct Movement
	{
		v2 pos;
		float speed;
		
	};

	struct Entity2D
	{
		MonGL::RenderData2D sprite;

		// TODO(ck): Fix this. We need this position to be in 
		v2 pos; // currently screen space is used for world position and is sent to render data
		TileMapPosition mapPos;
		
		v2 velocity;
		float speed;
		float maxSpeed;
		float weight;
		float rotation;
		bool destroyed;

	};

	void InitEntity(Entity2D* e, const char* fileLocation, bool isAlpha, v2 position, int size);
	// TODO(ck): Should this param be pointer?
	// should velocity be acceleration?
	void movePlayer(TileMap* map, Entity2D* p, v2* velocity, float deltaTime);

	// "factory" methods for creating entities
	Entity2D* InitPlayer();
	Entity2D* InitEnemy();
}
#endif