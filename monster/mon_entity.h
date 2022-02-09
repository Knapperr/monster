#ifndef MON_ENTITY_H
#define MON_ENTITY_H

#include "mon_collider.h"

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
	// TODO(ck): PARTICLE CHANGE TO RIGIDBODY AND FIX PARAMS
	struct RigidBody
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

	struct Entity
	{
		std::string name;
		std::string impPath;
		RigidBody rb;
		MonGL::RenderData data;
		MonGL::RenderSetup setup;
		Collider collider;

		
		int facingDir;
	};
}

#endif // MON_ENTITY_H