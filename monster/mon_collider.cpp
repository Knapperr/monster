#include "mon_collider.h"

namespace Mon {

	void InitBoxCollider(Collider* c)
	{
		c->type = ColliderType::BoundingBox;
		MonGL::InitBoundingBox(&c->data);
	}

	void InitBoxCollider(Collider* c, v3 entityPos)
	{
		c->type = ColliderType::BoundingBox;
		MonGL::InitBoundingBox(&c->data);
		
		// TODO(ck): Offset(collider, entityPos)
		// use this for calculating min and max 
		// min + entityPos
		// max + entityPos

		c->min = v3(entityPos.x - 0.5f, entityPos.y - 0.5f, entityPos.z - 0.5f);
		c->max = v3(entityPos.y + 0.5f, entityPos.y + 0.5f, entityPos.z + 0.5f);


	}

	//v3 GetBoxSize(Collider* c)
	//{
	//	return v3(c->size.max.x - c->size.min.x, c->size.max.y - c->size.min.y, c->size.max.z - c->size.min.z);
	//}

	v3 GetBoxSize(Collider* c, v3 scale)
	{
		return v3(c->max.x - c->min.x, c->max.y - c->min.y, c->max.z - c->min.z) * scale;
	}

	//v3 GetBoxCenter(Collider* c)
	//{
	//	return v3((c->size.min.x + c->size.max.x) / 2, (c->size.min.y + c->size.max.y) / 2, (c->size.min.z + c->size.max.z) / 2);
	//}

	v3 GetBoxCenter(Collider* c)
	{
		return v3((c->min.x + c->max.x) / 2, (c->min.y + c->max.y) / 2, (c->min.z + c->max.z) / 2);
	}

	mat4 GetBoxTransform(Collider* c, v3 entityPos, v3 entityScale)
	{
		// TODO(ck): need collision offset that can be edited in the gui and saved to the entity
		c->max = entityPos + 1.0f;
		c->min = entityPos;
		mat4 translateMatrix = mat4(1.0f);
		entityScale = entityScale / 2.0f;
		return translate(translateMatrix, GetBoxCenter(c)) * scale(mat4(1.0f), GetBoxSize(c, entityScale));
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

	// Set RenderMatrix 
	void SetBoxTransform(Collider* c, v3 entityPos, v3 entityScale)
	{
		c->data.worldMatrix = GetBoxTransform(c, entityPos, entityScale);
	}
	
	
	void UpdateWorldPosToWorldMatrix(Collider* c)
	{
		c->worldPos = c->data.worldMatrix[3];
	}
}