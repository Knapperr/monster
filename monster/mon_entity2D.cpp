#include "mon_entity2D.h"

namespace Mon
{
	void InitEntity(Entity2D* e, const char* name, v2 position, int size)
	{
		e->name = name;

		e->pos = position;
		e->speed = 50.0f;
		e->velocity = v2(0.0f);
		e->rotation = 0.0f;
		e->destroyed = false;
		e->weight = 44.0f;
	}

	void InitMinion(Entity2D* e, const char* name, v2 position, int size)
	{
		InitEntity(e, name, position, size);

		e->speed = 30.0f;
		e->velocity = v2(0.0f);
		e->weight = 44.0f;
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

		*velocity += -8.0f * p->velocity;

		//TileMapPosition oldPlayerP = p->mapPos;
		//TileMapPosition newPlayerP = oldPlayerP;

		v2 oldPos = p->pos;
		//oldPos.x = p->position.x;
		//oldPos.y = p->position.y;
		
		v2 newPos = oldPos;
		v2 delta = (0.5f * (*velocity) * square(deltaTime) + p->velocity * deltaTime);
		// TODO(ck): need to set an offest like casey does
		//newPlayerP.offset += delta;
		newPos += delta;
		
		p->velocity.x = velocity->x * deltaTime + p->velocity.x;
		p->velocity.y = velocity->y * deltaTime + p->velocity.y;

		p->pos = newPos;
	}

}