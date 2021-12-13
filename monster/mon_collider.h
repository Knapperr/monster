#ifndef MON_COLLIDER_H
#define MON_COLLIDER_H

#include "mon_math.h"
#include "mon_gl_render.h"

namespace Mon {
	struct Collider
	{
		struct Size
		{
			v3 min = v3(0.0f, 0.0f, 0.0f);
			v3 max = v3(1.0f, 1.0f, 1.0f);;
		};

		MonGL::RenderData data;
		v3 worldPos;
		Size size;
	};

	v3 GetSize(Collider* c);
	v3 GetCenter(Collider* c);
	mat4 GetTransform(Collider* c, v3 entityPos, v3 entityScale);
	void SetTransform(Collider* c, v3 entityPos, v3 entityScale);
	void UpdateWorldPosToWorldMatrix(Collider* c);


} // ::Mon

#endif // MON_COLLIDER_H
