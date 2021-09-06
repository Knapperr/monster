#ifndef MON_ENTITY_H
#define MON_ENTITY_H

#include "mon_math.h"
#include "mon_gl_render.h"

#include <vector>

namespace Mon
{
	struct Entity2D
	{
	public:

		MonGL::RenderData2D sprite;
		v2 pos;
		v2 velocity;
		float speed;
		float maxSpeed;
		float weight;
		float rotation;
		bool destroyed;
	};

	void initEntity(Entity2D* e, const char* fileLocation, bool isAlpha, v2 position);
	// TODO(ck): Should this param be pointer?
	// should velocity be acceleration?
	void movePlayer(Entity2D* p, v2* velocity, float deltaTime);

}
#endif