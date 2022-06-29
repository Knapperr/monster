#ifndef MON_WORLD_H
#define MON_WORLD_H

#include "mon_entity.h"


namespace Mon {

	struct World
	{
		unsigned int entityCount;
		Entity entities[256];

		// Probably want this in the rendering layer?
		MonGL::RenderData renderItems[256];
		// TODO(ck): Not sure if I want things like trees but they can go into instanced data I guess?
		// might still want to have trees in entities because they can be attacked or respond to player interaction?
		// StaticEntities??? staticEntities[256];
		

		unsigned int instancedDataCount;
		MonGL::InstancedData instancedData[2];
	};

	

	static unsigned int AddEntity(World* world)
	{
		unsigned int entityIndex = world->entityCount++;

		Entity* entity = &world->entities[entityIndex];
		entity = {};

		return entityIndex;
	}

	static unsigned int AddInstancedData(World* world)
	{
		unsigned int instancedIndex = world->instancedDataCount++;
		
		MonGL::InstancedData* data = &world->instancedData[world->instancedDataCount];
		data = {};

		return instancedIndex;
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
		Entity* entity = 0;

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

	static MonGL::InstancedData* GetInstancedData(World* world, unsigned int index)
	{
		MonGL::InstancedData* data = 0;
		if ((index > 0) && (index < ArrayCount(world->instancedData)))
		{
			data = &world->instancedData[index];
		}
		return data;
	}

	static void InitEntity(Entity* e, const char* name, v3 pos, v3 scale, float angleDegrees, int shaderHandle, int textureIndex, int meshIndex)
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
		e->rb.pos = pos;
		e->follow = false;
		InitBoxCollider(&e->collider);
		e->spriteAngleDegrees = angleDegrees;
	}

	static void InitPlayer(Entity* player, int shaderHandle)
	{
		player->name = "player";
		player->setup = {};
		player->data.meshIndex = 1;
		player->data.textureIndex = 17;
		player->data.wireFrame = false;
		player->data.visible = true;
		player->facingDir = Direction::Forward;
		player->data.programType = MonGL::ProgramType::Common;


		InitBoxCollider(&player->collider);
		player->rb.pos = v3(3.0f, -0.15f, 2.0);
		player->rb.inverseMass = 10.0f;
		player->rb.velocity = v3(0.0f, 0.0f, 0.0f); // 35m/s
		player->rb.gravity = 10.0f;
		player->rb.acceleration = v3(-40.0f, 0.0f, 0.0f);
		player->rb.orientation = v3(1.0f, 1.0f, 1.0);
		player->rb.damping = 0.9f;
		player->rb.speed = 30.0f;

		//player->data.mat.ambient = v3(1.0f, 0.5f, 0.6f);
		//player->data.mat.diffuse = v3(1.0f, 0.5f, 0.31f);
		//player->data.mat.specular = v3(0.5f, 0.5f, 0.5f);
		//player->data.mat.shininess = 32.0f;

		player->impPath = "none";
		player->spriteAngleDegrees = -45.0f;
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
		e->rb.pos = v3(15.00f, -0.40f, -19.95);
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
		// reserve slot 0 for null entity
		AddEntity(world);

		// reserve slot 1 for player entity
		AddEntity(world);
		Entity* player = GetPlayer(world);
		InitPlayer(player, shaderHandle);
		
		v3 basePoint = v3(-32.0f, 0.0f, -32.0f);
		for (int i = world->entityCount; i < 10; ++i)
		{
			AddEntity(world);
			Entity* tree = GetEntity(world, i);
			v3 offset = v3(i * 4, 1.50f ,i * 4) + basePoint; 
			InitEntity(tree, "tree", offset, v3(6.0f), angleDegrees, shaderHandle, 7, 1);
		}

		int length = world->entityCount + 5;
		for (int i = world->entityCount; i < length; ++i)
		{
			AddEntity(world);
			Entity* flower = GetEntity(world, i);
			InitEntity(flower, "flower", v3(10.0f, 0.1f, 6.0f), v3(1.0f), angleDegrees, shaderHandle, 7, 1);
		}
		
		AddEntity(world);
		Entity* cube = GetEntity(world, world->entityCount - 1);
		InitEntity(cube, "cube_1", v3(50.0f, 0.3f, 20.0f), v3(1.0f), angleDegrees, shaderHandle, 18, 2);

		// ch_minion
		AddEntity(world);
		Entity* ent = GetEntity(world, world->entityCount - 1);
		InitEntity(ent, "minion1", v3(10.0f,4.0f,10.0f), v3(1.0f), angleDegrees, shaderHandle, 9, 1);
		ent->facingDir = Direction::Right;

		AddEntity(world);
		Entity* ent1 = GetEntity(world, world->entityCount - 1);
		InitEntity(ent1, "minion2", v3(30.0f, 4.0f, 5.0f), v3(1.0f), angleDegrees, shaderHandle, 9, 1);
		ent1->facingDir = Direction::Right;
		ent1->follow = true;

		AddEntity(world);
		Entity* ent2 = GetEntity(world, world->entityCount - 1);
		InitEntity(ent2, "minion3", v3(26.0f, 2.0f, 7.0f), v3(1.0f), angleDegrees, shaderHandle, 9, 1);

		AddEntity(world);
		Entity* ent3 = GetEntity(world, world->entityCount - 1);
		InitEntity(ent3, "minion4", v3(2.0f, 6.0f, 1.0f), v3(1.0f), angleDegrees, shaderHandle, 9, 1);

		AddEntity(world);
		Entity* ent4 = GetEntity(world, world->entityCount - 1);
		InitEntity(ent4, "ch_witch2", v3(12.0f, 0.0f, 9.0f), v3(1.0f), angleDegrees, shaderHandle, 10, 1);

		AddEntity(world);
		Entity* plane64 = GetEntity(world, world->entityCount - 1);
		InitEntity(plane64, "plane64", v3(6.0f, 0.0f, 6.0f), v3(1.0f), 0.0f, shaderHandle, 8, 1);
		plane64->data.meshIndex = 7;

		AddEntity(world);
		Entity* gem = GetEntity(world, world->entityCount - 1);
		InitEntity(gem, "gem", v3(6.0f, 0.0f, 6.0f), v3(1.0f), 0.0f, shaderHandle, 8, 1);
		gem->data.meshIndex = 8;
	}
	
	static void CreateInstancedGrass(World* world, int shaderHandle)
	{
		AddInstancedData(world);
		MonGL::InstancedData* data = GetInstancedData(world, world->instancedDataCount - 1);
		MonGL::InitInstancedData(data, 100);
		//MonGL::LoadTexture(&data->renderData, 0, MonGL::TextureType::Diffuse, shaderHandle, "res/textures/grass.png", false);


	}

	static void UpdateWorld(World* world)
	{

	}
}
#endif