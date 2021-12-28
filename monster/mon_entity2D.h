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
	public:

		MonGL::RenderData2D sprite;

		v2 pos;
		TileMapPosition mapPos;
		
		v2 velocity;
		float speed;
		float maxSpeed;
		float weight;
		float rotation;
		bool destroyed;

	};

	void initEntity(Entity2D* e, const char* fileLocation, bool isAlpha, v2 position);
	// TODO(ck): Should this param be pointer?
	// should velocity be acceleration?
	void movePlayer(TileMap* map, Entity2D* p, v2* velocity, float deltaTime);

}
#endif