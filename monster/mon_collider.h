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
		v3 worldPos;
		v3 min;
		v3 max;
	};

	void InitBoxCollider(Collider* c);

	// TODO(ck): Remove world position from collider... only need the min, max size in relation to entity
	mat4 GetBoxTransform(Collider* c, v3 entityPos, v3 entityScale);
	void SetBoxTransform(Collider* c, v3 entityPos, v3 entityScale);
	void UpdateWorldPosToWorldMatrix(Collider* c);

	// intersection tests
	int TestAABBAABB(Collider a, Collider b);


}

#endif // MON_COLLIDER_H
