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

		if ((!input->lMouseBtn.endedDown && input->rMouseBtn.endedDown) && ent != nullptr)
		{
			ent->rb.pos.x = picker->currentTerrainPoint.x;
			ent->rb.pos.z = picker->currentTerrainPoint.z;

			float offset = 0.35f;
			if (input->shift.endedDown)
				offset = 0.10f;
			ent->rb.pos.y += input->wheel.y * offset;
		}
	}

	void PlayMode(GameState* state)
	{
		state->mode = Mode::Play;
	}

	void DebugMode(GameState* state)
	{
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

		if (e->rb.pos.x >= 20.0f && e->facingDir == Direction::Right)
		{	
			e->facingDir = Direction::Left;
		}
		if(e->rb.pos.x <= -20.0f && e->facingDir == Direction::Left)
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
								world->entities[i].rb.pos.y - (0.5f),
								world->entities[i].rb.pos.z - (0.5f) };

			UpdateCollider(&world->entities[i].collider, colliderPos, world->entities[i].data.scale);
		}
	}

	void InitCameras(GameState* state)
	{
		// TODO(ck): Memory management
		// reserve slot 0 for NULL camera
		AddCamera(state);

		int followCamIndex = AddCamera(state);
		InitCamera(&state->cameras[followCamIndex], CameraType::Follow, "Follow", state->config->viewPort);

		int debugCamIndex = AddCamera(state);
		InitCamera(&state->cameras[debugCamIndex], CameraType::Fly, "Debug 1", state->config->viewPort);
		int debugCamIndex2 = AddCamera(state);
		InitCamera(&state->cameras[debugCamIndex2], CameraType::Fly, "Debug 2", state->config->viewPort);

		int followCam2Index = AddCamera(state);
		InitCamera(&state->cameras[followCam2Index], CameraType::Follow, "Follow 2 (better?)", state->config->viewPort);
		state->cameras[followCam2Index].zoom = 40.0f;
		state->cameras[followCam2Index].pitch = 45.0f;

		state->currCameraIndex = debugCamIndex;
	}

	bool InitGame(GameState* state, int windowWidth, int windowHeight, float portWidth, float portHeight)
	{
		//
		// Initialize Assets for game here
		//
		// NOTE(ck): MUST BE CALLED FIRST - LOADS ALL ASSETS FOR GAME
		
		// TODO(ck): Memory allocations
		// memory to be allocated
		// g_Assets 
		// state->renderer
		// state->grid
		// cameras
		// world

		// allocate assets
		InitAssets(g_Assets);

		state->mode = Mode::Debug;
		// allocate renderer
		state->renderer = {};
		MonGL::InitRenderer(&state->renderer);

		state->setup = {};
		state->setup.materialShininess = 64.0f;

		// TODO(ck): Memory Allocation
		// allocate grid
		state->grid = new Grid();
		InitGrid(state->grid);

		state->simulate = false;

		state->config = new MonGL::Config();
		state->config->viewPort = { 5.0f, 5.0f, portWidth, portHeight };
		MonGL::ViewPort(&state->config->viewPort);

		state->config->spriteAngleDegrees = -45.0f;

		InitCameras(state);

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
		// allocate world
		state->world = new World();
		InitWorld(state->world, state->renderer.program.handle, state->renderer.waterProgram.common.handle, state->config->spriteAngleDegrees);

		return true;
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
		if (newInput->num4.endedDown)
		{
			state->currCameraIndex = 4;
			PlayMode(state);
		}

		bool enabled = (state->mode == Mode::Play);
		if (enabled)
		{
			v3 velocity = {};
			bool jumped = false;
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
				if (newInput->space.endedDown)
					jumped = true;

			}
			// PIPE velocity to function
			MovePlayer(state->world, &velocity, jumped, dt);



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
		MonGL::Mesh* grid = MonGL::GetMesh(g_Assets, state->grid->data.meshIndex);
		UpdatePicker(&state->picker, grid, newInput->mouseScreen, ViewMatrix(cam), Projection(cam), cam->pos);

		// DebugModule update?
		// DebugManager update?
		RunDebugControls(newInput, &state->picker, state->world, state->selectedIndex);
		//world->player->particle.pos = picker.currentTerrainPoint;



	}

	void Render(GameState* state, float time, double dt)
	{
		Camera* cam = GetCamera(state, state->currCameraIndex);
		mat4 projection = Projection(cam);
		mat4 viewMatrix = ViewMatrix(cam);

	 
		// TODO(ck): UseProgram should be called only one time before switching shaders
		//			 DO NOT CALL every time you draw an entity glUseProgram is expensive
		MonGL::UseProgram(&state->renderer.program, state->setup);

		MonGL::BeginRender(&state->renderer, state->config, projection, viewMatrix, state->renderer.program.handle);
		state->setup.projection = projection;
		state->setup.viewMatrix = viewMatrix;
		state->setup.time = time;
		
		//
		// TERRAIN
		//
		MonGL::DrawTerrain(&state->renderer, &state->grid->data, cam);

		//
		// ENTITIES
		//
		for (unsigned int i = 1; i < state->world->entityCount; ++i)
		{
			Entity e = state->world->entities[i];

			if (state->drawCollisions)
				MonGL::DrawBoundingBox(&state->renderer, &e.collider.data, cam);
			MonGL::Draw(&state->renderer, state->config, state->setup, e.spriteAngleDegrees, &e.data, e.rb.pos, cam);

			GetGridPosition(e.rb.pos);
		}

		//
		// DEBUG TOOLS
		// 
		//MonGL::DrawLine(&state->renderer, &state->lineOne);
		//MonGL::DrawLine(&state->renderer, &state->lineTwo);
		//DrawDebugInfo(); //MonGL:: calls inside 
		MonGL::DrawLights(&state->renderer);

		MonGL::DrawCubeMap(&state->renderer, state->setup);

		MonGL::EndRender();
	}

	void CleanUp(GameState* state)
	{
		// IMPORTANT(ck):
		// TODO(ck): clean up render data 
		//glDeleteVertexArrays(1, &world->player->data.VAO);
		//glDeleteBuffers(1, &world->player->data.VBO);
		delete state->grid;
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