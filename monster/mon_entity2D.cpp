#include "mon_entity2D.h"

namespace Mon
{
	void InitEntity(Entity2D* e, const char* name, v2 position, int size)
	{
		e->name = name;
		e->sprite = {};
		// TODO(ck): Don't automatically make opengl data... figure out what renderer we are using
		// TODO(ck): Texture load in renderer we need the sprite size from the texture
		MonGL::InitRenderData2D(&e->sprite, size);

		e->sprite.meshIndex = 1;
		e->sprite.textureIndex = 16;
		e->pos = position;
		
		// TODO(ck): Update this is a Point now not a v2
		e->sprite.pos = {};
		e->sprite.pos.x = position.x;
		e->sprite.pos.y = position.y;


		
		e->speed = 10.0f;
		e->velocity = v2(0.0f);
		
		e->rotation = 0.0f;
		e->destroyed = false;

		e->weight = 44.0f;

		e->sprite.wireFrame = false;
	}

	void InitMinion(Entity2D* e, const char* name, v2 position, int size)
	{
		InitEntity(e, name, position, size);

		e->speed = 10.0f;
		e->velocity = v2(0.0f);
		e->weight = 44.0f;
		e->sprite.textureIndex = 9;
		e->sprite.wireFrame = false;
	}

	// TODO(ck): Should this param be pointer?
	// should velocity be acceleration?
	void MovePlayer(TileMap* map, Entity2D* p, v2* velocity, float deltaTime)
	{
		// ddPLength
		float velocityLength = lengthSq(*velocity);
		if (velocityLength > 1.0f)
		{
			*velocity *= (1.0f / squareRoot(velocityLength));
		}

		*velocity *= p->speed;

		*velocity += -5.0f * p->velocity;

		//TileMapPosition oldPlayerP = p->mapPos;
		//TileMapPosition newPlayerP = oldPlayerP;

		v2 oldPos = p->pos;
		//oldPos.x = p->position.x;
		//oldPos.y = p->position.y;
		
		v2 newPos = oldPos;
		float deltaX = (0.5f * velocity->x * square(deltaTime) + p->velocity.x * deltaTime);
		float deltaY = (0.5f * velocity->y * square(deltaTime) + p->velocity.y * deltaTime);
		v2 delta = { deltaX, deltaY };

		// TODO(ck): need to set an offest like casey does
		//newPlayerP.offset += delta;
		newPos += delta;
		
		p->velocity.x = velocity->x * deltaTime + p->velocity.x;
		p->velocity.y = velocity->y * deltaTime + p->velocity.y;

		p->pos = newPos;
		
	}



}