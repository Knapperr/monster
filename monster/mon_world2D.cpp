#include "mon_world2D.h"

namespace Mon
{
	bool InitWorld(World2D* world)
	{
		world->selectedEntityIndex = 1;

		// Reserve slot 0 for null entity
		AddEntity2D(world);

		// Reserve slot 1 for player entity
		AddEntity2D(world);
		Entity2D* player = GetPlayer(world);
		InitEntity(player, "player", v2(1, 1), 32);
		// InitPlayer(player, shaderHandle);
		//Entity2D* collider = new Entity2D();
		//initEntity(collider, "res/textures/p1.png", true, v2(50, 50), 32);
		//collider->sprite.wireFrame = true;
		
		AddEntity2D(world);
		Entity2D* ball = GetEntity2D(world, world->entityCount - 1);
		InitEntity(ball, "ball", v2(10, 9), 96);

		AddEntity2D(world);
		Entity2D* e = GetEntity2D(world, world->entityCount - 1);
		InitEntity(e, "ent", v2(4, 5), 32);
		
		AddEntity2D(world);
		Entity2D* minion = GetEntity2D(world, world->entityCount - 1);
		InitMinion(minion, "minion", v2(0, 2), 32);

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
}