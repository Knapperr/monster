#include "mon_world2D.h"

namespace Mon
{
	bool InitWorld(World2D_* world)
	{
		// TODO(ck): Memory management
		world = new World2D_();

		// Reserve slot 0 for null entity
		AddEntity2D(world);

		// Reserve slot 1 for player entity
		AddEntity2D(world);
		Entity2D* player = GetPlayer(world);
		InitEntity(player, "res/textures/ch_witch.png", true, v2(0, 0), 32);
		// InitPlayer(player, shaderHandle);
		//Entity2D* collider = new Entity2D();
		//initEntity(collider, "res/textures/p1.png", true, v2(50, 50), 32);
		//collider->sprite.wireFrame = true;
		
		AddEntity2D(world);
		Entity2D* ball = GetEntity2D(world, world->entityCount - 1);
		InitEntity(ball, "res/textures/test_house.png", true, v2(75, 84), 96);

		AddEntity2D(world);
		Entity2D* e = GetEntity2D(world, world->entityCount - 1);
		InitEntity(e, "res/textures/ch_minion.png", true, v2(60, 20), 32);
		

		//for (int i = 0; i < 5; ++i)
		//{
		//	Entity2D* e = new Entity2D();
		//	initEntity(e, "res/textures/p1_.png", true, v2(30 * i, 5 * i), 32);
		//	//entities.push_back(e);
		//}

		/*Entity2D* partner = new Entity2D();
		initEntity(partner, "res/textures/sRansome.png", true, v2(20.0f, 20.0f), 32);
		entities.push_back(partner);*/

		
		// TODO(ck): MEMORY MANAGEMENT - Efficient tile map creation
		// TODO(ck): Move all of this into the initTileMap
		world->map = new TileMap();
		world->sheet = {};
		InitTileSheet(&world->sheet, "res/textures/basic_16.png");
		InitTileMap(world->map, &world->sheet);

		// ------------------------------------------------------------------



		return true;
	}

	World2D::World2D()
	{

		// TODO(ck): MEMORY MANAGEMENT - Efficient tile map creation
		// TODO(ck): Move all of this into the initTileMap
		map = new TileMap();
		sheet = {};
		InitTileSheet(&sheet, "res/textures/basic_16.png");
		InitTileMap(map, &sheet);

		// ------------------------------------------------------------------

		// TODO(ck): Memory management
		player = new Entity2D();
		InitEntity(player, "res/textures/ch_witch.png", true, v2(0, 0), 32);
		Entity2D* collider = new Entity2D();
		InitEntity(collider, "res/textures/p1.png", true, v2(50, 50), 32);
		collider->sprite.wireFrame = true;
		//entities.push_back(collider);

		Entity2D* ball = new Entity2D();
		InitEntity(ball, "res/textures/test_house.png", true, v2(75, 84), 96);
		entities.push_back(ball);

		Entity2D* ent = new Entity2D();
		InitEntity(ent, "res/textures/ch_minion.png", true, v2(60, 20), 32);
		entities.push_back(ent);

		for (int i = 0; i < 5; ++i)
		{
			Entity2D* e = new Entity2D();
			InitEntity(e, "res/textures/p1_.png", true,v2(30 * i, 5 * i), 32);
			//entities.push_back(e);
		}

		Entity2D* partner = new Entity2D();
		InitEntity(partner, "res/textures/sRansome.png", true, v2(20.0f, 20.0f), 32);
		entities.push_back(partner);

	}

	World2D::~World2D()
	{
		for (size_t i = 0; i < entities.size(); ++i)
		{
			delete entities[i];
		}
		delete player;
	}
}