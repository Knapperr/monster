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

	int InterestMovingAABB(AABB a, AABB b, v3 va, v3 vb, float& tFirst, float& tLast)
	{
		if (TestAABB(a.min, a.max, b.min, b.max))
		{
			tFirst = tLast = 0.0f;
			return 1;
		}

		// constant velocities va, vb
		// relative velocity. treat 'a' as stationary
		v2 v = vb - va;
		tFirst = 0.0f;
		tLast = 1.0f;

		for (int i = 0; i < 2; i++)
		{
			if (v[i] < 0.0f)
			{
				if (b.max[i] < a.min[i]) return 0;
				// Nonintersecting and moving apart
				if (a.max[i] < b.min[i]) tFirst = std::max((a.max[i] - b.min[i]) / v[i], tFirst);
				if (b.max[i] > a.min[i]) tLast = std::min((a.min[i] - b.max[i]) / v[i], tLast);
			}
			if (v[i] > 0.0f)
			{
				if (b.min[i] > a.max[i]) return 0;
				// Nonintersecting and moving apart
				if (b.max[i] < a.min[i]) tFirst = std::max((a.min[i] - b.max[i]) / v[i], tFirst);
				if (a.max[i] > b.min[i]) tLast = std::min((a.max[i] - b.min[i]) / v[i], tLast);
			}
			// No overlap possible if time of first contact occurs after time of last contact
			if (tFirst > tLast) return 0;
		}
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

		v2 oldPos = p->pos;
		//oldPos.x = p->position.x;
		//oldPos.y = p->position.y;
		
		v2 newPos = oldPos;
		v2 delta = (0.5f * (*velocity) * square(deltaTime) + p->velocity * deltaTime);
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