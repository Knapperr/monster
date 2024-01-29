#include "mon_entity2D.h"

namespace Mon
{
	void InitEntity(Entity2D* e, const char* name, v2 position, int size)
	{
		e->name = name;

		e->pos = position;
		e->speed = 6.5f;
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
	int TestAABB(v3 Amin, v3 Amax, v3 Bmin, v3 Bmax)
	{
		if (Amax.x < Bmin.x || Amin.x > Bmax.x) return 0;
		//if (Amax.z < Bmin.z || Amin.z > Bmax.z) return 0;
		// NOTE(ck): Check y last because it is the least likely collison
		if (Amax.y < Bmin.y || Amin.y > Bmax.y) return 0;
		// Overlapping on all axes means AABBs are intersecting
		return 1;
	}

	void MovePlayer(TileMap* map, Entity2D* p, v2* velocity, float deltaTime)
	{
		// ddPLength
		float velocityLength = lengthSq(*velocity);
		if (velocityLength > 1.0f)
		{
			*velocity *= (1.0f / squareRoot(velocityLength));
		}

		*velocity *= p->speed;

		*velocity += -9.0f * p->velocity;

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

	void MovePlayer(Entity2D* p, v2* velocity, float deltaTime)
	{

		p->pos.x += (velocity->x * p->speed * deltaTime);
		p->pos.y += (velocity->y * p->speed * deltaTime);

	}


}