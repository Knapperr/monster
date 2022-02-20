#ifndef MON_WORLD_H
#define MON_WORLD_H

#include "mon_terrain.h"


namespace Mon {

	struct World
	{
		unsigned int entityCount;
		Entity entities[256];

		// Probably want this in the rendering layer?
		MonGL::RenderData renderItems[256];
		MonGL::RenderData lines[128];
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
		if ((index > 0) && (index < ArrayCount(world->entities)))
		{
			entity = &world->entities[index];

			entity->collider = {};
			// TODO(ck): ClearRenderData - must call glDeleteTextures before generating new ones
			entity->data = {};
			entity->setup = {};
			entity->name = "";
		}
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

	static void InitEntity(World* world, unsigned int index)
	{

	}

	static void InitPlayer(Entity* player, int shaderHandle)
	{
		// TODO(ck): MEMORY MANAGEMENT
		//world->player = new Entity();
		player->name = "player";
		player->setup = {};
		MonGL::InitQuad(&player->data);

		// IMPORTANT(ck): These cant be loaded again if they are already loaded..
		MonGL::LoadTexture(&player->data, 0, MonGL::TextureType::Diffuse, shaderHandle, "res/textures/ch_witch.png");
		MonGL::LoadTexture(&player->data, 1, MonGL::TextureType::Diffuse, shaderHandle, "res/textures/p1.png");
		MonGL::LoadTexture(&player->data, 2, MonGL::TextureType::Diffuse, shaderHandle, "res/textures/p1SIDE.png");
		MonGL::LoadTexture(&player->data, 3, MonGL::TextureType::Diffuse, shaderHandle, "res/textures/p1BACK.png");
		player->facingDir = Direction::Forward;

		InitBoxCollider(&player->collider);
		player->rb.pos = v3(40.0f, 0.0f, 10.0);
		player->rb.inverseMass = 10.0f;
		player->rb.velocity = v3(0.0f, 0.0f, 0.0f); // 35m/s
		player->rb.gravity = 10.0f;
		player->rb.acceleration = v3(-40.0f, 0.0f, 0.0f);
		player->rb.orientation = v3(1.0f, 1.0f, 1.0);
		player->rb.damping = 0.9f;
		player->rb.speed = 50.0f;

		player->data.mat.ambient = v3(1.0f, 0.5f, 0.6f);
		player->data.mat.diffuse = v3(1.0f, 0.5f, 0.31f);
		player->data.mat.specular = v3(0.5f, 0.5f, 0.5f);
		player->data.mat.shininess = 32.0f;

		player->impPath = "none";
		player->spriteAngleDegrees = -45.0f;
	}

	static void PlayerAttack(Entity* player)
	{

	}

	static void InitWorld(World* world, int shaderHandle, float angleDegrees)
	{
		// reserve slot 0 for null entity
		AddEntity(world);

		// reserve slot 1 for player entity
		AddEntity(world);
		Entity* player = GetPlayer(world);
		InitPlayer(player, shaderHandle);
		
		for (int i = world->entityCount; i < 10; ++i)
		{
			AddEntity(world);
			Entity* tree = GetEntity(world, i);

			tree->setup = {};
			tree->name = "tree_" + std::to_string(i);
			tree->impPath = "none";
			MonGL::InitQuad(&tree->data);
			MonGL::LoadTexture(&tree->data, 0, MonGL::TextureType::Diffuse, shaderHandle, "res/textures/tree.png");
			tree->rb.pos = v3(6.0f * (i + 1), 5.30f, 5.5f * i);
			tree->data.scale = v3(16.0f, 16.0f, 16.0f);
			InitBoxCollider(&tree->collider);
			tree->spriteAngleDegrees = angleDegrees;
		}

		int length = world->entityCount + 5;
		for (int i = world->entityCount; i < length; ++i)
		{
			AddEntity(world);
			Entity* flower = GetEntity(world, i);

			flower->setup = {};
			flower->name = "flower_" + std::to_string(i);
			flower->impPath = "none";
			MonGL::InitQuad(&flower->data);
			MonGL::LoadTexture(&flower->data, 0, MonGL::TextureType::Diffuse, shaderHandle, "res/textures/sflow_tall.png");
			flower->rb.pos = v3(10.0f, 0.1f, 6.0f);
			InitBoxCollider(&flower->collider);
			flower->spriteAngleDegrees = angleDegrees;
		}
		
		AddEntity(world);
		Entity* cube = GetEntity(world, world->entityCount - 1);
		cube->setup = {};
		cube->name = "cube_1";
		cube->impPath = "none";
		MonGL::InitCube(&cube->data);
		MonGL::LoadTexture(&cube->data, 0, MonGL::TextureType::Diffuse, shaderHandle, "res/textures/container2.png");
		cube->rb.pos = v3(50.0f, 0.3f, 20.0f);
		InitBoxCollider(&cube->collider);
		cube->spriteAngleDegrees = angleDegrees;

		// ch_minion
		AddEntity(world);
		Entity* ent = GetEntity(world, world->entityCount - 1);
		ent->setup = {};
		ent->name = "minion1";
		ent->impPath = "none";
		MonGL::InitQuad(&ent->data);
		MonGL::LoadTexture(&ent->data, 0, MonGL::TextureType::Diffuse, shaderHandle, "res/textures/ch_minion.png");
		ent->rb.pos = v3(10.0f, 10.0f, 10.0f);
		InitBoxCollider(&ent->collider);
		ent->spriteAngleDegrees = angleDegrees;

		AddEntity(world);
		Entity* ent1 = GetEntity(world, world->entityCount - 1);
		ent1->setup = {};
		ent1->name = "minion2";
		ent1->impPath = "none";
		MonGL::InitQuad(&ent1->data);
		MonGL::LoadTexture(&ent1->data, 0, MonGL::TextureType::Diffuse, shaderHandle, "res/textures/ch_minion.png");
		ent1->rb.pos = v3(10.0f, 10.0f, 10.0f);
		InitBoxCollider(&ent1->collider);
		ent1->spriteAngleDegrees = angleDegrees;

		AddEntity(world);
		Entity* ent2 = GetEntity(world, world->entityCount - 1);
		ent2->setup = {};
		ent2->name = "minion3";
		ent2->impPath = "none";
		MonGL::InitQuad(&ent2->data);
		MonGL::LoadTexture(&ent2->data, 0, MonGL::TextureType::Diffuse, shaderHandle, "res/textures/ch_minion.png");
		ent2->rb.pos = v3(10.0f, 10.0f, 10.0f);
		InitBoxCollider(&ent2->collider);
		ent2->spriteAngleDegrees = angleDegrees;

		AddEntity(world);
		Entity* ent3 = GetEntity(world, world->entityCount - 1);
		ent3->setup = {};
		ent3->name = "minion4";
		ent3->impPath = "none";
		MonGL::InitQuad(&ent3->data);
		MonGL::LoadTexture(&ent3->data, 0, MonGL::TextureType::Diffuse, shaderHandle, "res/textures/ch_minion.png");
		ent3->rb.pos = v3(10.0f, 10.0f, 10.0f);
		InitBoxCollider(&ent3->collider);
		ent3->spriteAngleDegrees = angleDegrees;

		AddEntity(world);
		Entity* ent4 = GetEntity(world, world->entityCount - 1);
		ent4->setup = {};
		ent4->name = "witch";
		ent4->impPath = "none";
		MonGL::InitQuad(&ent4->data);
		MonGL::LoadTexture(&ent4->data, 0, MonGL::TextureType::Diffuse, shaderHandle, "res/textures/ch_witch2.png");
		ent4->rb.pos = v3(25.0f, 0.0f, 26.0f);
		InitBoxCollider(&ent4->collider);
		ent4->spriteAngleDegrees = angleDegrees;
	}

	static void AddCube(World* world, int shaderHandle)
	{
		AddEntity(world);
		Entity* cube = GetEntity(world, world->entityCount - 1);
		cube->setup = {};
		cube->name = "cube_" + std::to_string(world->entityCount-1);
		cube->impPath = "none";
		MonGL::InitCube(&cube->data);
		MonGL::LoadTexture(&cube->data, 0, MonGL::TextureType::Diffuse, shaderHandle, "res/textures/container2.png");
		cube->rb.pos = v3(10.0f, 0.3f, 20.0f);
		MonGL::InitBoundingBox(&cube->collider.data);
	}
	
	static void CreateInstancedGrass(World* world, int shaderHandle)
	{
		AddInstancedData(world);
		MonGL::InstancedData* data = GetInstancedData(world, world->instancedDataCount - 1);
		MonGL::InitInstancedData(data, 100);
		MonGL::LoadTexture(&data->renderData, 0, MonGL::TextureType::Diffuse, shaderHandle, "res/textures/grass.png", false);


	}

	static void UpdateWorld(World* world)
	{

	}
}
#endif