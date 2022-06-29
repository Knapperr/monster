#ifndef MON_ENTITY_H
#define MON_ENTITY_H

#include "mon_grid.h"

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
		
		// [06/06/2022]
		// NOTE(ck): I think this is right. similar to handmade hero the tile position is separate from the position used in the physics
		// you have to change the rigidbody and then map that back into cell space just like handmade?
		// Even in handmade casey uses the rigid body physics position for drawing and the tile position is more for world structure and keeping
		// the resolution in tact.
		// this is an interesting realization for me. I feel like this is what casey was trying to convey the whole time.
		CellPosition cellPos;
		RigidBody rb;
		// TODO(ck): The entities need to be driven by a grid and have grid positions
		// this will help me later on with level creation, loading and memory management
		//int gridX; = rb.pos.x / chunk_size (grid_size) ?
		//int gridY; = rb.pos.y / chunk_size (grid_size) ?
		// offsets?

		// TODO(ck): Switch to index so that the render list can be sorted from camera
		MonGL::RenderData data;
		MonGL::RenderSetup setup;
		Collider collider;

		Direction facingDir;

		// NOTE(ck): Should this be in the render layer??
		// its not really specific to drawing its just part of the 
		// world matrix?
		bool follow;
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