#include "mon_game.h"

#include "handmade_random.h"

#include <algorithm>


namespace Mon
{
	// Should this be part of the gui???
	void RunDebugControls(Input* input, MousePicker* picker, World* world, int &selectedIndex)
	{

		// Get the selected index from the GUI
		Entity* ent = {};
		if (selectedIndex > 0)
			ent = GetEntity(world, selectedIndex);

		if ((input->lMouseBtn.endedDown == false && input->rMouseBtn.endedDown) && ent != nullptr)
		{
			ent->rb.pos.x = picker->currentTerrainPoint.x;
			ent->rb.pos.z = picker->currentTerrainPoint.z;
			ent->rb.pos.y += input->wheel.y * 0.35f;
		}
	}

	void PlayMode(GameState* state)
	{
		state->currCameraIndex = 1;
		state->mode = Mode::Play;
	}

	void DebugMode(GameState* state)
	{
		if (state->currCameraIndex != 1)
			state->mode = Mode::Debug;
	}

	void UpdateCollider(Collider* collider, v3 colliderPos, v3 scale)
	{
		SetBoxTransform(collider, colliderPos, scale);
		UpdateWorldPosToWorldMatrix(collider);
	}

	//bool sortEntities(Entity leftEnt, Entity rightEnt)
	//{
	//	//Camera* cam = getCamera(this, currCameraIndex);


	//	float distToCameraA = glm::distance(leftEnt.rb.pos.z, 10.0f);
	//	float distToCameraB = glm::distance(rightEnt.rb.pos.z, 1.0f);
	//	return (distToCameraA > distToCameraB);
	//}

	void SortEntitiesFromCamera(GameState* game)
	{

		//for (int i = 1; i < game->world->entityCount; i++)
		//{
		//	Entity* e = GetEntity(game->world, i);
		//	if (e->rb.pos <)
		//}


		//qsort(game->world->entities, game->world->entityCount, sizeof(Entity), sortEntities);
	}
	void UpdateEntities(World* world, float dt)
	{
		// update minion around for fun

		// if direction left and less than 0
		// keep going left
		// set if 0 or 100 hit
		Entity* e = GetEntity(world, 16);
		float speed = 4.0f;

		if (e->rb.pos.x >= 40.0f && e->facingDir == Direction::Right)
		{	
			e->facingDir = Direction::Left;
		}
		if(e->rb.pos.x <= 0.0f && e->facingDir == Direction::Left)
		{
			e->facingDir = Direction::Right;
		}

		if (e->facingDir == Direction::Left)
			speed = -speed;

		e->rb.pos.x += speed * dt;

		// TODO(ck): Update entity and then update entity collider right after
		// instead of having two separate loops for entities and their colliders.
		for (unsigned int i = 1; i < world->entityCount; ++i)
		{
			// TODO(ck): Broad Phase Collision Check

			// TODO(ck): Precise Collision check
			
			v3 colliderPos = { world->entities[i].rb.pos.x - (0.5f),
								world->entities[i].rb.pos.y - (0.5),
								world->entities[i].rb.pos.z - (0.5f) };

			UpdateCollider(&world->entities[i].collider, colliderPos, world->entities[i].data.scale);
		}
	}

	bool InitGame(GameState* state, int windowWidth, int windowHeight, float portWidth, float portHeight)
	{
		//
		// Initialize Assets for game here
		//
		InitAssets(g_Assets);


		state->mode = Mode::Debug;
		state->renderer = {};
		MonGL::InitRenderer(&state->renderer);

		state->light = {};
		v3 lightColor = v3(0.2f, 0.3f, 0.6f);
		state->light.diffuse = lightColor * v3(0.5f);
		state->light.ambient = state->light.diffuse * v3(0.2f);
		state->light.specular = v3(1.0f, 1.0f, 1.0f);
		state->light.pos = v3(16.0f, 10.0f, 16.0f);


		//water = {};
		//MonGL::initDistortedWater(&water.data, &water.setup);

		//MonGL::initDistortedWater(water.data, wa)
		//MonGL::initQuad(&water.data);
		//MonGL::loadTexture(&water.data, MonGL::Type::Diffuse, shader.handle, "res/textures/water/uv.png");
		//MonGL::initBoundingBox(&water.colliderData);
		//water.particle.pos = v3(40.0f, 0.1f, 10.0);
		//water.particle.orientation = v3(1.0f, 1.0f, 1.0);

		// TODO(ck): Memory Allocation
		state->terrain = new Terrain(0, 0);

		state->simulate = false;

		state->config = new MonGL::Config();
		state->config->viewPort = { 5.0f, 5.0f, portWidth, portHeight };
		MonGL::ViewPort(&state->config->viewPort);

		state->config->angleDegrees = -45.0f;

		// TODO(ck): Memory management
		// reserve slot 0 for NULL camera
		AddCamera(state);

		int followCamIndex = AddCamera(state);
		InitCamera(&state->cameras[followCamIndex], CameraType::Follow, "Follow", state->config->viewPort);

		int debugCamIndex = AddCamera(state);
		InitCamera(&state->cameras[debugCamIndex], CameraType::Fly, "Debug 1", state->config->viewPort);
		int debugCamIndex2 = AddCamera(state);
		InitCamera(&state->cameras[debugCamIndex2], CameraType::Fly, "Debug 2", state->config->viewPort);

		state->currCameraIndex = debugCamIndex;
		state->selectedIndex = 1;
		state->drawCollisions = true;

		state->picker = {};
		InitMousePicker(&state->picker);

		state->mode = Mode::Debug;


		// debug stuff
		MonGL::InitLine(&state->lineOne);
		MonGL::InitLine(&state->lineTwo);

		// Init the game world
		// TODO(ck): MEMORY MANAGEMENT
		state->world = new World();
		InitWorld(state->world, state->renderer.program.handle, state->renderer.waterProgram.common.handle, state->config->angleDegrees);

		return true;
	}

	void MovePlayer(World* world, v3* velocity, float dt)
	{
		Entity* player = GetPlayer(world);

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

		*velocity += -7.0f * player->rb.velocity;


		v3 oldPos = player->rb.pos;
		v3 newPos = oldPos;
		float deltaX = (0.5f * velocity->x * square(dt) + player->rb.velocity.x * dt);
		float deltaY = player->rb.velocity.y;
		//float deltaY = velocity->y * square(deltaTime) + player.particle.velocity.y * deltaTime);
		float deltaZ = (0.5f * velocity->z * square(dt) + player->rb.velocity.z * dt);
		v3 delta = { deltaX, deltaY, deltaZ };

		// TODO(ck): need to set an offest like casey does
		newPos += delta;

		player->rb.velocity.x = velocity->x * dt + player->rb.velocity.x;
		player->rb.velocity.z = velocity->z * dt + player->rb.velocity.z;


		// Need to update the collider of the player first because we are using the position here
		// maybe not?
		Entity* minion = GetEntity(world, 16);
		if (TestAABBAABB(player->collider, minion->collider))
			minion->rb.speed = 1.0f;
		else
			minion->rb.speed = 40.0f;
		

		player->rb.pos = newPos;
		SetFacingDirection(player);
	}

	void Update(GameState* state, double dt, Input* newInput)
	{
		state->deltaTime = dt;
		state->input = *newInput;
		//
		// Input start
		//
		// TODO(ck): TEMP INPUT
		if (newInput->debug.endedDown)
		{
			if (state->mode == Mode::Debug)
			{
				PlayMode(state);
			}
			else if (state->mode == Mode::Play)
			{
				DebugMode(state);
			}
		}

		//
		// CAMERA
		//
		if (newInput->num1.endedDown)
		{
			state->currCameraIndex = 1;
			PlayMode(state);
		}
		if (newInput->num2.endedDown)
		{
			state->currCameraIndex = 2;
			DebugMode(state);
		}
		if (newInput->num3.endedDown)
		{
			state->currCameraIndex = 3;
			DebugMode(state);
		}

		bool enabled = (state->mode == Mode::Play);
		if (enabled)
		{
			v3 velocity = {};

			if (newInput->isAnalog)
			{
				velocity = v3{ newInput->stickAverageX, 0.0f ,newInput->stickAverageY };
			}
			else
			{
				if (newInput->up.endedDown)
					velocity.z = -1.0f;
				if (newInput->down.endedDown)
					velocity.z = 1.0f;
				if (newInput->left.endedDown)
					velocity.x = -1.0f;
				if (newInput->right.endedDown)
					velocity.x = 1.0f;
			}
			// PIPE velocity to function
			MovePlayer(state->world, &velocity, dt);



		}
		//player.particle.velocity *= player.particle.speed;
		//player.particle.pos.x += player.particle.velocity.x * dt;
		//player.particle.pos.z += player.particle.velocity.z * dt;

		//input.mouseOffset.x -= config->viewPort.x;
		//input.mouseOffset.y -= config->viewPort.y;
		//int xOffset = (1440 - 960) / 2;
		//int yOffset = (900 - 540) / 2;

		//float x = (2.0f * (mouseX)) / (float)displayWidth - 1.0f;
		//float y = (2.0f * (mouseY - yOffset)) / (float)displayHeight - 1.0f;
		//input.mouseOffset.x -= config->viewPort.x;
		//input.mouseOffset.y -= config->viewPort.y;
		//cam->update(deltaTime, &input, world->player->particle.pos, world->player->particle.orientation, true);


		Entity* player = GetPlayer(state->world);
		// player collider
		v3 colliderPos = { player->rb.pos.x - (0.5f),
					player->rb.pos.y - (0.5),
					player->rb.pos.z - (0.5f) };

		//UpdateCollider(&player->collider, colliderPos, player->data.scale);

		//
		//	ENTITIES UPDATE
		//
		UpdateEntities(state->world, dt);


		// 
		// CAMERA UPDATE
		//
		Camera* cam = GetCamera(state, state->currCameraIndex);
		Update(cam, dt, newInput, player->rb.pos, player->rb.orientation, true);
		// sort the entities from the camera
		//std::sort(world->entities, world->entities + world->entityCount, sortEntities);


		//
		// MOUSE PICKER
		//
		// TODO(ck): Only update the picker if we are in "picking" mode
		MonGL::Mesh* grid = MonGL::GetMesh(g_Assets, state->terrain->data.meshIndex);
		UpdatePicker(&state->picker, grid, newInput->mouseScreen, ViewMatrix(cam), Projection(cam), cam->pos);

		// DebugModule update?
		// DebugManager update?
		RunDebugControls(newInput, &state->picker, state->world, state->selectedIndex);
		//world->player->particle.pos = picker.currentTerrainPoint;

	}

	void Render(GameState* state, double dt)
	{
		Camera* cam = GetCamera(state, state->currCameraIndex);
		mat4 projection = Projection(cam);
		mat4 viewMatrix = ViewMatrix(cam);


		//MonGL::UseProgram(&state->renderer.waterProgram, state->setup);
		//MonGL::BeginRender(state->config, projection, viewMatrix, state->renderer.waterProgram.common.handle);
		
		// TODO(ck): need to make it a normal Draw(); just have to pass program and activate uniforms
		// in it no matter what type it is in draw? it like unwinds??? 
		// DrawWater()
		

		// 
		// TODO(ck): use shader
		MonGL::UseProgram(&state->renderer.program, state->setup);

		// TODO(ck): Remove begin render need to go through shaders after another
		MonGL::BeginRender(state->config, projection, viewMatrix, state->renderer.program.handle);
		//MonGL::BeginRender(state->config, Projection(cam), ViewMatrix(cam), state->waterShader.handle);

		//
		// TERRAIN
		//
		MonGL::DrawTerrain(&state->renderer, &state->terrain->data, &state->light, cam);

		//
		// ENTITIES
		//
		// IMPORTANT(ck): start at index 2 player is being drawn above right now.
		for (int i = 1; i < state->world->entityCount; ++i)
		{
			Entity e = state->world->entities[i];

			if (state->drawCollisions)
				MonGL::DrawBoundingBox(&state->renderer, &e.collider.data, cam);
			MonGL::Draw(&state->renderer, state->config, e.spriteAngleDegrees, &e.data, e.rb.pos, cam);
		}

		//
		// DEBUG TOOLS
		// 
		//MonGL::DrawLine(&state->renderer, &state->lineOne);
		//MonGL::DrawLine(&state->renderer, &state->lineTwo);

		MonGL::EndRender();

	}

	void CleanUp(GameState* state)
	{
		// IMPORTANT(ck):
		// TODO(ck): clean up render data 
		//glDeleteVertexArrays(1, &world->player->data.VAO);
		//glDeleteBuffers(1, &world->player->data.VBO);
		delete state->terrain;
		MonGL::DeleteShader(&state->renderer.program);
	}

	void SetViewPort(GameState* state, int width, int height)
	{
		state->config->viewPort = Rect{ 0.0f, 0.0f, (float)width, (float)height };
	}

	bool Playing(GameState* state)
	{
		return (state->mode == Mode::Play);
	}
}