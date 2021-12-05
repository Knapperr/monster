#ifndef MON_ENTITY_H
#define MON_ENTITY_H

#include "mon_math.h"
#include "mon_gl_render.h"

namespace Mon {

	// Use this for now
	enum class Direction
	{
		LEFT,
		RIGHT,
		FORWARD,
		BACKWARD
	};

	// Get to this starts pg49 Ian Millington
	struct Particle
	{
		v3 pos;
		v3 velocity;
		v3 acceleration;
		v3 orientation;
		float gravity;

		Direction dir = Direction::FORWARD;

		float damping;
		float inverseMass;

		void integrate(float duration);
		void clearAccumulator();

		// TODO(ck): TEST REMOVE
		float speed;

	};

	struct Collider
	{
		struct Size
		{
			v3 min = v3(0.0f, 0.0f, 0.0f);
			v3 max = v3(1.0f, 1.0f, 1.0f);;
		};

		MonGL::RenderData data;
		Size size;
	};

	v3 GetSize(Collider* c);
	v3 GetCenter(Collider* c);
	mat4 GetTransform(Collider* c, v3 entityPos, v3 entityScale);


	struct Entity
	{
		std::string name;
		Particle particle;
		MonGL::RenderData data;
		MonGL::RenderSetup setup;
		Collider collider;

		int facingDir;
	};
}
#endif