#ifndef MON_WORLD_H
#define MON_WORLD_H

#include "mon_entity.h"


namespace Mon {

	struct World
	{
		v3 origin;

		unsigned int entityCount;
		Entity entities[256];

		// Probably want this in the rendering layer?
		MonGL::RenderData renderItems[256];
		// TODO(ck): Not sure if I want things like trees but they can go into instanced data I guess?
		// might still want to have trees in entities because they can be attacked or respond to player interaction?
		// StaticEntities??? staticEntities[256];

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
		e->setup = {};
		e->name = name;
		e->impPath = "none";
		// TEXTURE INDEXES

		// e->data.meshIndex = GetMesh("quad"); and GetTexture("nameFromParams")
		e->data.meshIndex = meshIndex;
		e->data.programType = MonGL::ProgramType::Common;
		e->data.textureIndex = textureIndex;
		e->data.scale = scale;
		e->data.wireFrame = false;
		e->data.visible = true;
		e->rb.worldPos = pos;
		e->follow = false;
		e->spriteAnimationIndex = 0; // TODO(ck): Set sprite animation index for sprite entities
		Mesh* mesh = Mon::GetMesh(g_Assets, meshIndex);
		v3 meshSize = mesh->max - mesh->min;
		//if (meshSize.x == 0.0f && meshSize.y == 0.0f && meshSize.z == 0.0f)
			//meshSize = v3(1.0f);
		InitBoxCollider(&e->collider, e->rb.worldPos, scale, meshSize);
		e->spriteAngleDegrees = angleDegrees;
		
		e->data.programData.texCoordScale = 1.0f;

		if (isModel)
			e->flags = EntityRenderFlag::Model;
		else
			e->flags = EntityRenderFlag::Sprite;
	}

	static void InitPlayer(Entity* player)
	{
		player->name = "player";
		player->isPlayer = true;

		player->setup = {};
		player->data.meshIndex = 1;
		player->data.textureIndex = 17;
		player->data.wireFrame = false;
		player->data.visible = true;
		player->data.programData.texCoordScale = 1.0f;
		player->facingDir = Direction::Forward;
		player->data.programType = MonGL::ProgramType::Common;

		player->rb.worldPos = v3(3.0f, 0.0f, 2.0);
		player->rb.inverseMass = 10.0f;
		player->rb.velocity = v3(0.0f, 0.0f, 0.0f); // 35m/s
		player->rb.gravity = 10.0f;
		player->rb.acceleration = v3(-40.0f, 0.0f, 0.0f);
		player->rb.orientation = v3(1.0f, 1.0f, 1.0);
		player->rb.damping = 0.9f;
		player->rb.speed = 26.0f;
		InitBoxCollider(&player->collider, player->rb.worldPos, v3(1.0f), v3(1.0f));

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
		e->setup = {};
		e->data.meshIndex = 7;
		e->data.textureIndex = 5;
		e->data.programType = MonGL::ProgramType::Water;
		e->data.wireFrame = false;
		e->data.visible = true;

		InitBoxCollider(&e->collider);
		e->rb.worldPos = v3(15.00f, -0.40f, -19.95);
		e->rb.speed = 40.0f;
		e->data.scale = v3(18.0f, 1.0f, 18.0f);

		e->impPath = "none";
		e->spriteAngleDegrees = 0.0f;

		e->data.programData.tiling = 3.0f;
		e->data.programData.flowOffset = -0.5f;
		e->data.programData.flowStrength = 0.1f;
		e->data.programData.heightScale = 0.1f;
		e->data.programData.heightScaleModulated = 9.0f;
		e->data.programData.speed = 0.5f;
		e->data.programData.waveLength = 18.0f;
		e->data.programData.uJump = 0.10f;
		e->data.programData.vJump = 0.10f;
	}

	static void PlayerAttack(Entity* player)
	{

	}

	static void InitWorld(World* world, int shaderHandle, int waterShaderHandle, float angleDegrees)
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
			v3 offset = v3(i * 4, 0.0f ,i * 4) + basePoint; 
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
		plane64->data.meshIndex = 7;

		AddEntity(world);
		Entity* gem = GetEntity(world, world->entityCount - 1);
		InitEntity(gem, "gem", v3(6.0f, 0.0f, 6.0f), v3(1.0f), 0.0f, 8, 8, true);
		gem->data.meshIndex = 8;

		AddEntity(world);
		Entity* house = GetEntity(world, world->entityCount - 1);
		InitEntity(house, "house", v3(16.0f, 0.0f, 16.0f), v3(1.0f), 0.0f, 23, 10, true);
		house->data.meshIndex = 10;

	}
	
	static void UpdateWorld(World* world)
	{
		// move entity in here?
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

		*velocity *= speed;
		*velocity += -6.0f * player->rb.velocity;

		v3 oldPos = player->rb.worldPos;
		
		float deltaX = (0.5f * velocity->x * square(dt) + player->rb.velocity.x * dt);
		float deltaY = player->rb.velocity.y * 0.5f;
		//float deltaY = velocity->y * square(deltaTime) + player.particle.velocity.y * deltaTime);
		float deltaZ = (0.5f * velocity->z * square(dt) + player->rb.velocity.z * dt);
		v3 delta = { deltaX, deltaY, deltaZ };

		v3 newPos = oldPos + delta;

		player->rb.velocity.x = velocity->x * dt + player->rb.velocity.x;
		player->rb.velocity.z = velocity->z * dt + player->rb.velocity.z;
#endif




		// TODO(ck): Update entity and then update entity collider right after
		// instead of having two separate loops for entities and their colliders.
		for (unsigned int i = 1; i < world->entityCount; ++i)
		{
			// TODO(ck): Broad Phase Collision Check

			// TODO(ck): Precise Collision check

			if (i != 1) // 1 is player index
			{
				Entity testEntity = *GetEntity(world, i);

				if (TestAABBAABB(player->collider, testEntity.collider))
				{
					newPos = player->rb.worldPos + 0.05f;
					newPos.y = 0.0f;
					break;
				}
			}
		}


		// I don't want to use casey's method. I want something that feels more tile
		// game based
		//player->rb.worldPos += (*velocity * player->rb.speed * dt);
		player->rb.worldPos = newPos;

		SetFacingDirection(player);
	}
}
#endif