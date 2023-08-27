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
			ent->rb.worldPos.x = picker->currentTerrainPoint.x;
			ent->rb.worldPos.z = picker->currentTerrainPoint.z;

			float offset = 0.35f;
			if (input->shift.endedDown)
				offset = 0.10f;
			ent->rb.worldPos.y += input->wheel.y * offset;
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

	void UpdateEntities(World* world, MonGL::OpenGL* gl, float dt)
	{
		// update minion around for fun

		// if direction left and less than 0
		// keep going left
		// set if 0 or 100 hit
		Entity* e = GetEntity(world, 16);
		float speed = 4.0f;

		if (e->rb.worldPos.x >= 20.0f && e->facingDir == Direction::Right)
		{	
			e->facingDir = Direction::Left;
		}
		if(e->rb.worldPos.x <= -20.0f && e->facingDir == Direction::Left)
		{
			e->facingDir = Direction::Right;
		}

		if (e->facingDir == Direction::Left)
			speed = -speed;
		
		e->rb.worldPos.x += speed * dt;

		// TODO(ck): Update entity and then update entity collider right after
		// instead of having two separate loops for entities and their colliders.
		for (unsigned int i = 1; i < world->entityCount; ++i)
		{
			// TODO(ck): Broad Phase Collision Check

			// TODO(ck): Precise Collision check

			SetBoxTransform(&world->entities[i].collider, world->entities[i].rb.worldPos, world->entities[i].data.scale);
			world->entities[i].data.pos = world->entities[i].rb.worldPos;
			world->entities[i].data.angleDegrees = world->entities[i].spriteAngleDegrees;
		}
	}

	void InitCameras(GameState* state)
	{
		// TODO(ck): Memory management
		// reserve slot 0 for NULL camera
		AddCamera(state);

		int followCamIndex = AddCamera(state);
		InitCamera(&state->cameras[followCamIndex], CameraType::Follow, "Follow", state->config.viewPort);

		int debugCamIndex = AddCamera(state);
		InitCamera(&state->cameras[debugCamIndex], CameraType::Fly, "Debug 1", state->config.viewPort);
		int debugCamIndex2 = AddCamera(state);
		InitCamera(&state->cameras[debugCamIndex2], CameraType::Fly, "Debug 2", state->config.viewPort);

		int followCam2Index = AddCamera(state);
		InitCamera(&state->cameras[followCam2Index], CameraType::Follow, "Follow 2 (better?)", state->config.viewPort);
		state->cameras[followCam2Index].FOV = 17.0f;
		state->cameras[followCam2Index].pitch = 45.0f;

		int followCam3Index = AddCamera(state);
		InitCamera(&state->cameras[followCam3Index], CameraType::Follow, "Really top down.. easier to see?", state->config.viewPort);
		state->cameras[followCam3Index].FOV = 17.0f;
		state->cameras[followCam3Index].pitch = 65.0f;

		state->currCameraIndex = debugCamIndex;
	}

	bool InitGame(GameMemory* memory, int windowWidth, int windowHeight, float portWidth, float portHeight)
	{
		GameState* state = (GameState *)memory->permanentStorage;
		if (state->initialized)
			return true;

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
		// Putting this right after the game_state in the storaged that is already
		// allocated
		// NOTE(ck): First push to the memory !
		InitializeArena(&state->assetArena, memory->permanentStorageSize - sizeof(GameState),
						(uint8_t*)memory->permanentStorage + sizeof(GameState));
		g_Assets = PushStruct(&state->assetArena, Assets);
		InitAssets(g_Assets);

		state->mode = Mode::Debug;
		
		// TODO(ck): REMOVE TESTING FOR GUI
		state->selectedSubTextureIndex = 1;

		// TODO(ck): Memory Allocation for Renderer
		// allocate renderer
		state->renderer =  new MonGL::OpenGL();
		MonGL::InitRenderer(state->renderer);

		state->setup = {};
		state->setup.materialShininess = 64.0f;

		// TODO(ck): Memory Allocation Move this to World
		// allocate grid
		state->grid = new Grid();
		InitGrid(state->grid);

		state->grid_ = new Grid_();
		InitGrid(state->grid_);

		state->simulate = false;

		state->config = {};
		state->config.viewPort = { 5.0f, 5.0f, portWidth, portHeight };
		MonGL::ViewPort(&state->config.viewPort);

		state->config.spriteAngleDegrees = -45.0f;

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
		InitWorld(state->world, state->renderer->program.handle, state->renderer->waterProgram.common.handle, state->config.spriteAngleDegrees);


		state->initialized = true;
		return state->initialized;
	}

	void Update(GameMemory* memory, double dt, Input* newInput)
	{
		GameState* state = (GameState *)memory->permanentStorage;

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
				state->currCameraIndex = 4;
				PlayMode(state);
			}
			else if (state->mode == Mode::Play)
			{
				state->currCameraIndex = 2;
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

		//
		//	ENTITIES UPDATE
		//
		UpdateEntities(state->world, state->renderer, dt);
		
		// 
		// CAMERA UPDATE
		//
		Entity* player = GetPlayer(state->world);
		Camera* cam = GetCamera(state, state->currCameraIndex);
		Update(cam, dt, newInput, player->rb.worldPos, player->rb.orientation, true);
		// sort the entities from the camera
		//std::sort(world->entities, world->entities + world->entityCount, sortEntities);

		//
		// MOUSE PICKER
		//
		// TODO(ck): Only update the picker if we are in "picking" mode
		MonGL::Mesh* grid = MonGL::GetMesh(g_Assets, state->grid->data.meshIndex);
		UpdatePicker(&state->picker, grid, newInput->mouseScreen, ViewMatrix(cam), Projection(cam), cam->pos);
		//
		// DEBUG 
		//
		RunDebugControls(newInput, &state->picker, state->world, state->selectedIndex);
	}

	void Render(GameMemory* memory, float time, double dt)
	{
		GameState* state = (GameState*)memory->permanentStorage;

		Camera* cam = GetCamera(state, state->currCameraIndex);
		mat4 projection = Projection(cam);
		mat4 viewMatrix = ViewMatrix(cam);
	 

		// TODO(ck): UseProgram should be called only one time before switching shaders
		//			 DO NOT CALL every time you draw an entity glUseProgram is expensive
		MonGL::UseProgram(&state->renderer->program, state->setup);

		MonGL::BeginRender(state->renderer, &state->config, projection, viewMatrix, state->renderer->program.handle);
		state->setup.projection = projection;
		state->setup.viewMatrix = viewMatrix;
		state->setup.time = time;
		
		//
		// TERRAIN
		//

		// TODO(ck): shader fixes
		// 1. grid/terrain needs its own shader anyways
		// 2. using cubemap program inside here 
		// 3. start using main obj shader here go back to main shader
		
		// 1. grid/terrain needs its own shader anyways
		MonGL::DrawTerrain(state->renderer, &state->grid->data, cam);
		// 2. using cubemap program inside here
		MonGL::DrawCubeMap(state->renderer, state->setup);
		// 3. start using main obj shader here go back to main shader
 		MonGL::UseProgram(&state->renderer->program, state->setup);


		//
		// PRE RENDER go through entities
		// 
		for (unsigned int i = 1; i < state->world->entityCount; ++i)
		{
			Entity e = state->world->entities[i];

			if (state->drawCollisions)
				MonGL::DrawBoundingBox(state->renderer, &e.collider.data, cam);
			
			if(e.flags & EntityRenderFlag::Sprite)
			{
				//init batch item
				MonGL::BatchItem batchItem;
				if (e.isPlayer) 
					batchItem.isPlayer = e.isPlayer;

				batchItem.worldPos = e.rb.worldPos;
				batchItem.tileSize = 32;
				batchItem.spriteSize = 1.0f;
				batchItem.animationIndex = e.spriteAnimationIndex;
				batchItem.cameraZ = cam->pos.z;
				state->renderer->batchItems_.push_back(batchItem);
			}
			if (e.flags & EntityRenderFlag::Model)
			{
				// init and add render data
				state->renderer->renderItems_.push_back(e.data);
			}
		}

		MonGL::SortBatch(state->renderer->batchItems_);

		// 
		// 3D Models
		//
		for (int i = 0; i < state->renderer->renderItems_.size(); ++i)
		{
			MonGL::RenderData data = state->renderer->renderItems_[i];
			MonGL::Draw(state->renderer, &state->config, state->setup, data.angleDegrees, &data, data.pos, cam);
		}
		// 
		// end common shader rendering
		//

		// 
		// SPRITE BATCH
		//
		MonGL::UseProgram(&state->renderer->quadProgram, state->setup);
		
		MonGL::Batch* batch = MonGL::GetBatch(state->renderer, 1);

		for (int i = 0; i < state->renderer->batchItems_.size(); ++i)
		{
			// TEMP update player animation
			MonGL::BatchItem item = state->renderer->batchItems_[i];
			if (item.isPlayer)
			{
				MonGL::GLSpriteAnimation* anim = &state->renderer->spriteAnimators[2].animations[item.animationIndex];

				MonGL::UpdateSpriteAnimation(anim, 1, (float)dt);

				MonGL::GLSubTexture* subTexture = &anim->frames[anim->frameIndex].subTexture;//&animator->animations[item.animationIndex].frames[state->selectedSubTextureIndex].subTexture;
				MonGL::FillBatch(batch, item.worldPos.x, item.worldPos.y, item.worldPos.z, 
								 cam->pos.x, cam->pos.y, cam->pos.z,
								 subTexture, 32);
				continue;
			}
		
			MonGL::GLSpriteAnimation* anim = &state->renderer->spriteAnimators[1].animations[item.animationIndex];
			
			// NOTE(ck): Running faster because its getting updated each time the batch is filled
			//MonGL::UpdateSpriteAnimation(anim, 1, (float)dt);

			MonGL::GLSubTexture* subTexture = &anim->frames[1].subTexture;//&animator->animations[item.animationIndex].frames[state->selectedSubTextureIndex].subTexture;
			MonGL::FillBatch(batch, item.worldPos.x, item.worldPos.y, item.worldPos.z,
							 cam->pos.x, cam->pos.y, cam->pos.z,
							 subTexture, 32);
		}

		MonGL::BindBatchVertices(batch);
		MonGL::DrawBatch(state->renderer, batch, cam->right, cam->worldUp);

		//
		// DEBUG TOOLS
		// 
		//MonGL::DrawLine(&state->renderer, &state->lineOne);
		//MonGL::DrawLine(&state->renderer, &state->lineTwo);
		//DrawDebugInfo(); //MonGL:: calls inside 
		MonGL::UseProgram(&state->renderer->program, state->setup);
		MonGL::DrawLights(state->renderer);

		MonGL::EndRender(state->renderer);
	}

	void CleanUp(GameState* state)
	{
		// IMPORTANT(ck):
		// TODO(ck): clean up render data 
		//glDeleteVertexArrays(1, &world->player->data.VAO);
		//glDeleteBuffers(1, &world->player->data.VBO);
		delete state->grid;
		MonGL::DeleteShader(&state->renderer->program);
	}

	void SetViewPort(GameState* state, int width, int height)
	{
		state->config.viewPort = Rect{ 0.0f, 0.0f, (float)width, (float)height };
	}

	bool Playing(GameState* state)
	{
		return (state->mode == Mode::Play);
	}
}