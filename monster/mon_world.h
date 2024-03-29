#ifndef MON_WORLD_H
#define MON_WORLD_H

#include "mon_entity.h"


namespace Mon {

	struct World
	{
		v3 origin;

		unsigned int entityCount;
		Entity entities[256];
	};

	static unsigned int AddEntity(World* world)
	{
		unsigned int entityIndex = world->entityCount++;

		Entity* entity = &world->entities[entityIndex];
		entity = {};

		return entityIndex;
	}

	static Entity* GetEntity(World* world, unsigned int index)
	{
		Entity* entity = 0;
		if ((index > 0) && (index < ArrayCount(world->entities)))
		{
			entity = &world->entities[index];
		}
		return entity;
	}

	static Entity* GetPlayer(World* world)
	{
		Entity* entity = 0;
		entity = &world->entities[1];
		return entity;
	}


	static void ClearEntity(World* world, unsigned int index)
	{
		Entity* entity = &world->entities[index];
		entity = nullptr;
		// We just add an entity like normal
		//if ((index > 0) && (index < ArrayCount(world->entities)))
		//{
		//	entity = &world->entities[index];

		//	entity->collider = {};
		//	// TODO(ck): ClearRenderData - must call glDeleteTextures before generating new ones
		//	entity->data = {};
		//	entity->setup = {};
		//	entity->name = "";
		//}
	}


	static void InitEntity(Entity* e, const char* name, v3 pos, v3 scale, float angleDegrees, int textureIndex, int meshIndex, bool isModel = false)
	{
		e->name = name;
		e->impPath = "none";
		// TEXTURE INDEXES

		// e->data.meshIndex = GetMesh("quad"); and GetTexture("nameFromParams")
		e->meshIndex = meshIndex;
		e->textureIndex = textureIndex;
		//e->data.programType = MonGL::ProgramType::Common;
		//e->data.scale = scale;
		//e->data.wireFrame = false;
		//e->data.visible = true;
		e->rb.worldPos = pos;
		e->rb.worldPos.y = 0.5f;
		e->follow = false;
		e->spriteAnimationIndex = 0; // TODO(ck): Set sprite animation index for sprite entities
		Mesh* mesh = Mon::GetMesh(g_Assets, meshIndex);
		v3 meshSize = mesh->max - mesh->min;
		//if (meshSize.x == 0.0f && meshSize.y == 0.0f && meshSize.z == 0.0f)
			//meshSize = v3(1.0f);
		
		e->spriteAngleDegrees = angleDegrees;
		
		//e->data.programData.texCoordScale = 1.0f;

		if (isModel)
			e->flags = EntityRenderFlag::Model;
		else
			e->flags = EntityRenderFlag::Sprite;
	}

	static void InitPlayer(Entity* player)
	{
		player->name = "player";
		player->isPlayer = true;

		player->facingDir = Direction::Forward;
		player->meshIndex = 0;
		player->textureIndex = 16;
		//player->data.wireFrame = false;
		//player->data.visible = true;
		//player->data.programData.texCoordScale = 1.0f;
		//player->data.programType = MonGL::ProgramType::Common;

		player->rb.worldPos = v3(3.0f, 0.5f, 2.0);
		player->rb.inverseMass = 10.0f;
		player->rb.velocity = v3(0.0f, 0.0f, 0.0f); // 35m/s
		player->rb.gravity = 10.0f;
		player->rb.acceleration = v3(-40.0f, 0.0f, 0.0f);
		player->rb.orientation = v3(1.0f, 1.0f, 1.0);
		player->rb.damping = 0.9f;
		player->rb.speed = 26.0f;
		

		//player->data.mat.ambient = v3(1.0f, 0.5f, 0.6f);
		//player->data.mat.diffuse = v3(1.0f, 0.5f, 0.31f);
		//player->data.mat.specular = v3(0.5f, 0.5f, 0.5f);
		//player->data.mat.shininess = 32.0f;
		player->impPath = "none";
		player->spriteAngleDegrees = -45.0f;
		player->spriteAnimationIndex = 0;
		player->flags = EntityRenderFlag::Sprite;
	}

	static void InitWater(Entity* e, int shaderHandle)
	{
		e->name = "water";
		e->meshIndex = 6;
		e->textureIndex = 4;
	/*	e->data.programType = MonGL::ProgramType::Water;
		e->data.wireFrame = false;
		e->data.visible = true;*/

		
		e->rb.worldPos = v3(15.00f, -0.40f, -19.95);
		e->rb.speed = 40.0f;
		//e->data.scale = v3(18.0f, 1.0f, 18.0f);

		e->impPath = "none";
		e->spriteAngleDegrees = 0.0f;

		//e->data.programData.tiling = 3.0f;
		//e->data.programData.flowOffset = -0.5f;
		//e->data.programData.flowStrength = 0.1f;
		//e->data.programData.heightScale = 0.1f;
		//e->data.programData.heightScaleModulated = 9.0f;
		//e->data.programData.speed = 0.5f;
		//e->data.programData.waveLength = 18.0f;
		//e->data.programData.uJump = 0.10f;
		//e->data.programData.vJump = 0.10f;
	}

	static void PlayerAttack(Entity* player)
	{

	}

	static void InitWorld(World* world, float angleDegrees)
	{
		world->origin = v3(0.0f, 0.0f, 0.0f);

		// reserve slot 0 for null entity
		AddEntity(world);

		// reserve slot 1 for player entity
		AddEntity(world);
		Entity* player = GetPlayer(world);
		InitPlayer(player);
		

		AddEntity(world);
		Entity* light = GetEntity(world, 2);
		InitEntity(light, "light", v3(1.0f), v3(1.0f), angleDegrees, 1, 2, true);

		/*
		how should lights work? are they attached to an entity? right now they are part of the rendering layer which i think
		makes them easier to manage. attaching a light to an entity is possible?
		
		*/


		v3 basePoint = v3(-32.0f, 0.0f, -32.0f);
		for (int i = world->entityCount; i < 10; ++i)
		{
			AddEntity(world);
			Entity* tree = GetEntity(world, i);
			v3 offset = v3(i * 4, 0.5f ,i * 4) + basePoint; 
			InitEntity(tree, "tree", offset, v3(1.0f), angleDegrees, 7, 1);
		}
		
		AddEntity(world);
		Entity* cube = GetEntity(world, world->entityCount - 1);
		InitEntity(cube, "cube_1", v3(50.0f, 0.3f, 20.0f), v3(1.0f), angleDegrees, 18, 2, true);

		// ch_minion
		AddEntity(world);
		Entity* ent = GetEntity(world, world->entityCount - 1);
		InitEntity(ent, "minion1", v3(10.0f,4.0f,10.0f), v3(1.0f), angleDegrees, 9, 1);
		ent->facingDir = Direction::Right;

		AddEntity(world);
		Entity* ent1 = GetEntity(world, world->entityCount - 1);
		InitEntity(ent1, "minion2", v3(30.0f, 4.0f, 5.0f), v3(1.0f), angleDegrees, 9, 1);
		ent1->facingDir = Direction::Right;
		ent1->follow = true;

		AddEntity(world);
		Entity* ent2 = GetEntity(world, world->entityCount - 1);
		InitEntity(ent2, "minion3", v3(26.0f, 2.0f, 7.0f), v3(1.0f), angleDegrees, 9, 1);

		AddEntity(world);
		Entity* ent3 = GetEntity(world, world->entityCount - 1);
		InitEntity(ent3, "minion4", v3(2.0f, 6.0f, 1.0f), v3(1.0f), angleDegrees, 9, 1);

		AddEntity(world);
		Entity* ent4 = GetEntity(world, world->entityCount - 1);
		InitEntity(ent4, "ch_witch2", v3(12.0f, 0.0f, 9.0f), v3(1.0f), angleDegrees, 10, 1);

		AddEntity(world);
		Entity* plane64 = GetEntity(world, world->entityCount - 1);
		InitEntity(plane64, "plane64", v3(6.0f, 0.0f, 6.0f), v3(1.0f), 0.0f, 8, 7, true);
		plane64->meshIndex = 1;

		AddEntity(world);
		Entity* gem = GetEntity(world, world->entityCount - 1);
		InitEntity(gem, "gem", v3(6.0f, 0.0f, 6.0f), v3(1.0f), 0.0f, 8, 8, true);
		gem->meshIndex = 1;

		AddEntity(world);
		Entity* house = GetEntity(world, world->entityCount - 1);
		InitEntity(house, "house", v3(16.0f, 0.0f, 16.0f), v3(1.0f), 0.0f, 23, 10, true);
		house->meshIndex = 1;

	}
	
	static void UpdateWorld(World* world)
	{
		// move entity in here?
	}

	static int TestAABB(AABB a, AABB b)
	{
		if (a.max.x < b.min.x || a.min.x > b.max.x) return 0;
		if (a.max.z < b.min.z || a.min.z > b.max.z) return 0;
		// NOTE(ck): Check y last because it is the least likely collison
		if (a.max.y < b.min.y || a.min.y > b.max.y) return 0;
		// Overlapping on all axes means AABBs are intersecting
		return 1;
	}

	static int InterestMovingAABB(AABB a, AABB b, v3 va, v3 vb, float& tFirst, float& tLast)
	{
		if (TestAABB(a, b))
		{
			tFirst = tLast = 0.0f;
			return 1;
		}

		// constant velocities va, vb
		// relative velocity. treat 'a' as stationary
		v3 v = vb - va;
		tFirst = 0.0f;
		tLast = 1.0f;

		for (int i = 0; i < 3; i++) 
		{
			if (v[i] < 0.0f) 
			{
				if (b.max[i] < a.min[i]) return 0;
				// Nonintersecting and moving apart
				if (a.max[i] < b.min[i]) tFirst = std::max((a.max[i] - b.min[i]) / v[i], tFirst);
				if (b.max[i] > a.min[i]) tLast = std::min((a.min[i] - b.max[i]) / v[i], tLast);
			}
			if (v[i] > 0.0f) 
			{
				if (b.min[i] > a.max[i]) return 0;
				// Nonintersecting and moving apart
				if (b.max[i] < a.min[i]) tFirst = std::max((a.min[i] - b.max[i]) / v[i], tFirst);
				if (a.max[i] > b.min[i]) tLast = std::min((a.max[i] - b.min[i]) / v[i], tLast);
			}
			// No overlap possible if time of first contact occurs after time of last contact
			if (tFirst > tLast) return 0;
		}
		return 1;
	}

	static void MovePlayer(World* world, v3* velocity, bool jumped, float dt)
	{
		Entity* player = GetPlayer(world);


// handmade hero method
#if 1
		// ddPLength
		float velocityLength = lengthSq(*velocity);
		if (velocityLength > 1.0f)
		{
			*velocity *= (1.0f / squareRoot(velocityLength));
		}

		float speed = player->rb.speed;
		//if (input.shift.endedDown)
			//speed *= 1.5f;
		v3 v = *velocity;
		*velocity *= speed;
		*velocity += -6.0f * player->rb.velocity;

		v3 oldPos = player->rb.worldPos;
		
		float deltaX = (0.5f * velocity->x * square(dt) + player->rb.velocity.x * dt);
		float deltaY = player->rb.velocity.y;// *0.5f;
		//float deltaY = velocity->y * square(deltaTime) + player.particle.velocity.y * deltaTime);
		float deltaZ = (0.5f * velocity->z * square(dt) + player->rb.velocity.z * dt);
		v3 delta = { deltaX, deltaY, deltaZ };

		v3 newPos = oldPos + delta;


#endif

		// COLLISION TEST 
		Entity otherEnt = world->entities[3];
		float tFirst = 0.0f; 
		float tLast = 0.0f;
		
		AABB a = player->bbox;
		AABB b = otherEnt.bbox;
		//otherEnt.rb.velocity = v3(1.0f);

		//bool colliding = InterestMovingAABB(a, b, player->rb.velocity, otherEnt.rb.velocity, tFirst, tLast);
		bool colliding = InterestMovingAABB(a, b, v, otherEnt.rb.velocity, tFirst, tLast);

		//Mon::Log::print("tFirst:", tFirst);
		//Mon::Log::print("tLast:", tLast);
		player->rb.velocity.x = velocity->x * dt + player->rb.velocity.x;
		player->rb.velocity.z = velocity->z * dt + player->rb.velocity.z;
		//return 1;
		
		//for (unsigned int i = 1; i < world->entityCount; ++i)
		//{
		//	// TODO(ck): Broad Phase Collision Check

		//	// TODO(ck): Precise Collision check

		//	if (i != 1) // 1 is player index
		//	{
		//		Entity testEntity = *GetEntity(world, i);

		//		if (TestAABBAABB(player->collider, testEntity.collider))
		//		{
		//			newPos = player->rb.worldPos + 0.05f;
		//			newPos.y = 0.0f;
		//			break;
		//		}
		//	}
		//}


		// I don't want to use casey's method. I want something that feels more tile
		// game based
		//player->rb.worldPos += (*velocity * player->rb.speed * dt);
		player->rb.worldPos = newPos;
#if 0
		if (!colliding)
			player->rb.worldPos = newPos;
		else
		{
			player->rb.worldPos.x = oldPos.x;
			player->rb.worldPos.z = oldPos.z;
		}
#endif
		//player->rb.worldPos.x += (velocity->x * player->rb.speed * dt);
		//player->rb.worldPos.z += (velocity->z * player->rb.speed * dt);

		SetFacingDirection(player);
	}
}
#endif