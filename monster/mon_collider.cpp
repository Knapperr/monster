#include "mon_collider.h"

namespace Mon {

	v3 GetBoxSize(Collider* c)
	{
		return v3(c->max.x - c->min.x, c->max.y - c->min.y, c->max.z - c->min.z) * c->meshSize;
	}

	v3 GetBoxCenter(Collider* c)
	{
		return v3((c->min.x + c->max.x) / 2, (c->min.y + c->max.y) / 2, (c->min.z + c->max.z) / 2);
	}

	mat4 GetBoxTransform(Collider* c)
	{
		mat4 translateMatrix = mat4(1.0f);
		return translate(translateMatrix, GetBoxCenter(c)) * scale(mat4(1.0f), GetBoxSize(c));
	}

	// Public methods

	void InitBoxCollider(Collider* c)
	{
		c->type = ColliderType::BoundingBox;
		MonGL::SetBoundingBox(&c->data);

		c->meshSize = v3(1.0f);
	}

	void InitBoxCollider(Collider* c, v3 entityPos, v3 entityScale, float meshLength, float meshHeight, float meshWidth)
	{
		c->type = ColliderType::BoundingBox;
		MonGL::SetBoundingBox(&c->data);

		// TODO(ck): Offset(collider, entityPos)
		// use this for calculating min and max 
		// min + entityPos
		// max + entityPos
		c->meshSize = v3(meshLength, meshHeight, meshWidth);
		c->scale = entityScale;
		c->min = v3(entityPos.x - 0.5f, entityPos.y - 0.5f, entityPos.z - 0.5f);
		c->max = v3(entityPos.y + 0.5f, entityPos.y + 0.5f, entityPos.z + 0.5f);
	}

	void SetBoxTransform(Collider* c, v3 entityPos, v3 entityScale)
	{
		c->min = entityPos;
		c->max = entityPos + 1.0f;
		c->scale = entityScale;
		c->data.worldMatrix = GetBoxTransform(c);
		// TODO(ck): We don't need this because we can get the world pos from 
		// the min and max? 
		// we just grab the collider center and thats the world pos
		c->worldPos = c->data.worldMatrix[3];
	}

	// Source: Christer Ericson Realtime collision detection 4.2.1 AABB-AABB Intersection
	int TestAABBAABB(Collider a, Collider b)
	{
		// Exit with no intersection if separated along an axis
		if (a.max.x < b.min.x || a.min.x > b.max.x) return 0;
		if (a.max.z < b.min.z || a.min.z > b.max.z) return 0;
		// NOTE(ck): Check y last because it is the least likely collison
		if (a.max.y < b.min.y || a.min.y > b.max.y) return 0;
		// Overlapping on all axes means AABBs are intersecting
		return 1;
	}


}