#ifndef MON_COLLIDER_H
#define MON_COLLIDER_H

#include "mon_math.h"
#include "mon_gl_render.h"

namespace Mon {

	enum class ColliderType
	{
		BoundingBox,
		Plane,
		Sphere
	};

	struct Collider
	{
		struct Size
		{
			v3 min = v3(0.0f, 0.0f, 0.0f);
			v3 max = v3(1.0f, 1.0f, 1.0f);;
		};

		ColliderType type;
		MonGL::RenderData data;
		v3 worldPos;
		Size size;
	};

	void InitBoxCollider(Collider* c);
	v3 GetBoxSize(Collider* c);
	v3 GetBoxCenter(Collider* c);
	mat4 GetBoxTransform(Collider* c, v3 entityPos, v3 entityScale);
	void SetBoxTransform(Collider* c, v3 entityPos, v3 entityScale);

	void UpdateWorldPosToWorldMatrix(Collider* c);


} // ::Mon

#endif // MON_COLLIDER_H
