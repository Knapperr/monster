#include "mon_game.h"

#include "handmade_random.h"

namespace Mon
{
	void RunDebugControls(Input* input, MousePicker* picker, World* world, int selectedIndex)
	{
		// Get the selected index from the GUI
		Entity* ent = {};
		if (selectedIndex > 0)
			ent = GetEntity(world, selectedIndex);

		if ((input->lMouseBtn.endedDown == false && input->rMouseBtn.endedDown) && ent != nullptr)
		{
			ent->rb.pos = picker->currentTerrainPoint;
		}
	}


	bool Game::init(int windowWidth, int windowHeight, float portWidth, float portHeight)
	{
		state = State::Debug;

		shader = {};
		//waterShader = {};
		MonGL::LoadShader(&shader, "res/shaders/vert_colors.glsl", "res/shaders/frag_colors.glsl", NULL);
		//MonGL::LoadShader(&waterShader.common, "res/shaders/vert_water.glsl", "res/shaders/frag_water.glsl", NULL);

		mainShaderID = shader.handle;
		
		// TODO(ck): MEMORY MANAGEMENT
		world = new World();
		InitWorld(world, shader.handle);
		InitPlayer(world, shader.handle);

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

		state = State::Debug;

		// Debug cube entity
		//AddEntity(world);
		//Entity* debugEnt = GetEntity(world, world->entityCount - 1);
		debugEnt.setup = {};
		debugEnt.name = "debug_cube";
		MonGL::InitCube(&debugEnt.data);
		MonGL::LoadTexture(&debugEnt.data, 0, MonGL::TextureType::Diffuse, shader.handle, "res/textures/container2.png");
		debugEnt.rb.pos = v3(24.0f, 0.2f, 10.0f);
		InitBoxCollider(&debugEnt.collider);
		// =======================================

		picker = {};

		return true;
	}

	// TODO(ck): Should this param be pointer?
	// should velocity be acceleratio?
	void Game::movePlayer(v3* velocity)
	{
		// ddPLength
		float velocityLength = lengthSq(*velocity);
		if (velocityLength > 1.0f)
		{
			*velocity *= (1.0f / squareRoot(velocityLength));
		}
		*velocity *= world->player->rb.speed;

		*velocity += -7.0f * world->player->rb.velocity;

		
		v3 oldPos = world->player->rb.pos;
		v3 newPos = oldPos;
		float deltaX = (0.5f * velocity->x * square(deltaTime) + world->player->rb.velocity.x * deltaTime);
		float deltaY = world->player->rb.velocity.y;
		//float deltaY = velocity->y * square(deltaTime) + player.particle.velocity.y * deltaTime);
		float deltaZ = (0.5f * velocity->z * square(deltaTime) + world->player->rb.velocity.z * deltaTime);
		v3 delta = { deltaX, deltaY, deltaZ };

		// TODO(ck): need to set an offest like casey does
		newPos += delta;

		world->player->rb.velocity.x = velocity->x * deltaTime + world->player->rb.velocity.x;
		world->player->rb.velocity.z = velocity->z * deltaTime + world->player->rb.velocity.z;

		world->player->rb.pos = newPos;


		if ((world->player->rb.velocity.x == 0.0f) && (world->player->rb.velocity.z == 0.0f))
		{
			// NOTE(ck): Leave facingDirection whatever it was 
		}
		else if (absoluteValue(world->player->rb.velocity.x) > absoluteValue(world->player->rb.velocity.z))
		{
			if (world->player->rb.velocity.x > 0)
			{
				world->player->facingDir = 0; // right
			}
			else
			{
				world->player->facingDir = 2; // left
			}
		}
		else
		{
			if (world->player->rb.velocity.z > 0)
			{
				world->player->facingDir = 1; // back

			}
			else
			{
				world->player->facingDir = 3; // front
			}

		}
	}

	void UpdateCollider(Collider* collider, v3 colliderPos, v3 scale)
	{
		SetBoxTransform(collider, colliderPos, scale);
		UpdateWorldPosToWorldMatrix(collider);
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
			if (state == State::Debug)
			{
				playMode();
			}
			else if (state == State::Play)
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
		
		bool enabled = (state == State::Play);
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
				if (input.space.endedDown)
					velocity.y = 1.0f;
			}
			// PIPE velocity to function
			movePlayer(&velocity);
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
		

#if 1
		// ============  update DEBUG collider  =============
		v3 colliderPos = { debugEnt.rb.pos.x - (0.5f), debugEnt.rb.pos.y - (0.5),
							debugEnt.rb.pos.z - (0.5f) };
		SetBoxTransform(&debugEnt.collider, colliderPos, debugEnt.data.scale);
		UpdateWorldPosToWorldMatrix(&debugEnt.collider);
		// ==================================================
#endif

		// player collider
		colliderPos = { world->player->rb.pos.x - (0.5f),
					world->player->rb.pos.y - (0.5),
					world->player->rb.pos.z - (0.5f) };

		UpdateCollider(&world->player->collider, colliderPos, world->player->data.scale);

		//
		//	ENTITIES UPDATE
		//
		UpdateEntities(world);


		// 
		// CAMERA UPDATE
		//
		Camera* cam = getCamera(this, currCameraIndex);
		Update(cam, deltaTime, &input,
			   world->player->rb.pos, world->player->rb.orientation, true);

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
		// PLAYER DRAW
		//
		if (drawCollisions)
		{
			MonGL::DrawBoundingBox(&terrain->collider.data, cam, shader.handle);
			MonGL::DrawBoundingBox(&world->player->collider.data, cam, shader.handle);

			// DEBUG =============================================================
			MonGL::DrawBoundingBox(&debugEnt.collider.data, cam, shader.handle);
			// ===================================================================
		}
		MonGL::DrawTerrain(shader.handle, &terrain->mesh, &light, cam, terrain->wireFrame);
		MonGL::Draw(config, &world->player->data, world->player->rb.pos, cam, shader.handle, world->player->facingDir);

		//
		// ENTITIES DRAW
		//

		// DEBUG ======================================================================
		MonGL::Draw(config, &debugEnt.data, debugEnt.rb.pos, cam, shader.handle);
		// ============================================================================

		for (int i = 1; i < world->entityCount; ++i)
		{
			Entity e = world->entities[i];

			if (drawCollisions)
				MonGL::DrawBoundingBox(&e.collider.data, cam, shader.handle);
			
			MonGL::Draw(config, &e.data, e.rb.pos, cam, shader.handle);
		}
		

		MonGL::EndRender();
	}

	void Game::cleanUp()
	{
		// TODO(ck): clean up render data 
		glDeleteVertexArrays(1, &world->player->data.VAO);
		glDeleteBuffers(1, &world->player->data.VBO);
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
		return (state == State::Play);
	}

	void Game::playMode()
	{
		currCameraIndex = 1;
		state = State::Play;
	}

	void Game::debugMode()
	{
		if (currCameraIndex != 1)
			state = State::Debug;
	}
}