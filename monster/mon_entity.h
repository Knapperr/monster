#ifndef MON_ENTITY_H
#define MON_ENTITY_H

#include "mon_grid.h"

namespace Mon {


	enum EntityRenderFlag
	{
		Sprite = 0x1,
		Model = 0x2,
		Light = 0x4,
	};

	// Use this for now
	enum class Direction
	{
		Right,
		Backward,
		Left,
		Forward
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

	struct Light : public Component
	{
		// Index into the renderers lighting list
		int lightIndex;
	};


	// Get to this starts pg49 Ian Millington
	// TODO(ck): PARTICLE CHANGE TO RIGIDBODY AND FIX PARAMS
	struct RigidBody
	{
		v3 worldPos; // should this be CellPosition?
		v3 velocity;
		v3 acceleration;
		v3 orientation;
		float gravity;

		Direction dir = Direction::Forward;

		float damping;
		float inverseMass;

		void integrate(float duration);
		void clearAccumulator();

		float speed;
	};

	struct Entity
	{
		const char* name;
		std::string impPath;
		uint32 flags;
		bool isPlayer;
		// CellPosition is specific to the grid and editor?
		// Should we use this to save and load the map data?
		// position gets updated each frame?
		// maybe we dont even need this it should be something that 
		// isn't part of the entity we just check RigidBody position against
		// grid and save/load that way?
		// i think we need cell positions for path finding and making other things easier
		// that way we can iterate over the grid easier? it at least gives us a way to 
		// iterate over the world. without some kind of space how do we query it?
		CellPosition cellPos;
		RigidBody rb;
		// TODO(ck): The entities need to be driven by a grid and have grid positions
		// this will help me later on with level creation, loading and memory management
		//int gridX; = rb.pos.x / chunk_size (grid_size) ?
		//int gridY; = rb.pos.y / chunk_size (grid_size) ?
		// offsets?

		// TODO(ck): Switch to index so that the render list can be sorted from camera
		// and transparent objects need to be sorted... maybe build list of mesh indexes
		// to send through and sort from camera
		MonGL::RenderData data;
		MonGL::RenderSetup setup;
		// subtexture indexes?
		Collider collider;

		Direction facingDir;

		// NOTE(ck): Should this be in the render layer??
		// its not really specific to drawing its just part of the 
		// world matrix?
		bool follow;
		float spriteAngleDegrees;

		int spriteAnimationIndex;
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