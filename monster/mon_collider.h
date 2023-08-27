#ifndef MON_COLLIDER_H
#define MON_COLLIDER_H

#include "mon_math.h"
#include "mon_gl_render.h"

namespace Mon {

	enum class ColliderType
	{
		BoundingBox,
		Plane,
		Sphere,
		Rectangle
	};

	struct Collider
	{
		ColliderType type;
		MonGL::RenderData data;
		v3 min = {};
		v3 max = {};
		v3 meshSize = {};
		v3 extents = {};
	};

	void InitBoxCollider(Collider* c);
	void InitBoxCollider(Collider* c, v3 entityPos, v3 entityScale, v3 meshSize);
	void SetBoxTransform(Collider* c, v3 entityPos, v3 entityScale);

	// intersection tests
	int TestAABBAABB(Collider a, Collider b);
	// Modify collider inside test
	int TestAABBAABB(Collider *a, Collider *b);


}

#endif // MON_COLLIDER_H
