#ifndef MON_ENTITY_H
#define MON_ENTITY_H

#include "mon_collider.h"

namespace Mon {

	// Use this for now
	enum class Direction
	{
		Right,
		Backward,
		Left,
		Forward
	};

	// Get to this starts pg49 Ian Millington
	// TODO(ck): PARTICLE CHANGE TO RIGIDBODY AND FIX PARAMS
	struct RigidBody
	{
		// world position
		v3 pos;
		v3 velocity;
		v3 acceleration;
		v3 orientation;
		float gravity;

		Direction dir = Direction::Forward;

		float damping;
		float inverseMass;

		void integrate(float duration);
		void clearAccumulator();

		// TODO(ck): TEST REMOVE
		float speed;

	};

	struct Component
	{
		int id;
	};

	enum ChargeBarType
	{
		CLASSIC,
		HOLD
	};

	struct ChargeBar : public Component
	{
		float charge;
		ChargeBarType type;
	};

	struct Entity
	{
		const char* name;
		std::string impPath;
		
		// NOTE(ck):
		// Collider and RigidBody should be tied together???
		// The collider is basically the real position which is where the collider will be
		// RenderData holds the actual scale of the model that is being drawn
		// float position info is in here
		RigidBody rb;
		// TODO(ck): The entities need to be driven by a grid and have grid positions
		// this will help me later on with level creation, loading and memory management
		//int gridX; = rb.pos.x / chunk_size (grid_size) ?
		//int gridY; = rb.pos.y / chunk_size (grid_size) ?
		// offsets?

		MonGL::RenderData data;
		MonGL::RenderSetup setup;
		Collider collider;

		Direction facingDir;

		// NOTE(ck): Should this be in the render layer??
		// its not really specific to drawing its just part of the 
		// world matrix?
		float spriteAngleDegrees;
	};

	static void SetFacingDirection(Entity* e)
	{
		if ((e->rb.velocity.x == 0.0f) && (e->rb.velocity.z == 0.0f))
		{
			// NOTE(ck): Leave facingDirection whatever it was 
		}
		else if (absoluteValue(e->rb.velocity.x) > absoluteValue(e->rb.velocity.z))
		{
			if (e->rb.velocity.x > 0)
			{
				e->facingDir = Direction::Right;
			}
			else
			{
				e->facingDir = Direction::Left;
			}
		}
		else
		{
			if (e->rb.velocity.z > 0)
			{
				e->facingDir = Direction::Backward;
			}
			else
			{
				e->facingDir = Direction::Forward;
			}
		}
	}
}

#endif // MON_ENTITY_H