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
		const char* name; 
		// TODO(ck): Subtexture
		// MonGL::SubTexture subTexture;

		// TODO(ck): Fix this. We need this position to be in 
		v2 pos; // currently screen space is used for world position and is sent to render data
		
		v2 velocity;
		v4 colour = v4(1.0f);
		float speed;
		float weight;
		float rotation;
		bool destroyed;
		bool isPlayer;

		int subTextureIndex;
	};

	void InitEntity(Entity2D* e, const char* name, v2 position, int size);
	void InitMinion(Entity2D* e, const char* name, v2 position, int size);
	// TODO(ck): Should this param be pointer?
	// should velocity be acceleration?
	int TestAABB(v3 Amin, v3 Amax, v3 Bmin, v3 Bmax);
	int InterestMovingAABB(AABB a, AABB b, v3 va, v3 vb, float& tFirst, float& tLast);
	void MovePlayer(TileMap* map, Entity2D* p, v2* velocity, float deltaTime);
	void MovePlayer(Entity2D* p, v2* velocity, float deltaTime);

}
#endif