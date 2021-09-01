#ifndef MON_ENTITY_H
#define MON_ENTITY_H

#include "mon_math.h"
#include "mon_gl_render.h"

#include <vector>

namespace Mon
{
	class Entity2D
	{
	public:
		v2 pos;
		v2 velocity;
		float speed;
		float maxSpeed;
		float weight;
		float rotation;
		bool destroyed;


		// Components  Sprite can't be part of Default Constructor 
		// default just sets this to nulls
		MonGL::RenderData2D sprite;
		// Texture2D sprite

		// TODO(CK): Maybe worry about this later (remember we jsut want to make a game)
		// IComponent 
		//  - RigidBody 
		//  - Sprite 
		Entity2D(const char* fileLocation,
				 bool isAlpha,
				 v2 position);
		~Entity2D();
	};
}
#endif