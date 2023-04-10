#include "mon_collider.h"

namespace Mon {

	v3 GetBoxSize(Collider* c)
	{
		return v3(c->max.x - c->min.x, c->max.y - c->min.y, c->max.z - c->min.z);
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

	// Public
	void InitBoxCollider(Collider* c)
	{
		c->type = ColliderType::BoundingBox;
		MonGL::SetBoundingBox(&c->data);

		c->meshSize = v3(1.0f);
		c->extents = c->meshSize * 0.5f;
	}

	void InitBoxCollider(Collider* c, v3 entityPos, v3 entityScale, v3 meshSize)
	{
		c->type = ColliderType::BoundingBox;
		MonGL::SetBoundingBox(&c->data);

		c->min = v3(-0.5f);
		c->max = v3(0.5f);
		c->meshSize = meshSize;
		c->extents = meshSize * 0.5f;
	}

	void SetBoxTransform(Collider *c, v3 entityPos, v3 entityScale)
	{
		c->min = (entityPos) - c->extents * entityScale;
		c->max = (entityPos) + c->extents * entityScale;
		c->data.worldMatrix = GetBoxTransform(c);
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

	int TestAABBAABB(Collider* a, Collider* b)
	{
		a->data.color = v3(0.0f, 0.0f, 1.0f);
		b->data.color = v3(1.0f, 0.0f, 0.0f);

		// Exit with no intersection if separated along an axis
		if (a->max.x < b->min.x || a->min.x > b->max.x) {
			a->data.color = v3(0.7f, 0.15f, 0.4f);
			b->data.color = v3(0.7f, 0.15f, 0.4f);
			return 0;
		}
		if (a->max.z < b->min.z || a->min.z > b->max.z) {
			a->data.color = v3(0.7f, 0.15f, 0.4f);
			b->data.color = v3(0.7f, 0.15f, 0.4f);
			return 0;
		}
		// NOTE(ck): Check y last because it is the least likely collison
		if (a->max.y < b->min.y || a->min.y > b->max.y) {
			a->data.color = v3(0.7f, 0.15f, 0.4f);
			b->data.color = v3(0.7f, 0.15f, 0.4f);
			return 0;
		}
		// Overlapping on all axes means AABBs are intersecting
		return 1;
	}

}