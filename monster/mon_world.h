#ifndef MON_WORLD_H
#define MON_WORLD_H

#include "mon_entity.h"

namespace Mon {

	struct World
	{
		unsigned int entityCount;
		Entity entities[100];
		Entity* player;
	};

	static unsigned int AddEntity(World* world)
	{
		unsigned int entityIndex = world->entityCount++;

		Entity* entity = &world->entities[world->entityCount];
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

	static void InitEntity(World* world, unsigned int index)
	{

	}

	static void InitPlayer(World* world, int shaderHandle)
	{
		// TODO(ck): MEMORY MANAGEMENT
		world->player = new Entity();
		world->player->name = "player";
		world->player->setup = {};
		MonGL::InitQuad(&world->player->data);
		MonGL::LoadTexture(&world->player->data, 0, MonGL::Type::Diffuse, shaderHandle, "res/textures/p1SIDERIGHT.png");
		MonGL::LoadTexture(&world->player->data, 1, MonGL::Type::Diffuse, shaderHandle, "res/textures/p1.png");
		MonGL::LoadTexture(&world->player->data, 2, MonGL::Type::Diffuse, shaderHandle, "res/textures/p1SIDE.png");
		MonGL::LoadTexture(&world->player->data, 3, MonGL::Type::Diffuse, shaderHandle, "res/textures/p1BACK.png");
		world->player->facingDir = 0;

		MonGL::InitBoundingBox(&world->player->collider.data);
		world->player->particle.pos = v3(40.0f, 0.1f, 10.0);
		world->player->particle.inverseMass = 10.0f;
		world->player->particle.velocity = v3(0.0f, 0.0f, 0.0f); // 35m/s
		world->player->particle.gravity = 10.0f;
		world->player->particle.acceleration = v3(-40.0f, 0.0f, 0.0f);
		world->player->particle.orientation = v3(1.0f, 1.0f, 1.0);
		world->player->particle.damping = 0.9f;
		world->player->particle.speed = 50.0f;

		world->player->data.mat.ambient = v3(1.0f, 0.5f, 0.6f);
		world->player->data.mat.diffuse = v3(1.0f, 0.5f, 0.31f);
		world->player->data.mat.specular = v3(0.5f, 0.5f, 0.5f);
		world->player->data.mat.shininess = 32.0f;
	}

	static void InitWorld(World* world, int shaderHandle)
	{
		// reserve slot 0 for null entity
		AddEntity(world);

		for (int i = 1; i < 10; ++i)
		{
			AddEntity(world);
			Entity* tree = GetEntity(world, i);

			tree->setup = {};
			tree->name = "tree_" + std::to_string(i);
			MonGL::InitQuad(&tree->data);
			MonGL::LoadTexture(&tree->data, 0, MonGL::Type::Diffuse, shaderHandle, "res/textures/tree.png");
			tree->particle.pos = v3(6.0f * (i + 1), 6.80f, 5.5f * i);
			tree->data.scale = v3(16.0f, 16.0f, 16.0f);
			MonGL::InitBoundingBox(&tree->collider.data);
		}

		int length = world->entityCount + 5;
		for (int i = world->entityCount; i < length; ++i)
		{
			AddEntity(world);
			Entity* flower = GetEntity(world, i);

			flower->setup = {};
			flower->name = "flower_" + std::to_string(i);
			MonGL::InitQuad(&flower->data);
			MonGL::LoadTexture(&flower->data, 0, MonGL::Type::Diffuse, shaderHandle, "res/textures/sflow_tall.png");
			flower->particle.pos = v3(10.0f, 0.1f, 6.0f);
			MonGL::InitBoundingBox(&flower->collider.data);

		}
		
		AddEntity(world);
		Entity* cube = GetEntity(world, world->entityCount - 1);
		cube->setup = {};
		cube->name = "cube_1";
		MonGL::InitCube(&cube->data);
		MonGL::LoadTexture(&cube->data, 0, MonGL::Type::Diffuse, shaderHandle, "res/textures/container2.png");
		cube->particle.pos = v3(50.0f, 0.3f, 20.0f);
		MonGL::InitBoundingBox(&cube->collider.data);

		//for (int i = 0; i < 4; ++i)
		//{
		//	Entity entity = {};
		//	entity.setup = {};
		//	entity.name = "enemy_" + std::to_string(i);
		//	MonGL::initBoundingBox(&entity.collider.data, &entity.collider.size);
		//	entity.particle.pos = v3((8.0f * (i + 1)), 0.1f, 1.5f * i);
		//	mat4 model = mat4(1.0f);
		//	entity.collider.data.worldMatrix = glm::translate(model, entity.particle.pos);
		//	entity.collider.data.color = v3(0.0f, 0.0f, 0.0f);
		//	
		//	//enemies.push_back(entity);
		//}
	}

	static void AddCube(World* world, int shaderHandle)
	{
		AddEntity(world);
		Entity* cube = GetEntity(world, world->entityCount - 1);
		cube->setup = {};
		cube->name = "cube_" + std::to_string(world->entityCount-1);
		MonGL::InitCube(&cube->data);
		MonGL::LoadTexture(&cube->data, 0, MonGL::Type::Diffuse, shaderHandle, "res/textures/container2.png");
		cube->particle.pos = v3(10.0f, 0.3f, 20.0f);
		MonGL::InitBoundingBox(&cube->collider.data);
	}
	
}
#endif