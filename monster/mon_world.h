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

	static unsigned int addEntity(World* world)
	{
		unsigned int entityIndex = world->entityCount++;

		Entity* entity = &world->entities[world->entityCount];
		entity = {};

		return entityIndex;
	}

	static Entity* getEntity(World* world, unsigned int index)
	{
		Entity* entity = 0;
		if ((index > 0) && (index < ArrayCount(world->entities)))
		{
			entity = &world->entities[index];
		}
		return entity;
	}

	static void initEntity(World* world, unsigned int index)
	{

	}

	static void initPlayer(World* world, int shaderHandle)
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

		MonGL::InitBoundingBox(&world->player->collider.data, &world->player->collider.size);
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

	static void initWorld(World* world, int shaderHandle)
	{
		// reserve slot 0 for null enttiy
		addEntity(world);

		for (int i = 1; i < 10; ++i)
		{
			addEntity(world);
			Entity* tree = getEntity(world, i);

			tree->setup = {};
			tree->name = "tree_" + std::to_string(i);
			MonGL::InitQuad(&tree->data);
			MonGL::LoadTexture(&tree->data, 0, MonGL::Type::Diffuse, shaderHandle, "res/textures/tree.png");
			tree->particle.pos = v3(6.0f * (i + 1), 6.80f, 5.5f * i);
		}

		int length = world->entityCount + 5;
		for (int i = world->entityCount; i < length; ++i)
		{
			addEntity(world);
			Entity* flower = getEntity(world, i);

			flower->setup = {};
			flower->name = "flower_" + std::to_string(i);
			MonGL::InitQuad(&flower->data);
			MonGL::LoadTexture(&flower->data, 0, MonGL::Type::Diffuse, shaderHandle, "res/textures/sflow_tall.png");
			flower->particle.pos = v3(10.0f, 0.1f, 6.0f);
		}


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
	
}
#endif