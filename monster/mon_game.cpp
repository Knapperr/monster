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

	void SortEntitiesFromCamera(Game* game)
	{

		//for (int i = 1; i < game->world->entityCount; i++)
		//{
		//	Entity* e = GetEntity(game->world, i);
		//	if (e->rb.pos <)
		//}


		//qsort(game->world->entities, game->world->entityCount, sizeof(Entity), sortEntities);
	}
	void UpdateEntities(World* world)
	{
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
		state->mode = Mode::Debug;

		state->shader = {};
		//waterShader = {};
		MonGL::LoadShader(&state->shader, "res/shaders/vert_colors.glsl", "res/shaders/frag_colors.glsl", NULL);
		//MonGL::LoadShader(&waterShader.common, "res/shaders/vert_water.glsl", "res/shaders/frag_water.glsl", NULL);

		state->mainShaderID = state->shader.handle;

		// TODO(ck): MEMORY MANAGEMENT
		state->world = new World();
		InitWorld(state->world, state->shader.handle);


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

		player->rb.pos = newPos;


		if ((player->rb.velocity.x == 0.0f) && (player->rb.velocity.z == 0.0f))
		{
			// NOTE(ck): Leave facingDirection whatever it was 
		}
		else if (absoluteValue(player->rb.velocity.x) > absoluteValue(player->rb.velocity.z))
		{
			if (player->rb.velocity.x > 0)
			{
				player->facingDir = 0; // right
			}
			else
			{
				player->facingDir = 2; // left
			}
		}
		else
		{
			if (player->rb.velocity.z > 0)
			{
				player->facingDir = 1; // back

			}
			else
			{
				player->facingDir = 3; // front
			}

		}
	}

	void Update(GameState* state, double dt, Input* newInput)
	{
		state->deltaTime = dt;

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
		UpdateEntities(state->world);


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

		UpdatePicker(&state->picker, state->terrain, newInput->mouseScreen, ViewMatrix(cam), Projection(cam), cam->pos);

		// DebugModule update?
		RunDebugControls(newInput, &state->picker, state->world, state->selectedIndex);
		//world->player->particle.pos = picker.currentTerrainPoint;

	}

	void Render(GameState* state, double dt)
	{
		Camera* cam = GetCamera(state, state->currCameraIndex);

		MonGL::BeginRender(state->config, Projection(cam), ViewMatrix(cam), state->shader.handle);

		// TODO(ck): use shader
		glUseProgram(state->shader.handle);
		// light & material
		glUniform3fv(glGetUniformLocation(state->shader.handle, "light.pos"), 1, &state->light.pos[0]);
		glUniform3fv(glGetUniformLocation(state->shader.handle, "light.ambient"), 1, &state->light.ambient[0]);
		glUniform3fv(glGetUniformLocation(state->shader.handle, "light.diffuse"), 1, &state->light.diffuse[0]);
		glUniform3fv(glGetUniformLocation(state->shader.handle, "light.specular"), 1, &state->light.specular[0]);



		//
		// TERRAIN
		//
		if (state->drawCollisions)
		{
			MonGL::DrawBoundingBox(&state->terrain->collider.data, cam, state->shader.handle);
		}
		MonGL::DrawTerrain(state->shader.handle, &state->terrain->mesh, &state->light, cam, state->terrain->wireFrame);

		//
		// ENTITIES
		//
		// IMPORTANT(ck): start at index 2 player is being drawn above right now.
		for (int i = 1; i < state->world->entityCount; ++i)
		{
			Entity e = state->world->entities[i];

			if (state->drawCollisions)
				MonGL::DrawBoundingBox(&e.collider.data, cam, state->shader.handle);

			MonGL::Draw(state->config, &e.data, e.rb.pos, cam, state->shader.handle);
		}

		//
		// DEBUG TOOLS
		// 
		// Drawing mouse picker ray for testing
		//MonGL::DrawLine(&picker.data, picker.currentTerrainPoint, shader.handle);

		MonGL::EndRender();

	}

	void CleanUp(GameState* state)
	{
		// IMPORTANT(ck):
		// TODO(ck): clean up render data 
		//glDeleteVertexArrays(1, &world->player->data.VAO);
		//glDeleteBuffers(1, &world->player->data.VBO);
		MonGL::DeleteShader(&state->shader);
	}

	void SetViewPort(GameState* state, int width, int height)
	{
		state->config->viewPort = Rect{ 0.0f, 0.0f, (float)width, (float)height };
	}

	bool Playing(GameState* state)
	{
		return (state->mode == Mode::Play);
	}

	bool Game::init(int windowWidth, int windowHeight, float portWidth, float portHeight)
	{
		mode = Mode::Debug;

		shader = {};
		//waterShader = {};
		MonGL::LoadShader(&shader, "res/shaders/vert_colors.glsl", "res/shaders/frag_colors.glsl", NULL);
		//MonGL::LoadShader(&waterShader.common, "res/shaders/vert_water.glsl", "res/shaders/frag_water.glsl", NULL);

		mainShaderID = shader.handle;
		
		// TODO(ck): MEMORY MANAGEMENT
		world = new World();
		InitWorld(world, shader.handle);
		

		light = {};
		v3 lightColor = v3(0.2f, 0.3f, 0.6f);
		light.diffuse = lightColor * v3(0.5f);
		light.ambient = light.diffuse * v3(0.2f);
		light.specular = v3(1.0f, 1.0f, 1.0f);
		light.pos = v3(16.0f, 10.0f, 16.0f);


		//water = {};
		//MonGL::initDistortedWater(&water.data, &water.setup);
		
		//MonGL::initDistortedWater(water.data, wa)
		//MonGL::initQuad(&water.data);
		//MonGL::loadTexture(&water.data, MonGL::Type::Diffuse, shader.handle, "res/textures/water/uv.png");
		//MonGL::initBoundingBox(&water.colliderData);
		//water.particle.pos = v3(40.0f, 0.1f, 10.0);
		//water.particle.orientation = v3(1.0f, 1.0f, 1.0);

		// TODO(ck): Memory Allocation
		terrain = new Terrain(0, 0);

		simulate = false;
		
		config = new MonGL::Config();
		config->viewPort = { 5.0f, 5.0f, portWidth, portHeight };
		MonGL::ViewPort(&config->viewPort);

		config->angleDegrees = -45.0f;

		// TODO(ck): Memory management
		// reserve slot 0 for NULL camera
		addCamera(this);

		int followCamIndex = addCamera(this);
		InitCamera(&cameras[followCamIndex], CameraType::Follow, "Follow", config->viewPort);

		int debugCamIndex = addCamera(this);
		InitCamera(&cameras[debugCamIndex], CameraType::Fly, "Debug 1", config->viewPort);
		int debugCamIndex2 = addCamera(this);
		InitCamera(&cameras[debugCamIndex2], CameraType::Fly, "Debug 2", config->viewPort);
		
		currCameraIndex = debugCamIndex;
		selectedIndex = 1;
		drawCollisions = true;

		picker = {};
		InitMousePicker(&picker);

		mode = Mode::Debug;

		return true;
	}

	// should velocity be acceleratio?
	void Game::movePlayer(World* world, v3* velocity)
	{
		Entity* player = GetPlayer(world);

		// ddPLength
		float velocityLength = lengthSq(*velocity);
		if (velocityLength > 1.0f)
		{
			*velocity *= (1.0f / squareRoot(velocityLength));
		}

		float speed = player->rb.speed;
		if (input.shift.endedDown)
			speed *= 1.5f;

		*velocity *= speed;

		*velocity += -7.0f * player->rb.velocity;

		
		v3 oldPos = player->rb.pos;
		v3 newPos = oldPos;
		float deltaX = (0.5f * velocity->x * square(deltaTime) + player->rb.velocity.x * deltaTime);
		float deltaY = player->rb.velocity.y;
		//float deltaY = velocity->y * square(deltaTime) + player.particle.velocity.y * deltaTime);
		float deltaZ = (0.5f * velocity->z * square(deltaTime) + player->rb.velocity.z * deltaTime);
		v3 delta = { deltaX, deltaY, deltaZ };

		// TODO(ck): need to set an offest like casey does
		newPos += delta;

		player->rb.velocity.x = velocity->x * deltaTime + player->rb.velocity.x;
		player->rb.velocity.z = velocity->z * deltaTime + player->rb.velocity.z;

		player->rb.pos = newPos;


		if ((player->rb.velocity.x == 0.0f) && (player->rb.velocity.z == 0.0f))
		{
			// NOTE(ck): Leave facingDirection whatever it was 
		}
		else if (absoluteValue(player->rb.velocity.x) > absoluteValue(player->rb.velocity.z))
		{
			if (player->rb.velocity.x > 0)
			{
				player->facingDir = 0; // right
			}
			else
			{
				player->facingDir = 2; // left
			}
		}
		else
		{
			if (player->rb.velocity.z > 0)
			{
				player->facingDir = 1; // back

			}
			else
			{
				player->facingDir = 3; // front
			}

		}
	}

	void Game::update(double dt, Input* newInput)
	{
		if (dt > deltaTime)
			printf("HIGHER dt: %f\n", deltaTime);
		if (dt < deltaTime)
			printf("LOWER dt: %f\n", deltaTime);

		deltaTime = dt;
		input = *newInput;

		//
		// Input start
		//
		// TODO(ck): TEMP INPUT
		if (input.debug.endedDown)
		{
			if (mode == Mode::Debug)
			{
				playMode();
			}
			else if (mode == Mode::Play)
			{
				debugMode();
			}
		}
		
		//
		// CAMERA
		//
		if (input.num1.endedDown) 
		{ 
			currCameraIndex = 1;
			playMode();
		}
		if (input.num2.endedDown) 
		{ 
			currCameraIndex = 2; 
			debugMode();
		}
		if (input.num3.endedDown)
		{ 
			currCameraIndex = 3;
			debugMode();
		}
		
		bool enabled = (mode == Mode::Play);
		if (enabled)
		{
			v3 velocity = {};

			if (input.isAnalog)
			{
				velocity = v3{ input.stickAverageX, 0.0f ,input.stickAverageY };
			}
			else
			{
				if (input.up.endedDown)
					velocity.z = -1.0f;
				if (input.down.endedDown)
					velocity.z = 1.0f;
				if (input.left.endedDown)
					velocity.x = -1.0f;
				if (input.right.endedDown)
					velocity.x = 1.0f;
			}
			// PIPE velocity to function
			movePlayer(world, &velocity);



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
		

		Entity* player = GetPlayer(world);
		// player collider
		v3 colliderPos = { player->rb.pos.x - (0.5f),
					player->rb.pos.y - (0.5),
					player->rb.pos.z - (0.5f) };

		//UpdateCollider(&player->collider, colliderPos, player->data.scale);

		//
		//	ENTITIES UPDATE
		//
		UpdateEntities(world);


		// 
		// CAMERA UPDATE
		//
		Camera* cam = getCamera(this, currCameraIndex);
		Update(cam, deltaTime, &input,
			   player->rb.pos, player->rb.orientation, true);
		// sort the entities from the camera
		//std::sort(world->entities, world->entities + world->entityCount, sortEntities);
		

		//
		// MOUSE PICKER
		//
		// TODO(ck): Only update the picker if we are in "picking" mode
		
		UpdatePicker(&picker, terrain, input.mouseScreen, ViewMatrix(cam), Projection(cam), cam->pos);
	
		// DebugModule update?
		RunDebugControls(&input, &picker, world, selectedIndex);
		//world->player->particle.pos = picker.currentTerrainPoint;

		
	}

	void Game::render(double dt)
	{
		Camera* cam = getCamera(this, currCameraIndex);
		
		MonGL::BeginRender(config, Projection(cam), ViewMatrix(cam), shader.handle);

		// TODO(ck): use shader
		glUseProgram(shader.handle);
		// light & material
		glUniform3fv(glGetUniformLocation(shader.handle, "light.pos"), 1, &light.pos[0]);
		glUniform3fv(glGetUniformLocation(shader.handle, "light.ambient"), 1, &light.ambient[0]);
		glUniform3fv(glGetUniformLocation(shader.handle, "light.diffuse"), 1, &light.diffuse[0]);
		glUniform3fv(glGetUniformLocation(shader.handle, "light.specular"), 1, &light.specular[0]);
		
		

		//
		// TERRAIN
		//
		if (drawCollisions)
		{
			MonGL::DrawBoundingBox(&terrain->collider.data, cam, shader.handle);	
		}
		MonGL::DrawTerrain(shader.handle, &terrain->mesh, &light, cam, terrain->wireFrame);
		
		//
		// ENTITIES
		//
		// IMPORTANT(ck): start at index 2 player is being drawn above right now.
		for (int i = 1; i < world->entityCount; ++i)
		{
			Entity e = world->entities[i];

			if (drawCollisions)
				MonGL::DrawBoundingBox(&e.collider.data, cam, shader.handle);
			
			MonGL::Draw(config, &e.data, e.rb.pos, cam, shader.handle);
		}

		//
		// DEBUG TOOLS
		// 
		// Drawing mouse picker ray for testing
		//MonGL::DrawLine(&picker.data, picker.currentTerrainPoint, shader.handle);

		MonGL::EndRender();
	}

	void Game::cleanUp()
	{
		// IMPORTANT(ck):
		// TODO(ck): clean up render data 
		//glDeleteVertexArrays(1, &world->player->data.VAO);
		//glDeleteBuffers(1, &world->player->data.VBO);
		MonGL::DeleteShader(&shader);
	}

	///
	/// Utility 
	///

	void Game::setViewPort(int width, int height)
	{
		config->viewPort = Rect{ 0.0f, 0.0f, (float)width, (float)height };
	}

	bool Game::playing()
	{
		return (mode == Mode::Play);
	}

	void Game::playMode()
	{
		currCameraIndex = 1;
		mode = Mode::Play;
	}

	void Game::debugMode()
	{
		if (currCameraIndex != 1)
			mode = Mode::Debug;
	}
}